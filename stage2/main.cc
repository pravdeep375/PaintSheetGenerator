/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!! Programmed by David Jatczak !!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
#include <iostream>
#include <exception>
#include <vector>
#include <sstream>
#include <cmath>
#include <limits>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

using uint = unsigned int;
using uchar = unsigned char;
const string TITLE = "TEST";


void getImage( Mat& in, char* s ) {
    in = imread( s, 1 );
    if ( !in.data ) {
		throw runtime_error("No input file");
    }
}


void display(const Mat& image) {
    //namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow(TITLE, image);
	// If Escape is hit, close
	while (true) {
		int k = waitKey(10);
		if (k==27) break;
	}
}



// Returns regions that were divided by edges
void getRegions(Mat& in) {
	vector<Mat> ret;
	// Go through all white pixels and
	// for (int col=0; col < in.cols; col++) {
	// 	for (int row=0; row < in.rows; row++) {
	// 		//if (in.at<uchar>(row, col) == 0)
	// 	}
	// }

	vector<Mat> contours;
	vector<Vec4i> hier;
	threshold(in, in, 125, 255, THRESH_BINARY_INV);
	findContours(in, contours, hier, RETR_TREE , CHAIN_APPROX_NONE);
	for (uint iter = 0; iter < contours.size(); iter++) {
		Mat dst = Mat::zeros(in.rows, in.cols, CV_8UC3);
		drawContours(dst, contours, iter, Scalar(0,0,255), FILLED);
		display(dst);
	}
}


int main(int argc, char** argv ) {
    if ( argc != 2 ) {
		cout << "usage: <prog> <image>" << endl;
        return -1;
    }
	Mat img, greyImg;
	getImage( img, argv[1] );
	cvtColor(img, greyImg, COLOR_BGR2GRAY, 1);
	getRegions(greyImg);
    return 0;
}