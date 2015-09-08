#!/usr/bin/python3

import array
import datetime
import math
import sys


def ffs(value):
    if value == 0:
        return None
    if (value & (value - 1)) != 0:
        value &= ~(value - 1)
    return int(math.log(value, 2))

# Schedules are implemented as arrays of bit fields.
# Every traffic light has its own bit, with a 1 indicating the light being
# green. Thus, a bit field represents the state of all traffic lights at one
# point in time.
# (Note that "yellow" is just the end of the green phase for our purposes.)
# The schedule is thus an arrow of these fields, one for each point in time the
# schedule covers.


# time resolution
TR = 1 # schedule entries per second


from trambus2 import *

# Bitmask for each time unit of the day which traffic light states are important
# regarding the trams/busses
trambus_day_tl_mask  = None
# Value the traffic light state bit field is supposed to have after applying the
# bitmask
trambus_day_tl_value = None


# Bit shifts for each of the traffic lights
# (TLC_* .. traffic light for cars; TLP_* .. traffic light for pedestrians)
# (*_L .. left; *_SR .. straight/right)

TLC_EAST_SR     =  0
TLC_NORTH_SR    =  1
TLC_WEST_SR     =  2
TLC_SOUTH_SR    =  3

TLC_EAST_L  =  4
TLC_NORTH_L =  5
TLC_WEST_L  =  6
TLC_SOUTH_L =  7

TLP_EAST    =  8
TLP_NORTH   =  9
TLP_WEST    = 10
TLP_SOUTH   = 11

TLP_MIN_GREEN_TIME = [
    round(21.45 * TR + 0.5), # east:  21.45 s
    round(13.50 * TR + 0.5), # north: 13.50 s
    round(19.20 * TR + 0.5), # west:  19.20 s
    round(16.80 * TR + 0.5)  # south: 16.80 s
]



tl_conflict_mask = None
tl_config_valid  = None


def build_conflict_mask():
    global tl_conflict_mask
    tl_conflict_mask = 12 * [0]

    # Every straight/right TL:
    # - conflicts with every straight/right, except the opposite
    # - conflicts with every left turn, except its own
    # - conflicts with the opposite road's pedestrian TL
    # Every left turn TL:
    # - conflicts with every straight/right, except its own
    # - conflicts with every left turn, except the opposite
    # - conflicts with the target road's pedestrian TL

    # Straight/right car traffic lights
    for i in range(4):
        # conflict masks
        straight    = 0xf
        left        = 0xf
        pedestrian  = 0xf

        # Cannot conflict with itself
        straight &= ~(1 << i)
        # Does not conflict with opposite
        straight &= ~(1 << ((i + 2) % 4))

        # Does not conflict with own left turn
        left &= ~(1 << i)

        # Conflicts with parallel roads' pedestrian TLs
        pedestrian &= ~(1 << ((i + 1) % 4))
        pedestrian &= ~(1 << ((i + 3) % 4))

        tl_conflict_mask[i] =  straight \
                            | (left << 4) \
                            | (pedestrian << 8)

    # Left turn car traffic lights
    for i in range(4):
        straight    = 0xf
        left        = 0xf
        pedestrian  = 0xf

        # Does not conflict with own straight/right
        straight &= ~(1 << i)

        # Cannot conflict with itself
        left &= ~(1 << i)
        # Does not conflict with opposite
        left &= ~(1 << ((i + 2) % 4))

        # Does not conflict with the opposite road or the one to the right
        pedestrian &= ~(1 << ((i + 1) % 4))
        pedestrian &= ~(1 << ((i + 2) % 4))

        tl_conflict_mask[i + 4] =  straight \
                                | (left << 4) \
                                | (pedestrian << 8)

    # Pedestrian traffic lights
    for i in range(4):
        # They don't conflict with each other; so just collect the car TL
        # conflicts from what we've built already
        for j in range(8):
            if tl_conflict_mask[j] & (1 << (i + 8)):
                tl_conflict_mask[i + 8] |= 1 << j

    # Sanity check: Check symmetry
    for i in range(12):
        for j in range(12):
            if ((tl_conflict_mask[i] >> j) & 1) != ((tl_conflict_mask[j] >> i) & 1):
                print("Error: Asymmetric conflict mask,", i, "and", j, file=sys.stderr)
                print(tl_conflict_mask, file=sys.stderr)
                print(bin(tl_conflict_mask[i]), "vs.", bin(tl_conflict_mask[j]), file=sys.stderr)
                exit(1)

    # Build tl_config_valid
    global tl_config_valid
    tl_config_valid = array.array('B')

    for i in range(2 ** 12):
        valid = True
        for j in range(12):
            if i & (1 << j) and i & tl_conflict_mask[j]:
                valid = False
                break
        tl_config_valid.append(valid)


