//============================================================================
// Name        : basic-camera-cali.cpp
// Author      : Amy Tabb
// Version     : 1.0
// Copyright   : MIT
// Description : Single camera calibration using a chessboard pattern, OpenCV, 4.0.  Adapted from robot-world, hand-eye calibration release
// https://github.com/amy-tabb/RWHEC-Tabb-AhmadYousef
//============================================================================

#include "Includes.hpp"
#include "Calibration2.hpp"
#include <getopt.h>




bool CheckExistenceOfDirectory(string write_directory){

	bool exists= true;
	struct stat info;
	if( stat( write_directory.c_str(), &info ) != 0 ){
		cout << "Path to directory is wrong and/or cannot access " << write_directory << endl;
		exists = false;
	}

	return exists;

}


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

	//string read_directory = "";
	//	string write_directory = "";
	//	string image_filename = "";
	//	double homography_scaling = 0;
	//
	//	int write_intermediate = false;
	//
	//	int print_help = 0;
	//


	/////////////////////////////************************************/////////////////////////////
	string read_directory = "";
	string image_dir = "";
	string write_directory = "";
	string cali_object_file;
	ifstream in;
	ofstream out;

	int print_help = 0;


	double  chess_mm_height = 0;
	double chess_mm_width = 0;
	int chess_height = 0;
	int chess_width = 0;

	while (1)
	{
		static struct option long_options[] =
		{
				{"help",   no_argument,       &print_help, 1},
				/* These options don’t set a flag.
				             We distinguish them by their indices. */
				{"input",   required_argument, 0, 'a'},
				{"output",  required_argument, 0, 'b'},
		};


		if (print_help == 1){
			cout << "Printing help for basic-chessboard-cali" << endl;

			cout << "OPTIONAL FLAGS WITHOUT ARGUMENT -------------------" << endl;
			cout << std::left << setw(30) << "--help" << "No arguments.  Prints this help information." << endl;

			cout << endl;
			cout << "DIRECTORIES AND PATHS ----------------------- " << endl;
			cout << std::left << setw(30) << "--input=[STRING] "<< "Mandatory, has to be a directory." << endl;
			cout << std::left << setw(30) << "--output=[STRING] " << "Mandatory, has to be a directory." << endl;



			exit(1);
		}
		/* getopt_long stores the option index here. */
		int option_index = 0;
		int opt_argument;

		opt_argument = getopt_long (argc, argv, "ab",
				long_options, &option_index);

		/* Detect the end of the options. */
		if (opt_argument == -1)
			break;

		switch (opt_argument)
		{
		case 0:
			if (long_options[option_index].flag != 0)
				break;
			printf ("option %s", long_options[option_index].name);
			if (optarg)
				printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 'a':
			read_directory = optarg;
			break;
		case 'b':
			write_directory = optarg;
			break;
		default: {
			cout << "This argument is not handled by the program.  Exiting." << endl; exit(1);
		}
		break;

		}

	}

	if (read_directory.size() == 0){
		cout << "Please enter a read directory --input=[STRING] argument.  Use --help to see the options." << endl; exit(1);
	}

	if (write_directory.size() == 0){
		cout << "Please enter a write directory --output=[STRING] argument.  Use --help to see the options." << endl; exit(1);
	}

	/// check that both directories exist first.
	if (!CheckExistenceOfDirectory(read_directory)){
		exit(1);
	}

	if (!CheckExistenceOfDirectory(write_directory)){
		exit(1);
	}

	EnsureDirHasTrailingBackslash(read_directory);
	EnsureDirHasTrailingBackslash(write_directory);


	cali_object_file =  string(read_directory) + "calibration_object.txt";

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

	string filename = write_directory + "details.txt";
	out.open(filename.c_str());


	// create calibration object
	CaliObjectOpenCV2 CaliObj ( chess_width, chess_height,
			chess_mm_width, chess_mm_height);

	image_dir = read_directory + "images/";

	if (!CheckExistenceOfDirectory(image_dir)){
		cout << "Please check the README.  A directory named images is required within the input directory." << endl;
		exit(1);
	}


	CaliObj.ReadImages(image_dir, 1);

	if (CaliObj.external_images.size() == 0){
		cout << "Image directory " << image_dir << "was empty of images.  The code will fail, so quitting now. " << endl;
		exit(1);
	}

	// argument is whether or not to draw the corners
	CaliObj.AccumulateCornersFlexibleExternal(true);

	CaliObj.CalibrateFlexibleExternal(out,  write_directory);

	out.close();

	return 0;
}
