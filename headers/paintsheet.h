//  Copyright (C) 2020 David Jatczak <david.j.361@gmail.com>
//  Copyright (C) 2020 Cameron Canning-Potter <cameroncanningpotter@cmail.carleton.ca>
//  Copyright (C) 2020 Pravdeep Deol
//  
//  This file is part of Paint Sheet Generator.
//  
//  Paint Sheet Generator is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  Paint Sheet Generator is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with Paint Sheet Generator.  If not, see <https://www.gnu.org/licenses/>.


#ifndef PAINTSHEET_H
#define PAINTSHEET_H

#include <iostream>
#include <exception>
#include <vector>
#include <sstream>
#include <cmath>
#include <limits>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "legend.h"
#include "borders.h"


using namespace std;
using namespace cv;


class PaintSheet {
public:
	friend class Window;
	
	PaintSheet(Mat& input, int k, string& title, bool dial8 = true, bool drawBoxes=false);

	void showOrig();
	void showQuantized();
	void showProduct();
private:
	Segmenter segmenter;
	vector<Segment> segments;
	string title;
	Mat orig, quantized, quantizedNoEdges, product, kmeans;
	int k;
	bool drawBoxes, dial8;

	void quantize(Mat& input);
	void dilation(int size, int numTimes);
};

#endif
