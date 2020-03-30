/*
 * Calibration2.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: atabb
 *      Updated: May 25, 2018 to use Eigen instead of newmat.
 *      atabb
 */
#include "calibration.hpp"
#include "DirectoryFunctions.hpp"
#include "helper.hpp"

using namespace cv;



void CaliObjectOpenCV2::ReadImages(string directory){
	cv::Mat im;
	vector<string> im_names;
	string filename;
	string txt_ext = "txt";

	im_names.clear();

	cout << "Current dir " << directory << endl;
	ReadDirectory(directory, im_names);


	int num_images = im_names.size();
	images.resize(num_images);

#pragma omp parallel for private (filename, im)
	for (int c = 0; c < num_images; c++){
		filename = directory + im_names[c];
#pragma omp critical
		{

			cout << "Image name " << im_names[c] << endl;
		}
		{
			im = cv::imread(filename.c_str());
			images[c] = im;
		}
	}
}


//bool CaliObjectOpenCV2::AccumulateCorners(bool draw_corners){
//
//	cv::Mat im, gimage, result;
//	string current_name;
//	bool corner_found;
//	bool some_found = false;
//	string filename;
//
//	int corner_count = chess_h*chess_w;
//
//	vector<cv::Point2f> pointBuf;
//	cv::Point2f first_point, last_point;
//	vector<cv::Point2f> flipped_points(corner_count);
//
//	cv::Size boardsize;
//	boardsize.height = chess_h;
//	boardsize.width = chess_w;
//
//	number_internal_images_written = 0;
//	cout << "Doing internal images now .... " << endl;
//	for (int i = 0; i <  int(internal_images.size()); i++){
//		cout << "Looking for corners " << i << endl;
//		//for (int i = 0; i < int(images_to_process.size()); i++){
//		// find corners
//
//		im = internal_images[i];
//		cv::cvtColor(im, gimage, CV_BGR2GRAY);
//
//		// This is a difference in between OpenCV current version and past.
//		corner_found = cv::findChessboardCorners(gimage, boardsize, pointBuf,  CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK + CALIB_CB_FILTER_QUADS);
//
//		if (corner_found) {
//
//			// need to flip the orientation, possibly ...
//			first_point = pointBuf[0];
//			last_point = pointBuf[chess_w*chess_h - 1];
//
//			if (first_point.y < last_point.y){
//				cout << "WRONG ORIENTATION! " << endl;
//				for (int k=0; k<corner_count; k++) {
//
//					flipped_points[k] = pointBuf[chess_w*chess_h - 1 - k];
//
//				}
//
//				pointBuf.swap(flipped_points);
//
//			}
//
//			some_found = true;
//			//			// refine the corner positions
//			cornerSubPix(gimage, pointBuf, Size(11, 11), Size(-1, -1),
//					TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
//
//
//			// draw detected corner points in the current frame
//			cv::drawChessboardCorners(internal_images[i], boardsize, pointBuf, true);
//
//			im_points.push_back(pointBuf);
//
//			cout << "Number of patterns " << im_points.size() << endl;
//			number_internal_images_written++;
//		}
//	}
//
//
//
//	cout << "Doing external images now .... " << endl;
//	for (int i = 0; i <  int(external_images.size()); i++){
//		cout << "Looking for corners " << i << endl;
//		// find corners
//
//		im = external_images[i];
//		cv::cvtColor(im, gimage, CV_BGR2GRAY);
//
//		cv::imwrite("gray.png", gimage);
//
//		corner_found = cv::findChessboardCorners(gimage, boardsize, pointBuf,  CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK + CALIB_CB_FILTER_QUADS);
//
//		if (corner_found) {
//
//			// need to flip the orientation, possibly ...
//
//			//corners[chess_w*chess_h];
//
//			first_point = pointBuf[0];
//			last_point = pointBuf[chess_w*chess_h - 1];
//
//			if (first_point.y < last_point.y){
//				cout << "WRONG ORIENTATION! " << endl;
//
//				for (int k=0; k<corner_count; k++) {
//
//					flipped_points[k] = pointBuf[chess_w*chess_h - 1 - k];
//				}
//
//				pointBuf.swap(flipped_points);
//			}
//
//			some_found = true;
//			// refine the corner positions
//			cv::cornerSubPix( gimage, pointBuf, cv::Size(11,11),
//					cv::Size(-1,-1), cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
//
//			if (draw_corners){
//				// draw detected corner points in the current frame
//				cv::drawChessboardCorners(external_images[i], boardsize, cv::Mat(pointBuf), true);
//			}
//
//			im_points.push_back(pointBuf);
//
//			cout << "Number of patterns " << im_points.size() << endl;
//		}	else {
//			cout << "WARNING -- EXTERNAL NOT FOUND! " << endl;
//			exit(1);
//		}
//	}
//
//
//	if (external_images.size() > 0){
//		image_size = external_images[0].size();
//	}
//
//
//	if (external_images.size() > 0){
//		image_size = external_images[0].size();
//	}
//
//	return some_found;
//
//}


// todo rename, counter for how many patterns have been found.

//bool CaliObjectOpenCV2::AccumulateCorners(bool draw_corners){
//
//	cv::Mat im, gimage, result;
//	string current_name;
//	bool corner_found;
//	bool some_found = false;
//	string filename;
//
//	int corner_count = chess_h*chess_w;
//
//	vector<cv::Point2f> pointBuf;
//	cv::Point2f first_point, last_point;
//	vector<cv::Point2f> flipped_points(corner_count);
//
//	cv::Size boardsize;
//	boardsize.height = chess_h;
//	boardsize.width = chess_w;
//
//	for (int i = 0; i <  int(images.size()); i++){
//		cout << "Looking for corners " << i << endl;
//
//		// find corners
//
//		im = images[i];
//		cv::cvtColor(im, gimage, CV_BGR2GRAY);
//
//
//		corner_found = cv::findChessboardCorners(gimage, boardsize, pointBuf,
//				CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK + CALIB_CB_FILTER_QUADS);
//
//		if (corner_found) {
//
//			cout << "Corners found for " << i << endl;
//			// need to flip the orientation, possibly ...
//			first_point = pointBuf[0];
//			last_point = pointBuf[chess_w*chess_h - 1];
//
//			if (first_point.y < last_point.y){
//				// flipping orientation, this is application dependant.
//
//				for (int k=0; k<corner_count; k++) {
//
//					flipped_points[k] = pointBuf[chess_w*chess_h - 1 - k];
//				}
//
//				pointBuf.swap(flipped_points);
//
//			}
//
//			some_found = true;
//			// refine the corner positions
//			cv::cornerSubPix( gimage, pointBuf, cv::Size(11,11),
//					cv::Size(-1,-1), cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
//
//
//			if (draw_corners){
//				// draw detected corner points in the current frame
//				cv::drawChessboardCorners(images[i], boardsize, cv::Mat(pointBuf), true);
//
//
//				cv::circle( images[i],
//						pointBuf[0], round(images[i].rows/40), cv::Scalar(255, 0, 0), 3, -1);
//			}
//
//			im_points.push_back(pointBuf);
//
//		}	else {
//			cout << "Did not find corners for for " << i << endl;
//			im_points.push_back(vector<cv::Point2f>());
//		}
//	}
//
//	if (images.size() > 0){
//		image_size = images[0].size();
//	}
//
//	return some_found;
//
//}

