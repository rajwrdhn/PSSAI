#representation of all trams and buses at their respective timings
#for the trams the direction  does not matter as the 
#trams passing will always imply that the street lights for cars of streets 2 and 4 are red.
#But anyways we take that also into consideration while representing because maybe we require it later 
#we would now represent the timings using datetime which contains many functions and class for time manipulation
import datetime
#represents tram 10 coming from street 3 towards 1
Tram10_3_1=[datetime.time(6,2,0),datetime.time(6,19,0),datetime.time(6,29,0),datetime.time(6,49,0),datetime.time(7,4,0),datetime.time(7,19,0),
			datetime.time(7,34,0),datetime.time(7,49,0),datetime.time(8,4,0),datetime.time(8,19,0),datetime.time(8,34,0),datetime.time(8,49,0),
			datetime.time(9,4,0),datetime.time(9,17,0),datetime.time(9,34,0),datetime.time(9,47,0),datetime.time(10,2,0),datetime.time(10,17,0),
			datetime.time(11,17,0),datetime.time(12,17,0),datetime.time(13,17,0),datetime.time(14,17,0),datetime.time(15,17,0),datetime.time(16,17,0),
			datetime.time(17,17,0),datetime.time(10,32,0),datetime.time(11,32,0),datetime.time(12,32,0),datetime.time(13,32,0),datetime.time(14,32,0),
			datetime.time(15,32,0),datetime.time(16,32,0),datetime.time(17,32,0),datetime.time(11,2,0),datetime.time(12,2,0),datetime.time(13,2,0),
			datetime.time(14,2,0),datetime.time(15,2,0),datetime.time(16,2,0),datetime.time(17,2,0),datetime.time(10,47,0),datetime.time(11,47,0),
			datetime.time(12,47,0),datetime.time(13,47,0),datetime.time(14,47,0),datetime.time(15,47,0),datetime.time(16,47,0),datetime.time(17,47,0),
			datetime.time(18,2,0),datetime.time(18,19,0),datetime.time(18,34,0),datetime.time(18,49,0),datetime.time(19,2,0),datetime.time(19,17,0),
			datetime.time(19,32,0),datetime.time(19,47,0),datetime.time(20,2,0),datetime.time(20,17,0),datetime.time(20,32,0),datetime.time(20,47,0)
			]
#represents tram 10 coming from street 1 towards 3
Tram10_1_3=[datetime.time(6,15,0),datetime.time(6,33,0),datetime.time(6,44,0),datetime.time(6,59,0),datetime.time(7,14,0),datetime.time(7,29,0),
			datetime.time(7,44,0),datetime.time(7,59,0),datetime.time(8,14,0),datetime.time(8,29,0),datetime.time(8,44,0),datetime.time(8,59,0),
			datetime.time(9,14,0),datetime.time(9,29,0),datetime.time(9,45,0),datetime.time(10,0,0),datetime.time(11,0,0),datetime.time(12,0,0),
			datetime.time(13,0,0),datetime.time(14,0,0),datetime.time(15,0,0),datetime.time(16,0,0),datetime.time(17,0,0),datetime.time(18,0,0),
			datetime.time(10,15,0),datetime.time(11,15,0),datetime.time(12,15,0),datetime.time(13,15,0),datetime.time(14,15,0),datetime.time(15,15,0),
			datetime.time(16,15,0),datetime.time(17,15,0),datetime.time(18,15,0),datetime.time(10,30,0),datetime.time(11,30,0),datetime.time(12,30,0),
			datetime.time(13,30,0),datetime.time(14,30,0),datetime.time(15,30,0),datetime.time(16,30,0),datetime.time(17,30,0),datetime.time(10,45,0),
			datetime.time(11,45,0),datetime.time(12,45,0),datetime.time(13,45,0),datetime.time(14,45,0),datetime.time(15,45,0),datetime.time(16,45,0),
			datetime.time(17,45,0),datetime.time(18,29,0),datetime.time(18,46,0),datetime.time(19,6,0),datetime.time(19,19,0),datetime.time(19,34,0),
			datetime.time(19,49,0),datetime.time(20,4,0),datetime.time(20,19,0),datetime.time(20,34,0),
			datetime.time(20,49,0)
			]
