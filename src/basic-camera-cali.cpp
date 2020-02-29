//============================================================================
// Name        : Basic-camera-cali.cpp
// Author      : Amy Tabb
// Version     : 0.1
// Copyright   : MIT
// Description : Single camera calibration using a chessboard pattern, OpenCV, 4.0.  Adapted from robot-world, hand-eye calibration release
// https://github.com/amy-tabb/RWHEC-Tabb-AhmadYousef
//============================================================================

#include "Includes.hpp"
#include "Calibration2.hpp"



string FindValueOfFieldInFile(string filename, string fieldTag, bool seperator){

	/// reopen file each time, in case things get switched around.  Assume that these are very small files, not the most efficient.

	ifstream in(filename.c_str());

	if (!in.good()){
		cout << "Filename to find " << fieldTag << " is bad " << filename << " quitting !" << endl;
		exit(1);
	}

	string cmp_str;
	string read_str;


	vector<string> tokens;
	string token;
	string return_str = "";
	bool found = false;


	while (in  && found == false){

		in >> token;

		if (token.compare(fieldTag) == 0){
			found = true;

			if (seperator == true && in){
				in >> token;
			}

			if (in){
				in >> return_str;
			}

		}
	}


	cout << "Found! " << found << " field " << fieldTag << " and result " << return_str << endl;
	in.close();

	return return_str;

}



void EnsureDirHasTrailingBackslash(string& write_directory){
	int n_letters = write_directory.size();
	bool eval =  (write_directory[n_letters - 1] == '/');
	cout << "Last character compare " << write_directory << " " <<  eval << endl;
	if (eval == false){
		write_directory = write_directory + "/";
	}

}



int main(int argc, char** argv) {

	Eigen::initParallel();

	/////////////////////////////************************************/////////////////////////////
	string source_dir;
	string image_dir;
	string write_dir;
	string robot_file = "";
	string robot_dir;
	string hand_eye_dir;
	string cali_object_file;
	ifstream in;
	ofstream out;


	double  chess_mm_height = 0;
	double chess_mm_width = 0;
	int chess_height = 0;
	int chess_width = 0;


	cout << "Usage is: function_name input_directory write_directory."  << endl;

	if (argc == 3){
		source_dir =  string(argv[1]);
		write_dir = argv[2];

		EnsureDirHasTrailingBackslash(source_dir);
		EnsureDirHasTrailingBackslash(write_dir);

		cali_object_file =  string(source_dir) + "calibration_object.txt";

	}	else {
		cout << "Wrong number of arguments " << endl;
		exit(1);
	}


	in.open(cali_object_file.c_str());

	if (!in){
		cout << "You forgot the calibration_object.txt file or the path is wrong" << endl << argv[1] << endl;;
		cout << cali_object_file << endl;
		exit(1);
	}

	in.close();

	/////////////////////    READ CALIBRATION ITEMS FROM FILE /////////////////
	string fieldString;
	string returnString;

	fieldString = "chess_mm_height";
	returnString = FindValueOfFieldInFile(cali_object_file, fieldString, false);

	if (returnString.size() != 0){
		chess_mm_height = FromString<double>(returnString);
	}	else {
		cout << "Cannot find " << fieldString << "in " << cali_object_file << endl;
		exit(1);
	}


	fieldString = "chess_mm_width";
	returnString = FindValueOfFieldInFile(cali_object_file, fieldString, false);

	if (returnString.size() != 0){
		chess_mm_width = FromString<double>(returnString);
	}	else {
		cout << "Cannot find " << fieldString << "in " << cali_object_file << endl;
		exit(1);
	}


	fieldString = "chess_height";
	returnString = FindValueOfFieldInFile(cali_object_file, fieldString, false);

	if (returnString.size() != 0){
		chess_height = FromString<int>(returnString);
	}	else {
		cout << "Cannot find " << fieldString << "in " << cali_object_file << endl;
		exit(1);
	}


	fieldString = "chess_width";
	returnString = FindValueOfFieldInFile(cali_object_file, fieldString, false);

	if (returnString.size() != 0){
		chess_width = FromString<int>(returnString);
	}	else {
		cout << "Cannot find " << fieldString << "in " << cali_object_file << endl;
		exit(1);
	}

	//////////////// DONE READING /////////////

	string filename = write_dir + "details.txt";
	out.open(filename.c_str());


	// create calibration object
	CaliObjectOpenCV2 CaliObj ( chess_width, chess_height,
			chess_mm_width, chess_mm_height);

	image_dir = source_dir + "images";
	DIR* dir = opendir(image_dir.c_str());
	if (dir)
	{
		/* Directory exists. */
		closedir(dir);
		CaliObj.ReadImages(image_dir, 1);

		if (CaliObj.external_images.size() == 0){
			cout << "Image directory " << image_dir << "was empty of images.  The code will fail, so quitting now. " << endl;
			exit(1);
		}
	}	else {

		cout << "No input directory exists at " << image_dir << " re-read the README.md, rerun.  Quitting." << endl;
		exit(1);

	}

	DIR* dir_write = opendir(write_dir.c_str());
	if (dir_write){
		closedir(dir_write);
	}	else {
		cout << "No output directory exists at " << write_dir << " re-read the README.md, rerun.  Quitting." << endl;
		exit(1);
	}


	// argument is whether or not to draw the corners
	CaliObj.AccumulateCornersFlexibleExternal(true);

	CaliObj.CalibrateFlexibleExternal(out,  write_dir);

	out.close();

	return 0;
}