bool CaliObjectOpenCV2::AccumulateCorners(bool draw_corners){

	//cv::Mat im, gimage, result;
	string current_name;
	//bool corner_found;
	bool some_found = false;
	string filename;

	cv::Size boardsize;
	boardsize.height = chess_h;
	boardsize.width = chess_w;

	int number_images = images.size();
	int number_threads = omp_get_max_threads();

	im_points.resize(number_images, vector<cv::Point2f>() );

#pragma omp parallel for
	for (int thread_number = 0; thread_number < number_threads; thread_number++){
		// allocate everything per thread.
		vector<cv::Point2f> pointBuf_private;
		cv::Mat gimage_private;
		bool corner_found_private;

		for (int i = 0; i <  number_images; i++){
			if (i % number_threads == thread_number){
				// this indicates this threads takes this image.  Index in the images is i.
				// index in the per_thread is i/number_threads

				cv::cvtColor(images[i], gimage_private, CV_BGR2GRAY);


				corner_found_private = cv::findChessboardCorners(gimage_private, boardsize, pointBuf_private,
						CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK + CALIB_CB_FILTER_QUADS);

				if (corner_found_private) {

#pragma omp critical
					{
						// remove this if too slow.
						cout << "Corners found for " << i << " by thread number " << thread_number << endl;
					}

					// refine the corner positions
					cv::cornerSubPix( gimage_private, pointBuf_private, cv::Size(11,11),
							cv::Size(-1,-1), cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));


					if (draw_corners){
						// draw detected corner points in the current frame
						cv::drawChessboardCorners(images[i], boardsize, cv::Mat(pointBuf_private), true);


						cv::circle( images[i],
								pointBuf_private[0], round(images[i].rows/40), cv::Scalar(255, 0, 0), 3, -1);
					}

#pragma omp critical
					{
						// there's no write-write conflict, so this potentially does not have to be within critical.
						// anyway, this does not take long.  copy.
						im_points[i] = pointBuf_private;
					}
				}
			}
		}

	}

	for (int i = 0; i <  number_images; i++){
		if (im_points[i].size() > 0){
			some_found = true;
		}
	}

	if (images.size() > 0){
		image_size = images[0].size();
	}

	return some_found;

}


//struct CameraCaliData{
//	vector< vector<cv::Point2f> >* image_Points;
//	vector< vector<cv::Point3f> >* world_Points;
//	vector<vector<double> > ps;
//	vector<double>  vals;
//};
//

