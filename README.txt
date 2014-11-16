ZK Stereo.exe - Version 1.01, Jan 21, 2000
C. Lawrence Zitnick, clz@cs.cmu.edu

This README.txt file must be accompanied by all copies of this software, ZK Stereo.exe.

This software is in the public domain and is provided AS IS. Use it at YOUR OWN RISK. This software 
is UNSUPPORTED. It is freely available, however this software may not be used for commerical
purposes, i.e. this software may only be used for research and educational purposes. 

Please send any bug reports to Larry Zitnick at clz@cs.cmu.edu.  

The stereo algorithm is as described in:
A Cooperative Algorithm for Stereo Matching and Occlusion Detection, C. Zitnick and T. Kanade
CMU-RI-TR-99-35, Robotics Institute, Carnegie Mellon University, October, 1999
http://www.ri.cmu.edu/pubs/pub_3163.html

The coal stereo image pair is provided courtesy of VASC, Carnegie Mellon University.
http://www.ius.cs.cmu.edu/idb/

Requirements:  Windows NT 4.0 or later, 24 or 32 bit color

ZK Stereo.exe reads in two images files and a directions file named "*.zks".  The image files 
are specified within "*.zks".  

After the stereo data has been computed the following commands may be made:

F1 - Display Image 0
F2 - Display Image 1
F3 - Display Disparity Map
F4 - Display Confidence Map

i - Compute another iteration
p - Print disparity and confidence values into output file.

Left Mouse Click - Display a column slice of L0 values (initial match values)
Right Mouse Click - Display a column slice of Ln values (current match values)


Format for "*.zks"  

s0 s1 i0
MinDisparity 	i1
MaxDisparity 	i2
WinRadL0 	i3
WinRadRC 	i4
WinRadD 	i5
NumIterations 	i6
MaxScaler 	f0
USE_SAD 	i7
Output File 	s2

The above may be repeated so the program can do more than one stereo image pair at a time.

s0 = Name of the first/reference image.  Must be a bmp file.
s1 = Name of the second image, must be rectified (column wise) relative to first image.
i0 = Number of color bands, 1 for b/w and 3 for rgb.
i1 = Minimum disparity value in pixels.	
i2 = Maximum disparity value in pixels.	 If i2 - i1 is too large the program will crash due to a 
	shortage of memory.  A difference of greater than 80 is not recommended.  To make sure 
	disparities are found correctly pad the disparity range by 3 or 5 pixels.
i3 = Window radius used for computing the initial match values L0
	Recommended values = 1 or 2 
i4 = Local Support Radius (row-column dimensions) used for averaging match values during each iteration.
	Recommended values = 1 to 3
i5 = Local Support Radius (disparity dimension) used for averaging match values during each iteration.
	Recommended value = 1
i6 = Number of iterations used to refine the disparity map.
	Recommended values = 5 to 15
f0 = Used for linearly scaling the L0 values when SAD is used.  The worse the image quality 
	(more noise) the lower the value should be.  
	Recommended values = 0.95 to 0.97
i7 = 1 if SAD (sums of absolute differences) is to be used for computing the initial match values.  
	0 if normalized correlation should be used.  If image quality is good SAD (a value of 1) 
	is recommended.
s2 = Name of the output file which the disparity and confidence values will be printed to.

Example:

image0.bmp image1.bmp 1
MinDisparity 	-3
MaxDisparity 	24
WinRadL0 	1
WinRadRC 	2
WinRadD 	1
NumIterations 	10
MaxScaler 	0.96
USE_SAD 	1
Output File 	image.dis



Format of the Output File:

#Columns #Rows
i0 i1
i2 i3
.
.
.

i0 = Disparity value for pixel (0,0) (r,c)
i1 = Confidence value for pixel (0,0)
i2 = Disparity value for pixel (0,1)
i3 = Confidence value for pixel (0,1)

Disparity and confidence values are output horizontally (like a TV scan).




	