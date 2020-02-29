# basic-camera-calibration
This is basic camera calibration using a chessboard, OpenCV 4.0, C++. 

Author: Amy Tabb.

28 November 2018, an 2 December 2018.

Major update early March, 2020.

This repository is a companion to a webinar on camera calibration presented on [Plantae](https://plantae.org/), however viewing the webinar is not necessary to use and understand this code.  This code is a cleaned-up version of the camera calibration portion of another repository of mine, for [robot-world, hand-eye calibration](https://github.com/amy-tabb/RWHEC-Tabb-AhmadYousef).   

## What it does: single camera chessboard calibration
 A general sketch of input and output:

* Input 1: a directory of images that view a chessboard pattern from one camera.  It is not necessary for every image to capture the pattern.
* Input 2: a file describing the chessboard pattern: physical dimensions, and number of corners vertically and horizontally
* Output 1: undistorted images of the original images, with the located pattern overlaid.
* Output 2: `details.txt`, with internal calibration information (upper triangular matrix, radial distortion parameters), summed reprojection error, rotation and translation for every image for which a calibration pattern could be located.

## Dependencies

The current version depends on these versions of [OpenCV](http://opencv.org/) and [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page).  The version of Eigen is probably not critical.

OpenCV 4.0 (OpenCV 3.0 may work with changes of some enums - read below)  OpenCV 4.0 requires a [C++11-compliant compiler](https://opencv.org/opencv-4-0-0.html), FYI.

Eigen 3.3.4

As of this writing, the version installed from [github](https://github.com/opencv/opencv) is returning OpenCV 4.2.

This code has been tested on Ubuntu 16.04 and Ubuntu 18.04.  You are welcome to convert it to another OS, but I have not.  While OpenCV is available from distribution repositories, my long experience with it is has always been to build from the source to get the best results.

**Is getting all of this to work on your system too much of a pain and you are interested in a Docker release?  Let me know via an issue on Github!  The squeaky wheel gets the grease.**


## Building 

These instructions are provided to give the basics assuming you have an Eclipse installation, and cloned this git repository.  Experienced users, alter at will.


1. Clone the git repository.  `cd` to the desired directory, then from the command line  -- ` git clone https://github.com/amy-tabb/basic-chessboard-cali.git`.  

2. `$ cd basic-chessboard-cali`

3. The code is provided in a directory called `src`.  

4. Then, go to the Eclipse IDE, CDT (C development toolkit). Go to File->Import->Existing projects into workspace. Select the project in the `basic-chessboard-cali` directory.  This will load the project into Eclipse.  Build from Eclipse, and you're ready to run.  If your Eigen include files, or OpenCV libraries are in a different place than mine, you will have to alter that.  I have included the compiler flag in the miscellaneous section for the G++ Compiler.

5. Bash:  `$ cd Debug` -- assuming the project compiled, and linked, you are ready to run!

## Running

1. The executable using the method above is going to be in the `Debug` folder (unless you do your own setup).  The arguments for the program are:

	1. input directory -- this is where the image directory and specification of the calibration pattern is held.

	2. write directory -- this is where all of the results will be written to file, including undistorted images and the calibration information.
	
At the command line, you'd use something like `$  program_name input_dir output_dir` .

Run time:
It will take some time (on the order of 4 minutes) to perform camera calibration.  This depends on the number of images in the dataset.

## Required structure of the input directory

An example input and output directory, where the results in the output directory were produced from this code, are included in a [Google Drive link (~700MB)](https://drive.google.com/open?id=1_luPe6Cr2pXQBDIyXvIQK89kBRRv0L1L), with the names `input_dir` and `output_dir` within the `TestSet` folder.   

`input_directory` should have two items:

1. a subdirectory called `images` - place all of your images here.

2. `calibration_object.txt` : This file gives the specification of the calibration pattern. The strings `chess_mm_height`, etc. are read by the program. 

Example from the provided file in `input_directory`:

```
	chess_mm_height 26.0
	chess_mm_width 26.0
	chess_height 10
	chess_width 14
```