//void CaliObjectOpenCV2::Calibrate(std::ofstream& out, string write_directory){
//
//	// need to make the points 3D
//
//	vector< cv::Point3f> corners(chess_h*chess_w);
//	string filename;
//
//
//	cout << "Calibrating using " << im_points.size() << " patterns " << endl;
//
//	int counter = 0;
//	for( int i = 0; i < chess_h; ++i ){
//		for( int j = 0; j < chess_w; ++j, counter++ ){
//			corners[counter] = (cv::Point3f(float( j*mm_width ), float( i*mm_height ), 0));
//		}
//	}
//
//	// b/c all of the positions are the same ....
//	for (int i = 0; i < int(im_points.size()); i++){
//		all_3d_corners.push_back(corners);
//	}
//
//	cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
//
//	cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64F);
//	//cv::Mat distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
//
//	vector<cv::Mat> rvecs, tvecs;
//
//	if (image_size.width > 640){
//		cameraMatrix.at<double>(0, 0) = 2500;
//		cameraMatrix.at<double>(1, 1) = 2500;
//	}	else {
//		cameraMatrix.at<double>(0, 0) = 1800;
//		cameraMatrix.at<double>(1, 1) = 1800;
//		cameraMatrix.at<double>(0, 0) = 1000;
//		cameraMatrix.at<double>(1, 1) = 1000;
//	}
//	cameraMatrix.at<double>(0, 2) = image_size.width/2;
//	cameraMatrix.at<double>(1, 2) = image_size.height/2;
//
//	cout << "initial camera matrix " << endl;
//
//	for (int i = 0; i < 3; i++){
//		for (int j = 0; j < 3; j++){
//			cout << cameraMatrix.at<double>(i, j) << " ";
//		}
//		cout << endl;
//	}
//
//	cout << "Running calibration " << endl;
//	cout << "Number of dist coefficients  = " << distCoeffs.rows << endl;
//
//	// this one does not use the initial guess
//	double rms = 0;
//
//	if (text_file.size() == 0){
//
//		// OpenCV versions
//		rms = cv::calibrateCamera(all_3d_corners, im_points, image_size, cameraMatrix, distCoeffs, rvecs, tvecs,
//				CALIB_RATIONAL_MODEL);
//
//
//	}	else {
//		ifstream in(text_file.c_str());
//		string temp;
//		in >> cameraMatrix.at<double>(0, 0);
//		in >> temp;
//		in >> cameraMatrix.at<double>(0, 2);
//		in >> temp;
//		in >> cameraMatrix.at<double>(1, 1);
//		in >> cameraMatrix.at<double>(1, 2);
//		in >> temp >> temp >> temp;
//		in.close();
//	}
//
//
//	//double rms = cv::calibrateCamera(all_3d_corners, im_points, image_size, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_USE_INTRINSIC_GUESS);
//
//	cout << "rms " << rms << endl;
//	cout << "camera matrix " << endl;
//
//
//	for (int i = 0; i < 3; i++){
//		for (int j = 0; j < 3; j++){
//			cout << cameraMatrix.at<double>(i, j) << " ";
//		}
//		cout << endl;
//	}
//
//	cout << "Distortion " << endl;
//	for (int i = 0; i < distCoeffs.rows; i++){
//		cout << distCoeffs.at<double>(i, 0) << " ";
//	}
//	cout << endl;
//
//
//	out << "Internal images used: " << number_internal_images_written << endl;
//	out << "External images used: " << external_images.size() << endl;
//	out << "rms " << rms << endl;
//	out << "camera matrix " << endl;
//	for (int i = 0; i < 3; i++){
//		for (int j = 0; j < 3; j++){
//			out << cameraMatrix.at<double>(i, j) << " ";
//		}
//		out << endl;
//	}
//
//	out << "Distortion " << endl;
//	for (int i = 0; i < distCoeffs.rows; i++){
//		out << distCoeffs.at<double>(i, 0) << " ";
//	}
//	out << endl;
//
//	//	vector<vector<double> > A;
//	//		vector< double > k;
//	//		vector< vector <double> > Rt;
//	//		vector< vector< vector <double> > > Rts;
//
//	cv::Mat rotMatrix = cv::Mat::eye(3, 3, CV_64F);
//	vector< vector <double> > tempRt(3, vector<double>(4, 0));
//
//	A.resize(3, vector<double>(3, 0));
//	k.resize(8, 0);
//	for (int i = 0; i < 3; i++){
//		for (int j = 0; j < 3; j++){
//			A[i][j] = cameraMatrix.at<double>(i, j);
//		}
//	}
//
//	for (int i = 0; i < distCoeffs.rows; i++){
//		k[i] = distCoeffs.at<double>(i, 0);
//	}
//
//
//	double reproj_error = 0;
//	vector<cv::Point2f> imagePoints2;
//	double err;
//
//	for (int m = number_internal_images_written; m < int(im_points.size()); m++){
//
//		cv::projectPoints( cv::Mat(all_3d_corners[m]), rvecs[m], tvecs[m], cameraMatrix,  // project
//				distCoeffs, imagePoints2);
//		err = cv::norm(cv::Mat(im_points[m]), cv::Mat(imagePoints2), CV_L2);              // difference
//
//		reproj_error        += err*err;                                             // su
//	}
//
//	out << endl << "Summed reproj error " << reproj_error << endl << endl;
//
//	// we only want these for the external images ....
//	for (int m = number_internal_images_written; m < int(im_points.size()); m++){
//		cv::Rodrigues(rvecs[m], rotMatrix);
//
//		for (int i = 0; i < 3; i++){
//			for (int j = 0; j < 3; j++){
//				tempRt[i][j] = rotMatrix.at<double>(i, j);
//			}
//
//			tempRt[i][3] = tvecs[m].at<double>(i);
//		}
//
//		Rts.push_back(tempRt);
//
//		out << "Rt for cam " << m << endl;
//		for (int i = 0; i < 3; i++){
//			for (int j = 0; j < 4; j++){
//				out << tempRt[i][j] << " ";
//			}
//			out << endl;
//		}
//
//		out << endl;
//	}
//
//	cout << "Finish with cali ... " << endl;
//
//	cv::Mat view, rview, map1, map2;
//	cv::Mat gray;
//	cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
//			cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, image_size, 1, image_size, 0),
//			image_size, CV_16SC2, map1, map2);
//
//	for (int i = 0; i < int(external_images.size()); i++){
//		cout << "Writing external " << i << endl;
//		cv::remap(external_images[i], rview, map1, map2, cv::INTER_LINEAR);
//
//		filename  = write_directory + "/ext" + ToString<int>(i) + ".png";
//		cv::imwrite(filename.c_str(), rview);
//	}
//}

