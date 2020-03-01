# basic-camera-calibration
This is basic camera calibration using a chessboard, OpenCV 4.0, C++. 

Author: Amy Tabb.

28 November 2018, an 2 December 2018.

Major update early March, 2020.

This repository is a companion to a webinar on camera calibration presented on [Plantae](https://plantae.org/), however viewing the webinar is not necessary to use and understand this code.  This code is a cleaned-up version of the camera calibration portion of another repository of mine, for [robot-world, hand-eye calibration](https://github.com/amy-tabb/RWHEC-Tabb-AhmadYousef).   

Roadmap
- [What it does](#what-it-does-single-camera-chessboard-calibration)
- [Docker release](#docker-release)
- [Dependences](#dependencies)
	- [Tested operating system](#tested-operating-system)
	- [OpenCV 4](#opencv-4)
- [Building](#building)
- [Running camera-as-scanner executable](#running-basic-chessboard-cali-executable)
- [Required structure of the input directory](#required-structure-of-the-input-directory)
- [Contents of the output directory](#contents-of-the-output-directory)
	
## What it does: single camera chessboard calibration
 A general sketch of input and output:

* Input 1: a directory of images that view a chessboard pattern from one camera.  It is not necessary for every image to capture the pattern.
* Input 2: a file describing the chessboard pattern: physical dimensions, and number of corners vertically and horizontally
* Output 1: undistorted images of the original images, with the located pattern overlaid.
* Output 2: `details.txt`, with internal calibration information (upper triangular matrix, radial distortion parameters), summed reprojection error, rotation and translation for every image for which a calibration pattern could be located.

## Docker release

To avoid building the code yourself, a Docker image of this project is available, and the Dockerfile used to generate it is part of this repository.  Docker is not always easy to parse, though, so whichever path you take is largely situation dependent.

### Install Docker

[Install Docker](https://docs.docker.com/install/), if you haven't already.  I endorse uninstalling old versions if you have them floating around.

### Pull the image

The image for basic-camera-calibration is : [amytabb/basic-camera-calibration](https://hub.docker.com/r/amytabb/basic-camera-calibration).

```bash
docker pull amytabb/basic-camera-calibration
```

### Run the image

This code will write the results to disk; to do so with Docker means that we need to mount a portion of your hard drive to a volume in the Docker image.

I used a bind mount below; the Docker image's volume is `host_dir` and will not change no matter which machine or dataset you run it on.  `/full/file/path/on/your/machine` is the directory that you want the reading and writing to occur.  

Example:

```bash
sudo docker run -v /full/file/path/on/your/machine:/host_dir -it amytabb/basic-camera-calibration:latest bash
```

The bind mount is potentially confusing, so here is an example.  Say I have a directory `/home/amy/Data/March/` and within `March` is a directory of images that I want to process with basic-camera-calibration.  I also want to write to a directory within `/home/amy/Data/March/`.  So, 

```bash
sudo docker run -v /home/amy/Data/March:/host_dir -it basic-camera-calibration:latest bash
```

Creates a container with all of the libraries and a Ubuntu 18.04 operating system, and bash shell (command line), and may look something like:

```bash
root@f6feb7ce8c31:/host_dir# 
```

but if you take a look at the contents of `/host_dir`, with `ls`, they are `/home/amy/Data/March/`.  That's the bind mount magic.

First, suppose we forgot to create the write directory.  No problem.

```bash
root@f6feb7ce8c31:/host_dir# mkdir write-dir
```

creates our write directory `write-dir`.

And from here on out, we issue commands from this Docker container, which is writing to our filesystem.  Skip to [Running camera-as-scanner executable](#running-basic-chessboard-cali-executable) to get details on how to run the code.  The only difference is that `./` is not needed before commands when using the Docker version.

## Dependencies

This code uses the OpenCV 4.0, Eigen and is written in C++. 

### Tested operating system

This code has been tested on Ubuntu 16.04 and Ubuntu 18.04.  You are welcome to convert it to Windows, but I have not.  While OpenCV is available from distribution repositories, my long experience with it is has always been to build from the source to get the best results.

### OpenCV 4

While the OpenCV libraries are avaliable from package managers, my experience with it that best results happen when it is built from source.

These libraries need to be installed: 

- `opencv_core`
- `opencv_imgproc`
- `opencv_imgcodecs` 
- `opencv_calib3d`
- `opencv_highgui`


## Building 

To build, you'll also need Cmake.  Alternate methods are available, but cmake is faster.  On Ubuntu, 

```` bash
sudo apt-get install cmake
````


1. Clone the git repository to a desired location.. 

```bash
git clone https://github.com/amy-tabb/basic-camera-calibration.git
```

Then change to the `basic-camera-calibration`.  
 
```bash
cd basic-camera-calibration
```

2. Create a `build` folder (or something similar), and `cd` into it: `cd build`.

3. Configure with cmake.  Don't have cmake? (`sudo apt-get install cmake`). Then from the build folder, you can use any of the following four options below: 

- `cmake ../src`  (basic)
- `cmake  -DCMAKE_BUILD_TYPE=Release ../src` (Release configuration)
- `cmake  -DCMAKE_BUILD_TYPE=Debug ../src` (Debug configuration)
- `cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE ../src/` (Create an Eclipse project to import -- it will be in the `build` folder)

In case you have installed OpenCV and cmake can't find it, you need to specify the location of `OpenCVConfig.cmake`.  Don't know where it is?   Find out with `locate OpenCVConfig.cmake`.  Then append

`-DCMAKE_PREFIX_PATH=dir`

in my case on one machine, this was:

`-DCMAKE_PREFIX_PATH=/usr/local/opencv41/lib/cmake/opencv4/`

where `/usr/local/opencv41/lib/cmake/opencv4/` is the directory containing `OpenCVConfig.cmake`.  Of course, you will substitute whatever the approrpriate directory returned from  `locate OpenCVConfig.cmake` was.

4. Then, you can either import the project to Eclipse (if you used the last option), and build from there, or type `make`.   If the everything compiled and linked, and you have an executable named `basic-chessboard-cali`, you are ready to go.

5. You can download an input dataset from [Box, ~170MB](https://app.box.com/s/7zbc1098797ubk8mh0q9lg3ual007f3j); this dataset is in the format needed for the code, mainly the text file with the specifications.  If you need to check your output, there is the corresponding output dataset also on [Box, ~170MB](https://app.box.com/s/bsg2vm6o3uzqlbbgzfk6w9e27veyel0l).

[6.]  You can run `make install` to install this code to your system. However, this is optional.  To change the installation directory, add `-DCMAKE_INSTALL_PREFIX=/your/preferred/dir` to the cmake call, or alter this option in cmake-gui.

### Alternates

If you want to use an alternate to cmake, the `basic-chessboard-cali` executable requires Eigen3 (a header-only library) and the following OpenCV libraries:

- opencv_core, opencv_highgui, opencv_imgproc, opencv_imgcodecs, opencv_calib3d.

And OpenCV 4.xx requires a C++11 compiler or higher.  

The `aruco-pattern-write` executable requires the following OpenCV libraries:

- opencv_core, opencv_imgcodecs, opencv_aruco.

The only file in the source folder needed for the `aruco-pattern-write` executable is the `aruco-pattern-write-project.cpp` file.  All of the `*.cpp` and `*.hpp` files are for the `camera-as-scanner` executable.


## Running basic-chessboard-cali executable

 Provided you're built the project using the provided cmake setup, you will get the `basic-chessboard-cali` executable. This program has three mandatory arguments and one optional one.  You can see the format by running with the `--help` flag.  

```bash
./basic-chessboard-cali --help

Printing help for camera-as-scanner
OPTIONAL FLAGS WITHOUT ARGUMENT -------------------
--help                        No arguments.  Prints this help information.

DIRECTORIES AND PATHS ----------------------- 
--input=[STRING]              Mandatory, has to be a directory.
--output=[STRING]             Mandatory, has to be a directory.
```
The aguments are : 
- `--input` = read directory, 
- `--output` = write directory,


An example valid run command is:

```
./basic-chessboard-cali --input /my/input/dir --output /my/output/dir
```

Run time:
It will take some time (on the order of 4 minutes) to perform camera calibration.  This depends on the number of images in the dataset.

## Required structure of the input directory

Again, examples of an input directory properly formatted is here: [Box, ~170MB](https://app.box.com/s/7zbc1098797ubk8mh0q9lg3ual007f3j); this dataset is in the format needed for the code, mainly the text file with the specifications.  

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

The output directory should initially be empty.  If you need to check your output, there is the corresponding output dataset also on [Box, ~170MB](https://app.box.com/s/bsg2vm6o3uzqlbbgzfk6w9e27veyel0l).

## Contents of the output directory

The output directory consists of a file `details.txt` with items like:

- the number of internal images used for the calibration -- not relevant here, this is an artifact from another project.
- the number of images used when calibrating for intrinsic and extrinsic parameters (second line).
- rms = root mean square error of projection over all images where the pattern was found.
- camera matrix is the intrinsic camera calibration matrix
- Distorion - distortion coefficients.
- `Rt for cam 0` is the extrinsic camera calibration matrix (i.e. pose of the camera, rotation and translation) for image 0 in this case.  These are only listed for those images where a pattern could be detected.  

```
Internal images used: 0
External images used: 127
rms 0.131539
camera matrix 
1414.34 0 974.309 
0 1416.91 605.279 
0 0 1 
Distortion 
-0.163967 0.119912 -0.000319744 -0.000118378 -0.0293744 

Summed reproj error 210.745

Rt for cam 0
-0.999386 0.0337716 -0.00929798 810.926 
-0.0319937 -0.988133 -0.150229 217.661 
-0.0142611 -0.149839 0.988607 1657.55 
```
