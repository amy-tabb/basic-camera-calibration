

#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#include <iostream>


#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <list>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <inttypes.h>
#include <parallel/algorithm>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <cmath>
#include <cstdint>
#include <omp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

using std::vector;
using std::ifstream;
using std::string;

using namespace Eigen;
using namespace cv;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::string;


using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;



template<class T>
T FromString(const std::string& s)
{
	std::istringstream stream (s);
	T t;
	stream >> t;
	return t;
}

template<class T>
string ToString(T arg)
{
	std::ostringstream s;

	s << arg;

	return s.str();

}


#endif /* INCLUDES_HPP_ */