void CaliObjectOpenCV2::Calibrate(std::ofstream& out, string write_directory, float camera_size, int zero_tangent_dist, int zero_k3){

	// need to make the points 3D
	// colors for cameras
	vector<vector<int> > colors;

	vector<int> c(3);
	// lowest value is grey
	c[0] = 0;
	c[1] = 0;
	c[2] = 0;

	colors.push_back(c);

	// next lowest value is purple
	c[0] = 128;
	c[1] = 0;
	c[2] = 128;
	colors.push_back(c);

	// next lowest value is blue
	c[0] = 0;
	c[1] = 0;
	c[2] = 200;
	colors.push_back(c);

	// next lowest value is cyan
	c[0] = 0;
	c[1] = 255;
	c[2] = 255;
	colors.push_back(c);

	// next lowest value is green
	c[0] = 0;
	c[1] = 255;
	c[2] = 0;
	colors.push_back(c);

	// next lowest value is yellow
	c[0] = 255;
	c[1] = 255;
	c[2] = 0;
	colors.push_back(c);

	// next lowest value is red
	c[0] = 255;
	c[1] = 0;
	c[2] = 0;
	colors.push_back(c);

	int number_colors = colors.size();
	int color_index = 0;




	vector< cv::Point3f> corners(chess_h*chess_w);
	string filename;
	Matrix3d internalM;
	Matrix4d externalM;

	int num_images = images.size();


	int counter = 0;
	for( int i = 0; i < chess_h; ++i ){
		for( int j = 0; j < chess_w; ++j, counter++ ){
			corners[counter] = (cv::Point3f(float( j*mm_width ), float( i*mm_height ), 0));
		}
	}

	// b/c all of the positions are the same ....
	// create map ....
	vector<bool> pattern_detected;
	vector<int> mapping_from_limited_to_full;
	vector< vector< cv::Point2f> > all_points_wo_blanks;

	for (int i = 0; i < num_images; i++){
		if (im_points[i].size() > 0){
			mapping_from_limited_to_full.push_back(i);
			all_points_wo_blanks.push_back(im_points[i]);
			three_d_corners.push_back(corners);
			pattern_detected.push_back(true);
		}	else {
			pattern_detected.push_back(false);
		}

	}

	int num_patterns_found = all_points_wo_blanks.size();

	if (all_points_wo_blanks.size() == 0){
		cout << "No corners were found, so the code will fail at cv::calibrateCamera. Quitting." << endl;
		exit(1);
	}

	cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

	cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64F);

	vector<cv::Mat> rvecs, tvecs;

	double max_dim = image_size.width;
	max_dim < image_size.height ? max_dim = image_size.height : 0;

	double focal_length_px = max_dim*1.2;

	cameraMatrix.at<double>(0, 0) = focal_length_px;
	cameraMatrix.at<double>(1, 1) = focal_length_px;

	cameraMatrix.at<double>(0, 2) = image_size.width/2;
	cameraMatrix.at<double>(1, 2) = image_size.height/2;

	cout << "initial camera matrix " << endl;

	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			cout << cameraMatrix.at<double>(i, j) << " ";
		}
		cout << endl;
	}

	cout << "Running calibration " << endl;
	cout << "Number of dist coefficients  = " << distCoeffs.rows << endl;

	double rms = 0;

	int flags = 0;

	if (zero_tangent_dist && !zero_k3){
		flags = cv::CALIB_ZERO_TANGENT_DIST ;
	}

	if (!zero_tangent_dist && zero_k3){
		flags = cv::CALIB_FIX_K3 ;
	}

	if (zero_tangent_dist && zero_k3){
		flags =  cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_K3;
	}


	rms = cv::calibrateCamera(three_d_corners, all_points_wo_blanks, image_size, cameraMatrix, distCoeffs, rvecs, tvecs, flags);

	cout << "rms " << rms << endl;
	cout << "camera matrix " << endl;


	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			cout << cameraMatrix.at<double>(i, j) << " ";
		}
		cout << endl;
	}

	cout << "Distortion " << endl;
	for (int i = 0; i < distCoeffs.rows; i++){
		cout << distCoeffs.at<double>(i, 0) << " ";
	}
	cout << endl;

	out << "Images used: " << num_images << endl;
	out << "Patterns found:       " << num_patterns_found << endl;
	out << "rms " << rms << endl;
	out << "camera matrix " << endl;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			out << cameraMatrix.at<double>(i, j) << " ";
		}
		out << endl;
	}

	out << "Distortion " << endl;
	for (int i = 0; i < distCoeffs.rows; i++){
		out << distCoeffs.at<double>(i, 0) << " ";
	}
	out << endl;

	cv::Mat rotMatrix = cv::Mat::eye(3, 3, CV_64F);
	vector< vector <double> > tempRt(3, vector<double>(4, 0));

	A.resize(3, vector<double>(3, 0));
	k.resize(8, 0);
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			A[i][j] = cameraMatrix.at<double>(i, j);
			internalM(i, j) = cameraMatrix.at<double>(i, j);
		}
	}

	for (int i = 0; i < distCoeffs.rows; i++){
		k[i] = distCoeffs.at<double>(i, 0);
	}
	cout << "Line 13378 " << endl;
	//char ch; cin >> ch;

	double reproj_error = 0;
	vector<cv::Point2f> imagePoints2;
	double err;


	for (int m = 0; m < num_patterns_found; m++){

		cv::projectPoints( cv::Mat(three_d_corners[m]), rvecs[m], tvecs[m], cameraMatrix,  // project
				distCoeffs, imagePoints2);
		err = cv::norm(cv::Mat(all_points_wo_blanks[m]), cv::Mat(imagePoints2), CV_L2);              // difference
		reproj_error        += err*err;

	}

	out << endl << "Summed reproj error " << reproj_error << endl << endl;

	for (int m = 0; m < num_images; m++){
		Rts.push_back(vector<vector <double> >());
	}


	externalM.setZero();
	externalM(3, 3) = 1;
	// we only want these for the external images ....
	for (int m = 0; m < num_patterns_found; m++){

		cv::Rodrigues(rvecs[m], rotMatrix);


		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 3; j++){
				tempRt[i][j] = rotMatrix.at<double>(i, j);
				externalM(i, j) = rotMatrix.at<double>(i, j);
			}

			tempRt[i][3] = tvecs[m].at<double>(i);
			externalM(i, 3) =  tvecs[m].at<double>(i);
		}

		Rts[mapping_from_limited_to_full.at(m)] = tempRt;

		out << "Rt for cam " << mapping_from_limited_to_full[m]<< endl;
		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 4; j++){
				out << tempRt[i][j] << " ";
			}
			out << endl;
		}

		out << endl;

		color_index = m % number_colors;

		filename = write_directory + "camera_pose" + ToString<int>(mapping_from_limited_to_full.at(m)) + ".ply";
		create_camera(internalM, externalM, camera_size, colors[color_index][0], colors[color_index][1], colors[color_index][2],
				image_size.height, image_size.width, filename);
	}

	cout << "Finish with cali ... " << endl;

	cv::Mat view, rview, map1, map2;
	cv::Mat gray;
	cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
			cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, image_size, 1, image_size, 0),
			image_size, CV_16SC2, map1, map2);


	cout << "Writing external .... " << endl;
#pragma omp parallel for private (filename, rview)
	for (int i = 0; i < num_images; i++){
		//cout << "Writing external " << i << endl;
		cv::remap(images[i], rview, map1, map2, cv::INTER_LINEAR);

		filename  = write_directory + "ext" + ToString<int>(i) + ".png";
		cv::imwrite(filename.c_str(), rview);

	}


	filename = write_directory + "world-coord-system.ply";
	WritePatterns(corners, chess_h, chess_w, filename);
}

void camera(Matrix3d& Kinv, float max_u, float max_v, float mag, vector< Vector3d >& vertex_coordinates ) {

	Vector3d a;
	Vector3d b;
	Vector3d c;
	Vector3d d;

	Vector3d x;
	x << 0, 0, 1;

	a = mag*Kinv*x;

	x << max_u, 0, 1;
	b = mag*Kinv*x;

	x << max_u, max_v, 1;
	c = mag*Kinv*x;

	x << 0, max_v, 1;
	d = mag*Kinv*x;

	vertex_coordinates.push_back(a);
	vertex_coordinates.push_back(d);
	vertex_coordinates.push_back(c);
	vertex_coordinates.push_back(b);

}