#represents tram 4 coming from street 3 towards 1
Tram4_3_1=[datetime.time(6,11,0),datetime.time(6,24,0),datetime.time(6,34,0),datetime.time(6,44,0),datetime.time(6,54,0),datetime.time(7,3,0),
			datetime.time(7,11,0),datetime.time(7,21,0),datetime.time(7,33,0),datetime.time(7,41,0),datetime.time(7,51,0),datetime.time(8,3,0),
			datetime.time(8,11,0),datetime.time(8,21,0),datetime.time(8,33,0),datetime.time(8,41,0),datetime.time(8,51,0),datetime.time(9,3,0),
			datetime.time(9,11,0),datetime.time(9,21,0),datetime.time(9,33,0),datetime.time(9,41,0),datetime.time(9,52,0),datetime.time(10,4,0),
			datetime.time(10,12,0),datetime.time(10,22,0),datetime.time(10,34,0),datetime.time(10,42,0),datetime.time(10,52,0),datetime.time(11,4,0),
			datetime.time(11,12,0),datetime.time(11,22,0),datetime.time(11,34,0),datetime.time(11,42,0),datetime.time(11,52,0),datetime.time(12,4,0),
			datetime.time(12,12,0),datetime.time(12,22,0),datetime.time(12,34,0),datetime.time(12,42,0),datetime.time(12,52,0),datetime.time(13,4,0),
			datetime.time(13,12,0),datetime.time(13,22,0),datetime.time(13,34,0),datetime.time(13,42,0),datetime.time(13,52,0),datetime.time(14,4,0),
			datetime.time(14,12,0),datetime.time(14,22,0),datetime.time(14,34,0),datetime.time(14,42,0),datetime.time(14,52,0),datetime.time(15,4,0),
			datetime.time(15,12,0),datetime.time(15,22,0),datetime.time(15,34,0),datetime.time(15,42,0),datetime.time(15,52,0),datetime.time(16,4,0),
			datetime.time(16,12,0),datetime.time(16,22,0),datetime.time(16,34,0),datetime.time(16,42,0),datetime.time(16,52,0),datetime.time(17,4,0),
			datetime.time(17,12,0),datetime.time(17,22,0),datetime.time(17,34,0),datetime.time(17,42,0),datetime.time(17,52,0),datetime.time(18,4,0),
			datetime.time(18,12,0),datetime.time(18,22,0),datetime.time(18,34,0),datetime.time(18,42,0),datetime.time(18,52,0),datetime.time(19,4,0),
			datetime.time(19,12,0),datetime.time(19,22,0),datetime.time(19,34,0),datetime.time(19,42,0),datetime.time(19,52,0),datetime.time(20,4,0),
			datetime.time(20,12,0),datetime.time(20,22,0),datetime.time(20,34,0),datetime.time(20,42,0),datetime.time(20,52,0),
			]
#represents tram 4 coming from street 1 towards 3
Tram4_1_3=[datetime.time(6,8,0),datetime.time(6,22,0),datetime.time(6,28,0),datetime.time(6,41,0),datetime.time(6,51,0),datetime.time(7,14,0),
			datetime.time(7,24,0),datetime.time(7,44,0),datetime.time(7,54,0),datetime.time(8,14,0),datetime.time(8,24,0),datetime.time(8,44,0),
			datetime.time(8,54,0),datetime.time(9,14,0),datetime.time(9,22,0),datetime.time(9,42,0),datetime.time(9,52,0),datetime.time(10,12,0),
			datetime.time(10,22,0),datetime.time(10,42,0),datetime.time(10,52,0),datetime.time(11,12,0),
			datetime.time(11,22,0),datetime.time(11,42,0),datetime.time(11,52,0),datetime.time(12,12,0),
			datetime.time(13,22,0),datetime.time(13,42,0),datetime.time(13,52,0),datetime.time(13,12,0),
			datetime.time(14,22,0),datetime.time(14,42,0),datetime.time(14,52,0),datetime.time(14,12,0),
			datetime.time(15,22,0),datetime.time(15,42,0),datetime.time(15,52,0),datetime.time(15,12,0),
			datetime.time(16,22,0),datetime.time(16,42,0),datetime.time(16,52,0),datetime.time(16,12,0),
			datetime.time(17,22,0),datetime.time(17,42,0),datetime.time(17,52,0),datetime.time(17,12,0),
			datetime.time(12,22,0),datetime.time(12,42,0),datetime.time(12,52,0),datetime.time(18,14,0),
			datetime.time(18,24,0),datetime.time(18,44,0),datetime.time(18,54,0),datetime.time(19,14,0),
			datetime.time(19,24,0),datetime.time(19,44,0),datetime.time(19,54,0),datetime.time(20,14,0),
			datetime.time(20,24,0),datetime.time(20,44,0),datetime.time(18,54,0),
			]

