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