int create_camera(Matrix3d& internal, Matrix4d& external, float camera_size, int r, int g, int b, int rows, int cols, string ply_file){


	Vector3d C;
	Vector3d t;
	Matrix3d R;
	// t = -RC, so C = -inv(R)*t
	for (int r0 = 0; r0 < 3; r0++){
		for (int c = 0; c < 3; c++){
			R(r0, c) = external(r0, c);
		}

		t(r0) = external(r0, 3);
	}

	C = -R.transpose()*t;

	create_camera(internal, external, C, r, g, b, rows, cols, ply_file, camera_size);

	return 0;
}

int create_camera(Matrix3d& internal, Matrix4d& external, Vector3d& C, int r, int g, int b, int rows, int cols,
		string ply_file, float camera_size)
{
	vector< Vector3d > vertex_coordinates;
	vector< Vector3d > vertex_normals;
	vector< vector<int> > face_indices;
	vector< vector<int> > edge_indices;
	vector<Vector3d> colors;

	//vector< Vector3d >& vertex_coordinates_external;
	//////////////////////////roi box///////////////////////////////////

	Matrix3d R;
	Vector3d t;

	Matrix3d Kinv = internal.inverse();

	for (int r = 0; r < 3; r++){
		for (int c = 0; c < 3; c++){
			R(r, c) = external(r, c);
		}
		t(r) = external(r, 3);
	}

	Matrix3d Rinv = R.transpose();


	camera(Kinv, cols, rows, camera_size, vertex_coordinates);

	Vector3d tempV;

	for (int i = 0; i < 4; i++){
		tempV = vertex_coordinates[i];

		vertex_coordinates[i] = Rinv*(vertex_coordinates[i] - t);

	}

	Vector3d diff_vector;

	vertex_coordinates.push_back(C);


	Vector3d cp;

	int vertex_number = 0;

	// front face.
	vector<int> face;
	face.push_back(vertex_number);
	face.push_back(vertex_number + 3);
	face.push_back(vertex_number + 1);

	face_indices.push_back(face);


	face.clear();

	face.push_back(vertex_number + 2);
	face.push_back(vertex_number + 1);
	face.push_back(vertex_number + 3);

	face_indices.push_back(face);


	face.clear();
	// a side.

	face.push_back(vertex_number);
	face.push_back(vertex_number + 4);
	face.push_back(vertex_number + 3);


	face_indices.push_back(face);

	face.clear();

	face.push_back(vertex_number);
	face.push_back(vertex_number + 1);
	face.push_back(vertex_number + 4);


	face_indices.push_back(face);

	face.clear();

	face.push_back(vertex_number + 1);
	face.push_back(vertex_number + 2);
	face.push_back(vertex_number + 4);


	face_indices.push_back(face);

	face.clear();

	face.push_back(vertex_number + 2);
	face.push_back(vertex_number + 3);
	face.push_back(vertex_number + 4);


	face_indices.push_back(face);

	face.clear();


	vertex_number += 5;


	std::ofstream out;
	out.open(ply_file.c_str());


	out << "ply" << endl;
	out << "format ascii 1.0" << endl;
	out << "element vertex " << vertex_coordinates.size() << endl;
	out << "property float x" << endl;
	out << "property float y" << endl;
	out << "property float z" << endl;
	out << "property uchar red" << endl;
	out << "property uchar green" << endl;
	out << "property uchar blue" << endl;
	out << "property uchar alpha" << endl;
	out << "element face " << face_indices.size() << endl;
	out << "property list uchar int vertex_indices"<< endl;

	out << "end_header" << endl;

	for (int i = 0, nc = vertex_coordinates.size(); i < nc; i++){
		out << vertex_coordinates[i](0) << " " << vertex_coordinates[i](1) << " " << vertex_coordinates[i](2) << " ";

		if (i == 0){
			out << " 255 255 255 255" << endl;
		}	else {
			out << r << " " << g << " " << b << " 255 " << endl;
		}


	}

	for (int i = 0; i < int(face_indices.size()); i++){

		out << face_indices[i].size() << " ";


		for (int j = 0; j < int(face_indices[i].size()); j++){
			out << face_indices[i].at(j) << " ";
		}


		out << endl;

	}


	out.close();

	return(0);

}

void WritePatterns(vector<cv::Point3f>& pattern_points, int chess_h, int chess_w, string outfile){

	// each vertex needs a color ....
	// first, find the range ....

	vector<int> c(3);

	c[0] = 0;
	c[1] = 0;
	c[2] = 0;

	cout << "Writing to " << outfile << endl;
	std::ofstream out;
	out.open(outfile.c_str());

	// first row has chess_h - 2 faces ?
	int number_faces = (chess_h/2)*(chess_w/2) + (chess_h/2 - 1)*(chess_w/2 - 1);


	out << "ply" << endl;
	out << "format ascii 1.0" << endl;
	out << "element vertex " << chess_h*chess_w << endl;
	out << "property float x" << endl;
	out << "property float y" << endl;
	out << "property float z" << endl;
	out << "property uchar red" << endl;
	out << "property uchar green" << endl;
	out << "property uchar blue" << endl;
	out << "property uchar alpha" << endl;
	out << "element face " << number_faces << endl;
	out << "property list uchar int vertex_indices"<< endl;
	out << "end_header" << endl;

	for (int i = 0; i < chess_h*chess_w; i++){
		out << pattern_points[i].x << " " << pattern_points[i].y << " " << pattern_points[i].z << " "
				<< c[0] << " " << c[1] << " " << c[2] << " 175" << endl;
	}


	int p0, p1, p2, p3;
	for (int i = 0; i < chess_h; i++){

		for (int j = 0; j < chess_w/2; j++){
			if (i % 2  == 0){
				p0 = i*chess_w + 2*j;
				p1 = i*chess_w + 2*j + 1;
				p2 = (i + 1)*chess_w + 2*j + 1;
				p3 = (i + 1)*chess_w + 2*j;
				out << "4 " << p0 << " " << p1 << " " << p2 << " " << p3 << endl;

			}	else {
				if (j < chess_w/2 - 1){
					p0 = i*chess_w + 2*j + 1;
					p1 = i*chess_w + 2*j + 2;
					p2 = (i + 1)*chess_w + 2*j + 2;
					p3 = (i + 1)*chess_w + 2*j + 1;
					out << "4 " << p0 << " " << p1 << " " << p2 << " " << p3 << endl;

				}
			}


		}
	}


	out << endl;

	out.close();
}