# http://www.openstreetmap.org/relation/3165980#map=17/51.04340/13.78214
# Actually, the day we took the measurement it went from 2 to 1, but that was likely just some alternative routing
# So this is from 3 to 1
Bus64_3_1=[datetime.time(6,3,0),datetime.time(6,16,0),datetime.time(6,28,0),datetime.time(6,43,0),datetime.time(6,58,0),datetime.time(7,3,0),
			datetime.time(7,16,0),datetime.time(7,28,0),datetime.time(7,43,0),datetime.time(7,58,0),datetime.time(8,3,0),datetime.time(8,16,0),
			datetime.time(8,28,0),datetime.time(8,43,0),datetime.time(8,58,0),datetime.time(9,3,0),datetime.time(9,16,0),datetime.time(9,28,0),
			datetime.time(9,43,0),datetime.time(9,58,0),datetime.time(10,3,0),datetime.time(10,16,0),datetime.time(10,28,0),datetime.time(10,43,0),
			datetime.time(10,58,0),datetime.time(11,3,0),datetime.time(11,16,0),datetime.time(11,28,0),datetime.time(11,43,0),datetime.time(11,58,0),
			datetime.time(12,3,0),datetime.time(12,16,0),datetime.time(12,28,0),datetime.time(12,43,0),datetime.time(12,58,0),datetime.time(13,3,0),
			datetime.time(13,16,0),datetime.time(13,28,0),datetime.time(13,43,0),datetime.time(13,58,0),datetime.time(14,3,0),datetime.time(14,16,0),
			datetime.time(14,28,0),datetime.time(14,43,0),datetime.time(14,58,0),datetime.time(15,3,0),datetime.time(15,16,0),datetime.time(15,28,0),
			datetime.time(15,43,0),datetime.time(15,58,0),datetime.time(16,3,0),datetime.time(16,16,0),datetime.time(16,28,0),datetime.time(16,43,0),
			datetime.time(16,58,0),datetime.time(17,3,0),datetime.time(17,16,0),datetime.time(17,28,0),datetime.time(17,43,0),datetime.time(17,58,0),
			datetime.time(18,3,0),datetime.time(18,16,0),datetime.time(18,28,0),datetime.time(18,43,0),datetime.time(18,58,0),datetime.time(19,3,0),
			datetime.time(19,16,0),datetime.time(19,28,0),datetime.time(19,43,0),datetime.time(19,58,0),datetime.time(20,3,0),datetime.time(20,16,0),
			datetime.time(20,28,0),datetime.time(20,43,0),datetime.time(20,58,0)
			]

# See above, this is from 1 to 3
Bus64_1_3=[datetime.time(6,7,0),datetime.time(6,25,0),datetime.time(6,40,0),datetime.time(6,56,0),datetime.time(7,7,0),datetime.time(7,25,0),
			datetime.time(7,40,0),datetime.time(7,56,0),datetime.time(8,7,0),datetime.time(8,25,0),datetime.time(8,40,0),datetime.time(8,56,0),
			datetime.time(9,7,0),datetime.time(9,25,0),datetime.time(9,40,0),datetime.time(9,56,0),datetime.time(10,7,0),datetime.time(10,25,0),
			datetime.time(10,40,0),datetime.time(10,56,0),datetime.time(11,7,0),datetime.time(11,25,0),datetime.time(11,40,0),datetime.time(11,56,0),
			datetime.time(12,7,0),datetime.time(12,25,0),datetime.time(12,40,0),datetime.time(12,56,0),datetime.time(13,7,0),datetime.time(13,25,0),
			datetime.time(13,40,0),datetime.time(13,56,0),datetime.time(14,7,0),datetime.time(14,25,0),datetime.time(14,40,0),datetime.time(14,56,0),
			datetime.time(15,7,0),datetime.time(15,25,0),datetime.time(15,40,0),datetime.time(15,56,0),datetime.time(16,7,0),datetime.time(16,25,0),
			datetime.time(16,40,0),datetime.time(16,56,0),datetime.time(17,7,0),datetime.time(17,25,0),datetime.time(17,40,0),datetime.time(17,56,0),
			datetime.time(18,7,0),datetime.time(18,25,0),datetime.time(18,40,0),datetime.time(18,56,0),datetime.time(19,7,0),datetime.time(19,25,0),
			datetime.time(19,40,0),datetime.time(19,56,0),datetime.time(20,7,0),datetime.time(20,25,0),datetime.time(20,40,0),datetime.time(20,56,0),
			]

# http://www.openstreetmap.org/relation/611113#map=17/51.04285/13.78300
# From 2 to 3
Bus63_2_3=[datetime.time(6,9,0),datetime.time(6,18,0)]

# From 3 to 2
Bus63_3_2=[datetime.time(6,16,0)]
