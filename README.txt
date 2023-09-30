
Computer Vision HW2
Contains programs p1.cpp, p2.cpp, p3.cpp, p4.cpp, image.cc, DisjSets.cc and its accompanying header files

All parts of the assignment were completed. 
The threshold value to convert to binary is 128
There are bugs in p4.cpp because it fails to recognize one image in particular. It only recognizes one of the two objects correctly based on two attributes: area and roundedness. I set a value threshold of 30%, so if the attributes are plus or minus 30% of the object the program is analyzing, it will mark it as detected.

----------------------
To compile in Linux:
----------
 
   make all


To run:
---------

Make run_p1
Make run_p2
Make run_p3
Make run_p4

-----------

To view .pgm files you can use the open source program gimp:

https://www.gimp.org/