//PatternsCreatedCharuco::PatternsCreatedCharuco(string read_dir, string write_dir, bool rotate, string src_file, bool generate_only){
//
//	vector<int> squaresX;
//	vector<int> squaresY;
//	vector<int> pixelSquareLength;
//	vector<int> pixelMarkerLength;
//	vector<double> squareLength;
//	vector<int> number_markers;
//	int number_boards;
//	int number_markers_this_board;
//	rotate_case = rotate;
//
//	ofstream out;
//	string filename_write;
//	string command;
//
//	if (rotate_case == false){
//		max_internal_patterns = 0;
//		internalx = 0;  internaly = 0;
//
//		string returnString;
//		int dictionary_version;
//
//		/// read spec file,
//		string filename = read_dir + "network_specification_file.txt";
//		ifstream in;
//
//		returnString = FindValueOfFieldInFile(filename, "aruco_dict", false, true);
//		dictionary_version = FromString<int>(returnString);
//
//
//		returnString = FindValueOfFieldInFile(filename, "number_boards", false, true);
//		number_boards = FromString<int>(returnString);
//
//		dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionary_version));
//
//		in.open(filename.c_str());
//
//		// This particular read has to be in order.
//		string temp;
//		for (int i = 0; i < 4; i++){
//			in >> temp;
//		}
//
//		for (int i = 0; i < number_boards; i++){
//			in >> temp >> temp; squaresX.push_back(FromString<int>(temp));
//			in >> temp >> temp; squaresY.push_back(FromString<int>(temp));
//			in >> temp >> temp; pixelSquareLength.push_back(FromString<int>(temp));
//			in >> temp >> temp; pixelMarkerLength.push_back(FromString<int>(temp));
//		}
//		in.close();
//
//		string filename_write = write_dir + "network_specification_file.txt";
//
//		string command = "cp " + filename + " " + filename_write;
//
//		system(command.c_str());
//
//		for (int i = 0, m = 0, s = 0; i < number_boards; i++){
//			/// the dimensions of the board are linked in here ....
//			boards.push_back(cv::aruco::CharucoBoard::create(squaresX[i], squaresY[i], pixelSquareLength[i], pixelMarkerLength[i], dictionary));
//
//			square_h_w.push_back(pair<int, int>(squaresY[i], squaresX[i]));
//
//			min_max_id_pattern.push_back(pair<int, int>(m, m));
//			number_markers_this_board = boards[i]->ids.size();
//			number_markers.push_back(number_markers_this_board);
//
//			for (int j = 0; j < number_markers_this_board; j++, m++){
//				boards[i]->ids[j] = m;
//			}
//
//			min_max_id_pattern[i].second = min_max_id_pattern[i].first + number_markers[i] - 1;
//			min_max_id_squares.push_back(pair<int, int>(s, s));
//			s = s + (squaresX[i] - 1)*(squaresY[i] - 1) - 1;
//			min_max_id_squares[i].second = s;
//			s = s + 1;
//
//
//			cv::Size imageSize;
//			imageSize.width = 20 + 1 * pixelSquareLength[i]*squaresX[i];
//			imageSize.height = 20 + 1 * pixelSquareLength[i] *squaresY[i];
//			Mat boardImage = Mat::zeros(imageSize.height, imageSize.width, CV_8UC1);
//			boardImage.setTo(255);
//
//
//			boards[i]->draw( imageSize, boardImage, 10, 1 );
//			filename = write_dir + "Board" + ToString<int>(i) + ".png";
//			imwrite(filename.c_str(), boardImage);
//		}
//
//		//pendatic, but want to avoid re-computing it and making a mistake later.
//		number_total_squares = min_max_id_squares[number_boards - 1].second  + 1;
//		int_number_markers = min_max_id_pattern[number_boards - 1].second + 1;
//
//		float mm = 0;
//		// then open up all of the physical measurement files, add to square length.
//		for (int i = 0; i < number_boards; i++){
//			if (!generate_only){
//				filename = read_dir + "pattern_square_mm" + ToString<int>(i) + ".txt";
//
//				returnString = FindValueOfFieldInFile(filename, "squareLength_mm", false, true);
//				mm = FromString<float>(returnString);
//
//				squareLength.push_back(mm);
//
//				in.close();
//
//				filename_write = write_dir + "pattern_square_mm" + ToString<int>(i) + ".txt";
//				command = "cp " + filename + " " + filename_write;
//
//				system(command.c_str());
//			}	else {
//
//				filename_write = write_dir + "pattern_square_mm" + ToString<int>(i) + ".txt";
//
//				out.open(filename_write.c_str());
//
//				out << "squareLength_mm  XX" << endl;
//
//				out.close();
//			}
//		}
//
//		// convert everything to the class members.
//		//////////////////////// double to single.//////////////////////////////
//		if (!generate_only){
//			three_d_points = vector< cv::Point3f >(number_total_squares, cv::Point3f());
//			int sm = 0;
//			for (int i = 0, sc = 0; i < number_boards; i++){
//				mm = squareLength[i];
//
//				vector<int> current_index(number_markers[i], 0);
//
//				for (int m = 0; m < number_markers[i]; m++, sc++){
//					current_index[m] = sc;
//				}
//
//				for (int r = 0; r < squaresY[i] - 1; r++){
//					for (int c = 0; c < squaresX[i] - 1; c++, sm++){
//						Point3f p(mm*float(c), float(r)*mm, 0);
//						three_d_points[sm] = p;
//					}
//				}
//
//				double_to_single.push_back(current_index);
//			}
//		}
//
//
//		number_patterns = number_boards;
//	}	else {
//		////////////////////////////////////////////////////////
//		//////// Rotate case, possible internal parameters ///////
//		/////////////////////////////////////////////////////////
//
//		vector<Mat> images;
//		string temp;
//		int sLin, mLin, margin, id_start, squaresXext, squaresYext, num_markExt,
//		sLext, mLext, dictionary_version;
//		ifstream in;
//		string returnString;
//		double mm;
//		vector< int > ids;
//		vector< vector< Point2f > > corners, rejected;
//		Mat imageCopy;
//		double mm_internal = 0;
//		double mm_external = 0;
//
//		/// read specification file,
//		string filename = read_dir + "rotate_specification_file.txt";;
//
//		//		internal_squaresX 14
//		//		internal_squaresY 14
//		//		internal_squareLength 200
//		//		internal_markerLength 90
//		//		internal_margin 50
//		//		number_boards 8
//		//		squaresX 3
//		//		squaresY 3
//		//		squareLength 60
//		//		markerLength 46
//
//
//		returnString = FindValueOfFieldInFile(filename, "aruco_dict", false, true);
//		dictionary_version = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "internal_squaresX", false, true);
//		internalx = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "internal_squaresY", false, true);
//		internaly = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "internal_squareLength", false, true);
//		sLin = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "internal_markerLength", false, true);
//		mLin = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "internal_margin", false, true);
//		margin = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "number_boards", false, true);
//		number_boards = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "squaresX", false, true);
//		squaresXext = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "squaresY", false, true);
//		squaresYext = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "squareLength", false, true);
//		sLext = FromString<int>(returnString);
//
//		returnString = FindValueOfFieldInFile(filename, "markerLength", false, true);
//		mLext = FromString<int>(returnString);
//
//		id_start = internalx*internaly;
//
//		/// now, read in mm for external
//		// now, read in mm for internal
//
//		cout << "After read " << number_boards << endl;
//
//		filename_write = write_dir + "rotate_specification_file.txt";
//
//		string command = "cp " + filename + " " + filename_write;
//
//		system(command.c_str());
//
//		dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionary_version));
//
//		for (int i = 0, m = id_start, s = 0; i < number_boards; i++){
//			/// the dimensions of the board are linked in here ....
//			boards.push_back(cv::aruco::CharucoBoard::create(squaresXext, squaresYext, sLext, mLext, dictionary));
//			square_h_w.push_back(pair<int, int>(squaresYext, squaresXext));
//
//
//			min_max_id_pattern.push_back(pair<int, int>(m, m));
//
//			num_markExt = boards[i]->ids.size();
//
//			// relabel so that the aruco and charuco labels are different.
//			for (int j = 0; j < num_markExt; j++, m++){
//				boards[i]->ids[j] = m;
//			}
//
//			//cout << "Line 791" << endl; cin >> df;
//			min_max_id_pattern[i].second = min_max_id_pattern[i].first + num_markExt - 1;
//			min_max_id_squares.push_back(pair<int, int>(s, s));
//			s = s + (squaresXext - 1)*(squaresYext - 1) - 1;
//			min_max_id_squares[i].second = s;
//			s = s + 1;
//
//			cv::Size imageSize;
//			imageSize.width = 20 + 1 * sLext * squaresXext;
//			imageSize.height = 20 + 1 * sLext * squaresYext;
//			Mat boardImage = Mat::zeros(imageSize.height, imageSize.width, CV_8UC1);
//			boardImage.setTo(255);
//
//			boards[i]->draw( imageSize, boardImage, 10, 1 );
//			filename = write_dir + "Marker" + ToString<int>(i) + ".png";
//			imwrite(filename.c_str(), boardImage);
//		}
//
//		// this is all inane, but want to avoid re-computing it and making a mistake later.
//		number_total_squares = min_max_id_squares[number_boards - 1].second  + 1;
//		int_number_markers = min_max_id_pattern[number_boards - 1].second + 1;
//
//		filename = read_dir + "pattern_square_mm_external.txt";
//
//		if (!generate_only){
//			returnString = FindValueOfFieldInFile(filename, "squareLength_mm", false, true);
//			mm_external = FromString<float>(returnString);
//
//			for (int i = 0; i < number_boards; i++){
//				squareLength.push_back(mm_external);
//			}
//
//
//			//////////////////////// double to single.//////////////////////////////
//			three_d_points = vector< cv::Point3f >(number_total_squares, cv::Point3f());
//			int sm = 0;
//			for (int i = 0, sc = 0; i < number_boards; i++){
//				mm = squareLength[i];
//
//				vector<int> current_index(num_markExt, 0);
//
//				for (int m = 0; m < num_markExt; m++, sc++){
//					current_index[m] = sc;
//				}
//
//				for (int r = 0; r < squaresYext - 1; r++){
//					for (int c = 0; c < squaresXext - 1; c++, sm++){
//						Point3f p(mm*float(c), float(r)*mm, 0);
//						three_d_points[sm] = p;
//					}
//				}
//
//				double_to_single.push_back(current_index);
//			}
//
//			filename = read_dir + "pattern_square_mm_external.txt";
//			filename_write = write_dir + "pattern_square_mm_external.txt";
//			command = "cp " + filename + " " + filename_write;
//
//			system(command.c_str());
//
//
//		}	else {
//			filename_write = write_dir +  "pattern_square_mm_external.txt";
//
//			out.open(filename_write.c_str());
//
//			out << "squareLength_mm  XX" << endl;
//
//			out.close();
//		}
//
//
//
//
//		// pendantic, to avoid re-computing it and making a mistake later.
//		number_total_squares = min_max_id_squares[number_boards - 1].second  + 1;
//		int_number_markers = min_max_id_pattern[number_boards - 1].second + 1;
//
//		cout << "Number boards " << number_boards << endl;
//		cout << "Number total squares " << number_total_squares << endl;
//
//		cout << "Before 3d points " << endl;
//
//		/// internal markers.
//		filename = read_dir + "pattern_square_mm_internal.txt";
//
//		if (!generate_only){
//
//			returnString = FindValueOfFieldInFile(filename, "squareLength_mm", false, true);
//			mm_internal = FromString<float>(returnString);
//
//
//			filename = read_dir + "pattern_square_mm_internal.txt";
//			filename_write = write_dir + "pattern_square_mm_internal.txt";
//			command = "cp " + filename + " " + filename_write;
//
//			system(command.c_str());
//
//
//		}	else {
//			filename_write = write_dir +  "pattern_square_mm_internal.txt";
//
//			out.open(filename_write.c_str());
//
//			out << "squareLength_mm  XX" << endl;
//
//			out.close();
//		}
//
//		cout << "Before create images " << endl;
//		// detect pattern locations on the image..
//
//		CreateRotateCaseImagesCharuco(images, internalx, internaly, sLin, mLin, margin, id_start, dictionary_version, src_file);
//		filename = write_dir + "created_backstop.png";
//		imwrite(filename.c_str(), images[0]);
//
//		// for each tag, generate where it should be in 3D space.  so indices are tag0 corner0-3 tag1 corner0-3 etc., .etc.  not on a grid necessarily.
//		// try the default parameters,
//		Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create();
//
//		aruco::detectMarkers(images[0], dictionary, corners, ids, detectorParams, rejected);
//
//		cvtColor(images[0], imageCopy, cv::COLOR_GRAY2RGB);
//
//		if(ids.size() > 0) {
//			aruco::drawDetectedMarkers(imageCopy, corners, ids, Scalar(255, 255, 0));
//			filename = write_dir + "internal_with_labels.png";
//			cv::imwrite(filename, imageCopy);
//		}
//
//		///////////// FILL IN THE INTERAL PARAMETERS AND WRITE ///////////////////
//
//		// internal_mm is the measurement per square edge.
//		// how far apart are the squares?
//
//		if (!generate_only){
//			max_internal_patterns = internalx*internaly;
//			if (ids.size() > 0 && int(ids.size()) == max_internal_patterns){
//				vector<int> mapping_to_id(max_internal_patterns, 0);
//				// are the ids in order?
//				for (int i = 0, in = ids.size(); i < in; i++){
//					mapping_to_id[ids[i]] = i;
//				}
//
//				double distance_between_squares = mm_internal*double(sLin)/double(mLin);
//
//				cout << "Current settings, square length, distance bewteen squares froms start to finish " << mm_internal << ", " << distance_between_squares << endl;
//
//				int current_index;
//				int x_value, y_value;
//				vector< Point2f > current_corners;
//
//				int internal_squares = 4*max_internal_patterns;
//				three_d_points_internal = vector< cv::Point3f >(internal_squares, cv::Point3f());
//
//				for (int i = 0, in = ids.size(); i < in; i++){
//					cout << "Pattern creation, in this order: " << ids[i] << endl;
//					current_index = ids[i];
//					current_corners = corners[i];
//
//					x_value = current_index/internaly;
//					y_value = internaly - current_index%internaly - 1;
//
//					// assign the value -- clockwise
//					Point3f p0(distance_between_squares*double(x_value), distance_between_squares*double(y_value) + mm_internal, 0);
//					Point3f p1(distance_between_squares*double(x_value) + mm_internal, distance_between_squares*double(y_value) + mm_internal, 0);
//					Point3f p2(distance_between_squares*double(x_value) + mm_internal, distance_between_squares*double(y_value), 0);
//					Point3f p3(distance_between_squares*double(x_value), distance_between_squares*double(y_value), 0);
//
//					three_d_points_internal[4*current_index] = p0; // would correspond to current corners[0]
//					three_d_points_internal[4*current_index + 1] = p1;
//					three_d_points_internal[4*current_index + 2] = p2;
//					three_d_points_internal[4*current_index + 3] = p3;
//
//
//					string coords = ToString<float>(x_value) + ", " + ToString<float>(y_value);
//					putText(imageCopy, coords, Point(current_corners[0].x,current_corners[0].y), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 0, 0), 1);
//					for (int j = 1; j < 4; j++){
//						string coords = ToString<float>(three_d_points_internal[4*current_index + j].x) + ", " + ToString<float>(three_d_points_internal[4*current_index + j].y);
//						putText(imageCopy, coords, Point(current_corners[j].x,current_corners[j].y), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 0, 0), 1);
//					}
//				}
//
//				filename = write_dir + "internal_with_morelabels.png";
//				cv::imwrite(filename, imageCopy);
//			}	else {
//				cout << "not finding all of the elements .... squareX, Y is wrong?  Double check specification file" << endl;
//				exit(1);
//			}
//		}
//
//		number_patterns = number_boards;
//	}
//
//	// GET 8 COLORS
//	vector<int> color0(3, 20);
//	vector<int> color1(3, 20);
//	vector<int> color2(3, 20);
//	vector<int> color3(3, 20);
//	vector<int> color4(3, 20);
//	vector<int> color5(3, 20);
//	vector<int> color6(3, 20);
//	vector<int> color7(3, 20);
//
//	// r
//	color0[2] = 255;
//
//	// g
//	color1[1] = 255;
//
//	// r
//	color2[0] = 255;
//
//	// mix 0
//	color3[0] = 255;
//	color3[1] = 255;
//
//	// mix1
//	color4[1] = 255;
//	color4[2] = 255;
//
//	// mix2
//	color5[0] = 255;
//	color5[2] = 255;
//
//	// dark g
//	color6[0] = 0;
//	color6[1] = 180;
//	color6[2] = 0;
//
//
//	//dark r
//	color7[0] = 0;
//	color7[1] = 0;
//	color7[2] = 180;
//
//
//	display_colors.push_back(color0);
//	display_colors.push_back(color1);
//	display_colors.push_back(color2);
//	display_colors.push_back(color3);
//	display_colors.push_back(color4);
//	display_colors.push_back(color5);
//	display_colors.push_back(color6);
//	display_colors.push_back(color7);
//}
//
//
//int PatternsCreatedCharuco::NumberMarkers(){
//	return int_number_markers;
//}
//
//int PatternsCreatedCharuco::NumberPatterns(){
//	return number_patterns;
//}
//
//int PatternsCreatedCharuco::NumberSquares(){
//	return number_total_squares;
//}
//
//void PatternsCreatedCharuco::DetermineBoardsPresentFromMarkerList(vector<int>& markers, vector<bool>& boards_seen){
//
//	int np = NumberPatterns();
//
//	for (int i = 0, in = markers.size(); i < in; i++){
//		for (int j = 0;  j < np; j++){ // don't walk this one if it is already occupied.
//			if (markers[i] >= min_max_id_pattern[j].first && markers[i] <= min_max_id_pattern[j].second){
//				boards_seen[j] = true;
//			}
//		}
//	}
//}
//
//int PatternsCreatedCharuco::MappingArucoIDToPatternNumber(int id){
//	int np = NumberPatterns();
//
//	for (int j = 0;  j < np; j++){ // don't walk this one if it is already occupied.
//		if (id >= min_max_id_pattern[j].first && id <= min_max_id_pattern[j].second){
//			return j;
//		}
//	}
//
//	return -1;
//}
//
//
//Scalar PatternsCreatedCharuco::Color(int index){
//
//	if (index >= int(display_colors.size()) ){
//		index = display_colors.size() % index;
//	}
//	return Scalar(display_colors[index][0], display_colors[index][1],display_colors[index][2]);
//}
