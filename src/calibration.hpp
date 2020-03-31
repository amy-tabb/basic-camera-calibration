/*
 * Calibration2.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: atabb
 */

#ifndef CALIBRATION_HPP_
#define CALIBRATION_HPP_

#include "Includes.hpp"
#include "DirectoryFunctions.hpp"

#include <math.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

enum cali_type{chess, charuco, aruco};


class CaliObjectOpenCV2{
public:
	cali_type c_type = chess;

	int chess_w = -1;
	int chess_h = -1;
	double mm_width = -1;
	double mm_height = -1;
	double mean_reproj_error = 0;
	double mean_ext_reproj_error = 0; // todo necessary??

	vector< vector< cv::Point2f> > im_points;
	vector< vector< cv::Point3f> > three_d_corners;

	vector<vector<double> > A;
	vector< double > k;
	vector< vector <double> > Rt;
	vector< vector< vector <double> > > Rts;

	vector<cv::Mat> images;

	cv::Size image_size;


	CaliObjectOpenCV2(cali_type c_type, int chess_w, int chess_h, double mm_width, double mm_height):
		c_type(c_type), chess_w(chess_w), chess_h(chess_h), mm_width(mm_width), mm_height(mm_height){}


	void ReadImages(string directory);

	bool AccumulateCorners(bool draw_corners);

	void Calibrate(std::ofstream& out, string write_directory, float camera_size = 40, int zero_tangent_dist = 0, int zero_k3 = 0);


};

int create_camera(Matrix3d& internal, Matrix4d& external, Vector3d& C, int r, int g, int b, int rows, int cols,
		string ply_file, float camera_size);

int create_camera(Matrix3d& internal, Matrix4d& external, float camera_size, int r, int g, int b, int rows, int cols, string ply_file);

void camera(Matrix3d& Kinv, float max_u, float max_v, float mag, vector< Vector3d >& vertex_coordinates );

void WritePatterns(vector<cv::Point3f>& pattern_points, int chess_h, int chess_w, string outfile);


#endif
