//============================================================================
// Name        : basic-camera-cali.cpp
// Author      : Amy Tabb
// Version     : 1.0
// Copyright   : MIT
// Description : Single camera calibration using a chessboard pattern, OpenCV, 4.0.  Adapted from robot-world, hand-eye calibration release
// https://github.com/amy-tabb/RWHEC-Tabb-AhmadYousef
//============================================================================

#include "Includes.hpp"
#include "helper.hpp"
#include <getopt.h>
#include "calibration.hpp"
#include "basic-camera-cali.hpp"

// redo help.

int main(int argc, char** argv) {

	Eigen::initParallel();


	/////////////////////////////************************************/////////////////////////////
	string read_directory = "";
	string image_dir = "";
	string write_directory = "";
	string cali_object_file = "";
	string argument_file = "";
	ifstream in;
	ofstream out;

	int print_help = 0;


	double  chess_mm_height = 0;
	double chess_mm_width = 0;
	int chess_height = 0;
	int chess_width = 0;
	int zero_tangent_dist = 0;
	int zero_k3 = 0;
	int number_threads = omp_get_max_threads();
	float camera_size = 40;
	int create_pattern = 0;

	if (argc == 1){
		print_help = 1;
	}

	while (1)
	{
		static struct option long_options[] =
		{
				{"help",   no_argument,       &print_help, 1},
				{"zero-tangent", no_argument,       &zero_tangent_dist, 1},
				{"zero-k3", no_argument,       &zero_k3, 1},
				{"create-pattern", no_argument, &create_pattern, 1},
				/* These options don’t set a flag.
				             We distinguish them by their indices. */
				{"input",   required_argument, 0, 'a'},
				{"output",  required_argument, 0, 'b'},
				{"number-threads",  required_argument, 0, 'c'},
				{"camera-size",  required_argument, 0, 'd'},
		};


		if (print_help == 1){
			// TODO redo arguments after complete. -- test camera size
			cout << "Printing help for basic-camera-cali" << endl;

			cout << "OPTIONAL FLAGS WITHOUT ARGUMENT -------------------" << endl;
			cout << std::left << setw(30) << "--help" << "No arguments.  Prints this help information." << endl;
			cout << std::left << setw(30) << "--zero-tangent " << "No arguments. In the camera calibration part, sets the tangential components of radial distortion (p1, p2) to zero." << endl;
			cout << std::left << setw(30) << "--zero-k3 " << "No arguments. In the camera calibration part, sets the 3rd radial distortion k value to zero." << endl;
			cout << std::left << setw(30) << "--create-pattern " <<"No arguments. Creates a pattern image given the parameters for chess_height, chess_width in calibration_object.txt." << endl;

			cout << endl;
			cout << "DIRECTORIES AND OPTIONS ----------------------- " << endl;
			cout << std::left << setw(30) << "--input=[STRING] "<< "Mandatory, has to be a directory." << endl;
			cout << std::left << setw(30) << "--output=[STRING] " << "Mandatory, has to be a directory." << endl;
			cout << std::left << setw(30) << "--number-threads=[INT] " << "Optional, number of threads for parallel operational.";
			cout << " Default is omp_get_max_threads() " << endl;
			cout << std::left << setw(30) << "--camera-size=[float] " << "Optional, size of the cameras in the .ply files.  Units will be ";
			cout << "the same units as in calibration object.txt.  In other words, the units of the calibration will be the same units for ";
			cout << "this item.  Default is 40." << endl;

			exit(1);
		}
		/* getopt_long stores the option index here. */
		int option_index = 0;
		int opt_argument;

		opt_argument = getopt_long (argc, argv, "abcd",
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
		case 'c':
			number_threads = FromString<int>(optarg);
			if (number_threads <= 0){
				cout << "number_threads was 0 or negative.  Promoting to 1." << endl;
				number_threads = 1;
			}
			omp_set_num_threads(number_threads);
			break;
		case 'd':
			camera_size = FromString<float>(optarg);
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


	cali_object_file =  read_directory + "calibration_object.txt";

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

	//// IF CREATE /////////////////////

	if (create_pattern){
		CreateChessboardPattern(write_directory, chess_height, chess_width);

	}	else {
		//////////////////////// WRITE ARGUMENTS //////////////////////////////

			ofstream arg_out;
			argument_file =  write_directory + "arguments.txt";

			arg_out.open(argument_file.c_str());

			arg_out << argv[0] << " ";

			if (zero_tangent_dist){
				arg_out << "--zero-tangent ";
			}

			if (zero_k3){
				arg_out << "--zero-k3 ";
			}

			arg_out << "--number-threads=" << number_threads << " ";

			arg_out << "--camera-size=" << camera_size << " ";

			arg_out << "--input=" << read_directory << " ";

			arg_out << "--output=" << write_directory << " " << endl;
			arg_out.close();


		//////////////// DONE READING /////////////

		string filename = write_directory + "details.txt";
		out.open(filename.c_str());


		auto start_timer = std::chrono::high_resolution_clock::now();


		// create calibration object
		CaliObjectOpenCV2 CaliObj (chess,  chess_width, chess_height,
				chess_mm_width, chess_mm_height);

		image_dir = read_directory + "images/";

		if (!CheckExistenceOfDirectory(image_dir)){
			cout << "Please check the README.  A directory named images is required within the input directory." << endl;
			exit(1);
		}


		CaliObj.ReadImages(image_dir);

		if (CaliObj.images.size() == 0){
			cout << "Image directory " << image_dir << "was empty of images.  The code will fail, so quitting now. " << endl;
			exit(1);
		}

		// argument is whether or not to draw the corners
		CaliObj.AccumulateCorners(true);

		CaliObj.Calibrate(out,  write_directory, camera_size, zero_tangent_dist, zero_k3);

		auto end_timer = std::chrono::high_resolution_clock::now();

		out << "Everything ... " << std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer).count() << " seconds " << endl;
		out << "Everything ... " << std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer).count() << " milliseconds " << endl;

		out << " with this many threads " << omp_get_num_threads() << endl;

		out.close();
	}

	return 0;
}

void CreateChessboardPattern(string write_directory, int chess_h, int chess_w){

	int block_size = 100; // 100 pixels per block.
	int border_proportion = 2;
	cv::Size im_size((chess_w + 1)*100 + border_proportion*2*block_size, (chess_h + 1)*100 + border_proportion*2*block_size);
	Mat im(im_size, CV_8U);

	im.setTo(255);

	int start_p = border_proportion*block_size;

	Point p0; Point p1;

	// Outer layer
	for (int block_c = 0; block_c < chess_w + 1; block_c = block_c + 2){

		// the rows.
		for (int block_r = 0; block_r < chess_h + 1; block_r = block_r + 2){
			p0.x = start_p + block_c*block_size;
			p0.y = start_p + block_r*block_size;

			p1.x = start_p + (block_c + 1)*block_size;
			p1.y = start_p + (block_r + 1)*block_size;

			rectangle(im,  p0,  p1,  Scalar(0), -1);
		}
	}

	// Inner layer
	for (int block_c = 1; block_c < chess_w + 1; block_c = block_c + 2){

		// the rows.
		for (int block_r = 1; block_r < chess_h + 1; block_r = block_r + 2){
			p0.x = start_p + block_c*block_size;
			p0.y = start_p + block_r*block_size;

			p1.x = start_p + (block_c + 1)*block_size;
			p1.y = start_p + (block_r + 1)*block_size;

			rectangle(im,  p0,  p1,  Scalar(0), -1);
		}
	}

	string filename = write_directory + "pattern.png";

	imwrite(filename, im);

}