def build_trambus_schedules():
    global trambus_day_tl_mask
    global trambus_day_tl_value

    trambus_day_tl_mask = array.array('h')
    trambus_day_tl_value = array.array('h')
    for i in range(86400 * TR):
        trambus_day_tl_mask.append(0)
        trambus_day_tl_value.append(0)

    for time in Tram10_3_1 + Tram4_3_1 + Bus64_3_1:
        # Ensure east's straight is green and left is red; all other traffic
        # lights we don't have to care about because they'll conflict (or won't)
        # with the green light anyway.
        index = ((time.hour * 60 + time.minute) * 60 + time.second) * TR
        trambus_day_tl_mask[index]  |= (1 << TLC_EAST_SR) | (1 << TLC_EAST_L)
        trambus_day_tl_value[index] |= (1 << TLC_EAST_SR)

    for time in Tram10_1_3 + Tram4_1_3 + Bus64_1_3:
        index = ((time.hour * 60 + time.minute) * 60 + time.second) * TR
        trambus_day_tl_mask[index]  |= (1 << TLC_WEST_SR) | (1 << TLC_WEST_L)
        trambus_day_tl_value[index] |= (1 << TLC_WEST_SR)

    for time in Bus64_2_1:
        index = ((time.hour * 60 + time.minute) * 60 + time.second) * TR
        trambus_day_tl_mask[index]  |= (1 << TLC_SOUTH_L)
        trambus_day_tl_value[index] |= (1 << TLC_SOUTH_L)

    for time in Bus64_1_2:
        index = ((time.hour * 60 + time.minute) * 60 + time.second) * TR
        trambus_day_tl_mask[index]  |= (1 << TLC_WEST_SR)
        trambus_day_tl_value[index] |= (1 << TLC_WEST_SR)

    for time in Bus63_3_2:
        index = ((time.hour * 60 + time.minute) * 60 + time.second) * TR
        trambus_day_tl_mask[index]  |= (1 << TLC_EAST_L)
        trambus_day_tl_value[index] |= (1 << TLC_EAST_L)

    for time in Bus63_2_3:
        index = ((time.hour * 60 + time.minute) * 60 + time.second) * TR
        trambus_day_tl_mask[index]  |= (1 << TLC_SOUTH_SR)
        trambus_day_tl_value[index] |= (1 << TLC_SOUTH_SR)

    # FIXME: We should actually make sure that we didn't accidentally set
    # TLC_*_L in _tl_value when it wasn't set before but was set in _tl_mask
    # (i.e. some other tram/bus says it should actually be red). But we'll just
    # assume this won't happen, and if it does, that the traffic light will
    # perform some magic that allows the straight-going tram/bus to pass before
    # the left turn traffic light is switched to green.

    for i in range(86400 * TR):
        if not tl_config_valid[trambus_day_tl_value[i]]:
            print("Error: Bus/tram schedule overlap at %02i:%02i:%02i" % (i // 3600, (i // 60) % 60, i % 60), file=sys.stderr)
            exit(1)


class Schedule:
    def __init__(self, seconds, start_time):
        self.data = array.array('h')
        for i in range(seconds * TR):
            self.data.append(0)
        self.length = self.data.buffer_info()[1]
        self.start_index = (start_time.hour * 60 + start_time.minute) * 60 * TR
        self.end_index = self.start_index + self.length

    def initial_constraints(self):
        # c) Between switching of green phases consider 3 sec of red phase for
        #    all directions.
        # d) Consider a maximum waiting time of 2 min always

        i = self.start_index
        last_index = i
        last_config = 0
        while i < self.end_index:
            while i < self.end_index and not trambus_day_tl_value[i]:
                i += 1
            if trambus_day_tl_value[i] & ((1 << TLC_EAST_SR) | (1 << TLC_WEST_SR)):
                this_config = 0
            elif trambus_day_tl_value[i] & ((1 << TLC_EAST_L) | (1 << TLC_WEST_L)):
                this_config = 1
            elif trambus_day_tl_value[i] & ((1 << TLC_NORTH_SR) | (1 << TLC_SOUTH_SR)):
                this_config = 2
            elif trambus_day_tl_value[i] & ((1 << TLC_NORTH_L) | (1 << TLC_SOUTH_L)):
                this_config = 3
            else:
                break

            time = i - last_index
            changes = (this_config - last_config) % 4

            # FIXME: Works with the 17:00 schedule by accident...
            if time == 0 and changes == 0:
                i += 1
                continue

            while changes == 0 or time / changes > 30:
                changes += 4
            minimal_time = changes // 4 * 53
            for cycle_i in range(last_config, last_config + changes % 4):
                if cycle_i % 4 == 0:
                    minimal_time += 20
                elif cycle_i % 4 == 2:
                    minimal_time += 25
                else:
                    minimal_time += 4
            if time < minimal_time:
                print("Error building initial schedule", file=sys.stderr)
                print(time, changes, time / changes, minimal_time, file=sys.stderr)
                print("%02i:%02i:%02i" % (i // 3600, (i // 60) % 60, i % 60), file=sys.stderr)
                exit(1)

            average_time = time / changes
            if average_time >= 25:
                times = [None] * changes
                remaining_time = time

                for j in range(changes):
                    times[j] = round(remaining_time / (changes - j))
                    remaining_time -= times[j]
            elif changes >= 4:
                times = [None] * changes
                remaining_time = time

                rel_i = changes // 4 * 4
                for cycle_i in range(last_config, last_config + changes % 4):
                    if cycle_i % 4 == 0:
                        times[rel_i] = 20
                    elif cycle_i % 4 == 2:
                        times[rel_i] = 25
                    else:
                        times[rel_i] = 4
                    remaining_time -= times[rel_i]
                    rel_i += 1

                config = last_config
                full_cycle_changes = changes // 4 * 4

                for j in range(full_cycle_changes):
                    average_time = remaining_time / (full_cycle_changes - j)
                    if average_time >= 25:
                        times[j] = round(average_time)
                    else:
                        minimal_time = remaining_time - 30 * (full_cycle_changes - j - 1)
                        if config == 0:
                            times[j] = max(minimal_time, 20)
                        elif config == 2:
                            times[j] = max(minimal_time, 25)
                        else:
                            times[j] = max(minimal_time, 4)
                    remaining_time -= times[j]
                    config = (config + 1) % 4
            else:
                times = [None] * changes
                remaining_time = time

                rel_i = 0
                for cycle_i in range(last_config, last_config + changes):
                    if cycle_i % 4 == 0:
                        times[rel_i] = 20
                    elif cycle_i % 4 == 2:
                        times[rel_i] = 25
                    else:
                        times[rel_i] = 4
                    remaining_time -= times[rel_i]
                    rel_i += 1

                times[rel_i - 1] += remaining_time

            config = last_config
            time = last_index - self.start_index
            for length in times:
                for j in range((length - 3) * TR):
                    if config == 0:
                        for tl in [TLC_EAST_SR, TLC_WEST_SR, TLP_NORTH, TLP_SOUTH]:
                            self.make_green(time + j, tl)
                    elif config == 1:
                        for tl in [TLC_EAST_L, TLC_WEST_L]:
                            self.make_green(time + j, tl)
                    elif config == 2:
                        for tl in [TLC_NORTH_SR, TLC_SOUTH_SR, TLP_EAST, TLP_WEST]:
                            self.make_green(time + j, tl)
                    else:
                        for tl in [TLC_NORTH_L, TLC_SOUTH_L]:
                            self.make_green(time + j, tl)

                config = (config + 1) % 4
                time += length

            if config != this_config:
                print("FIXME: config != this_config", file=sys.stderr)
                print(config, this_config, file=sys.stderr)
                exit(1)
            if time + self.start_index != i:
                print("FIXME: time + self.start_index != i", file=sys.stderr)
                print(time + self.start_index, i, file=sys.stderr)
                exit(1)

            last_config = this_config
            last_index = i
            i += 1

        # And now enter the last one
        config - last_config
        time = last_index - self.start_index
        while time < self.length:
            for i in range(min(self.length - time, 27 * TR)):
                if config == 0:
                    for tl in [TLC_EAST_SR, TLC_WEST_SR, TLP_NORTH, TLP_SOUTH]:
                        self.make_green(time + i, tl)
                elif config == 1:
                    for tl in [TLC_EAST_L, TLC_WEST_L]:
                        self.make_green(time + i, tl)
                elif config == 2:
                    for tl in [TLC_NORTH_SR, TLC_SOUTH_SR, TLP_EAST, TLP_WEST]:
                        self.make_green(time + i, tl)
                else:
                    for tl in [TLC_NORTH_L, TLC_SOUTH_L]:
                        self.make_green(time + i, tl)

            config = (config + 1) % 4
            time += 30

        # XXX: This assumes the best strategy is having the following basic
        # schedule:
        # - east+west straight/right
        # - east+west left
        # - north+south straight/right
        # - north+south left
        # XXX: Also, it's really bad.
        # (But note: The nice thing about this is that it works with trams,
        #  because the left turn is red while straight is green)
        #for i in range(0, self.length, 120 * TR):
        #    for offset in range(0, 120 * TR, 60 * TR):
        #        tl = offset // (60 * TR)
        #        # d)
        #        self.make_green(i + offset, tl)
        #        self.make_green(i + offset, tl + 2)
        #        self.make_green(i + offset + 30 * TR, tl + 4)
        #        self.make_green(i + offset + 30 * TR, tl + 6)
        #        # c) -- switch off pedestrian lights
        #        for j in range(1, 3 * TR + 1):
        #            self.data[i + offset + j] &= 0xff
        #            self.data[i + offset + 30 * TR - j] &= 0xff
        #            self.data[i + offset + 30 * TR + j] &= 0xff
        #            self.data[i + offset + 60 * TR - j] &= 0xff

        #for i in range(self.start_index, self.end_index):
        #    if trambus_day_tl_value[i]:
        #        self.make_green(i - self.start_index, ffs(trambus_day_tl_value[i]))

        if not self.satisfied(True):
            print("Error: Initial schedule does not satisfy contraints", file=sys.stderr)
            exit(1)

    def make_green(self, second, traffic_light):
        # Make it green
        self.data[second] |= (1 << traffic_light)
        # Make all conflicting lights red
        self.data[second] &= ~tl_conflict_mask[traffic_light]

    def satisfied(self, verbose=False):
        # a) Pedestrian lights need a minimal time of green phase
        # Note: We don't have to care about the red phase for pedestrian TLs;
        # just make the final ten seconds of their green phase red. It's the
        # same thing.
        green_time = 4 * [0]
        for i in range(self.length):
            configuration = self.data[i]
            for j in range(4):
                if configuration & (1 << (j + 8)):
                    green_time[j] += 1
                elif green_time[j] > 0 and green_time[j] < TLP_MIN_GREEN_TIME[j]:
                    if verbose:
                        print("Error: Schedule does not satisfy constraint A", file=sys.stderr)
                        print(i, j, configuration, green_time, file=sys.stderr)
                    return False

        # 0) Two conflicting directions cannot be green at the same time
        # b) It is not allowed to have green lights for pedestrians when the
        #    cars can cross their way, except if both go in the same direction.
        for i in range(self.length):
            if not tl_config_valid[self.data[i]]:
                if verbose:
                    print("Error: Schedule does not satisfy contraints 0/B", file=sys.stderr)
                return False

        # c) Between switching of green phases consider 3 sec of red phase for
        #    all directions.
        # (Note: This breaks condition "there needs to be a green light
        #  anywhere", so we'll just ignore that one)
        last_configuration = None
        first_totally_red = None
        for i in range(self.length):
            configuration = self.data[i]
            if last_configuration is not None and configuration != last_configuration:
                if last_configuration:
                    if configuration:
                        if verbose:
                            print("Error: Schedule does not satisfy constraint C", file=sys.stderr)
                            print("(Immediate green to green switch)", file=sys.stderr)
                        return False
                    first_totally_red = i
                else:
                    if i - first_totally_red < 3 * TR:
                        if verbose:
                            print("Error: Schedule does not satisfy constraint C", file=sys.stderr)
                            print("(Intermediate red phase too short)", file=sys.stderr)
                        return False
            last_configuration = configuration

        # d) Consider a maximum waiting time of 2 min always
        red_time = 12 * [0]
        for i in range(self.length):
            configuration = self.data[i]
            for j in range(8):
                if configuration & (1 << j):
                    red_time[j] = 0
                else:
                    red_time[j] += 1
                    if red_time[j] > 120 * TR:
                        if verbose:
                            print("Error: Schedule does not satisfy contraint D", file=sys.stderr)
                        return False

        # e) The Buses and Trams should always pass when they are scheduled
        # (Note: Since the interpretation "always pass exactly when they are
        #  scheduled" fits this constraint, and it hopefully makes things
        #  easier, we are going to employ it, even though it is pretty stupid
        #  (a realistic interpretation would be "should be able to pass within a
        #   10 second window 30 seconds after the scheduled time"))
        for i in range(self.start_index, self.end_index):
            if (self.data[i - self.start_index] & trambus_day_tl_mask[i]) != trambus_day_tl_value[i]:
                if verbose:
                    print("Error: Schedule does not satisfy constraint E", file=sys.stderr)
                    print(i - self.start_index, "(", self.data[i - self.start_index], "&", trambus_day_tl_mask[i], "!=",
                          trambus_day_tl_value[i], ")", file=sys.stderr)
                return False

        return True


build_conflict_mask()
build_trambus_schedules()


pre_morning_rush  = Schedule(int(1.5 * 60 * 60), datetime.time( 6,  0, 0)) #  6:00 –  7:30
morning_rush      = Schedule(int(2.0 * 60 * 60), datetime.time( 7, 30, 0)) #  7:30 –  9:30
day               = Schedule(int(7.5 * 60 * 60), datetime.time( 9, 30, 0)) #  9:30 – 17:00
evening_rush      = Schedule(int(2.0 * 60 * 60), datetime.time(17,  0, 0)) # 17:00 – 19:00
post_evening_rush = Schedule(int(2.0 * 60 * 60), datetime.time(19,  0, 0)) # 19:00 – 21:00

full_day = Schedule(15 * 60 * 60, datetime.time(6, 0, 0)) # 6:00 – 21:00

for sched in pre_morning_rush, morning_rush, day, evening_rush, post_evening_rush, full_day:
    sched.initial_constraints()
