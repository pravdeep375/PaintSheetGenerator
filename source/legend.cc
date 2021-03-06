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


#include "../headers/legend.h"
#include "../headers/helper.h"

using namespace std;
using namespace cv;

using uint = unsigned int;
using uchar = unsigned char;

Legend::Legend(Mat& edges, Mat& quantized, const vector<Segment>& segments) : segments(segments), img(edges.clone()), quantized(quantized.clone()) {
	for (size_t i = 0; i < segments.size(); i++) {
		Segment s = segments[i];
		if (findColour(s.getColour()) == -1) {
			colours.push_back(s);
		}
	}
}

void Legend::createLegend(Mat& dst, bool drawBoxes) {
	// Put numbers on the actual image
	for (size_t i = 0; i < segments.size(); i++) {
		int number = findColour(segments[i].getColour());
		if (number == -1)
			throw runtime_error("createLegend broke");

		//number positioning and sizing
		int baseline = 0;
		Size textSize;
		double fontScale;
		Point p;
		for (fontScale = 1.0; fontScale > 0.15; fontScale = fontScale / 2.0) {
			textSize = getTextSize(to_string(number + 1), Fontface, fontScale, 2, &baseline);
			// Get proper spot for placing the number
			p = findTextSpot(segments[i], textSize);
			// Fits!
			if (p.x != -1 && p.y != -1)
				break;
		}
		// Segment too small for text?
		if (p.x == -1 || p.y == -1)
			continue;

		// draw the box
		if (drawBoxes) {
			Rect r = Rect(p.x, p.y, textSize.width, -textSize.height);
			rectangle(img, r.tl(), r.br(), Scalar(0, 0, 255));
		}

		putText(img, to_string(number + 1), p, Fontface, fontScale, Scalar::all(255), 2);
		putText(img, to_string(number + 1), p, Fontface, fontScale, Scalar(0, 0, 255), 1);
		// also need to update quantized so no overlapping numbers
		putText(quantized, to_string(number + 1), p, Fontface, fontScale, Scalar::all(255), 2);
		putText(quantized, to_string(number + 1), p, Fontface, fontScale, Scalar(0, 0, 255), 1);
	}

	//add black border around img
	copyMakeBorder(img, dst, 1, 1, 1, 1, BORDER_CONSTANT, Scalar::all(0));

	//add inital padding to bottom and right side of img
	copyMakeBorder(dst, dst, 10, 10, 10, 42, BORDER_CONSTANT, Scalar::all(255));


	//point pt1 is rectangle top left corner
	//point pt2 is rectangle bottom right corner
	//with 20 pixel spacing between subsuqent rectangles vertically
	Point pt1 = Point(img.cols + 20, 10);
	Point pt2 = Point(pt1.x + 10, pt1.y + 10);
	Point text;

	// Create the legend
	for (size_t i = 0; i < colours.size(); i++) {

		//added legend entries to right once they reach the height of the orginal img
		//and also extend the img by the needed amount
		if (pt1.y > img.rows) {
			copyMakeBorder(dst, dst, 0, 0, 0, 30, BORDER_CONSTANT, Scalar::all(255));
			pt1 = Point(text.x + 20, 10);
			pt2 = Point(pt1.x + 10, pt1.y + 10);
		}

		rectangle(dst, pt1, pt2, Scalar(colours[i].getColour()), -1);
		rectangle(dst, pt1, pt2, Scalar::all(0), 1);
		text.x = pt1.x + 10;
		text.y = pt2.y;
		putText(dst, to_string(i + 1), text, Fontface, 0.5, Scalar::all(0));

		pt1.y += 20;
		pt2.y += 20;
	}
}

int Legend::findColour(const Scalar& s) const {
	for (size_t i = 0; i < colours.size(); i++) {
		Scalar s2 = colours[i].getColour();
		if (isSameScalar(s, s2)) {
			return i;
		}
	}
	return -1;
}

bool Legend::isSameScalar(const Scalar& s, const Scalar& s2) const {
	if (s[0] == s2[0] && s[1] == s2[1] && s[2] == s2[2]) {
		return true;
	}
	return false;
}

bool Legend::validTextArea(const Point& p, const Size& textSize, const Scalar& colour) const {
	// Check area of textSize if all the same pixel colour

	int yLimit = p.y + textSize.height;
	int xLimit = p.x + textSize.width;
	for (int row = p.y; row < yLimit; row++) {
		for (int col = p.x; col < xLimit; col++) {
			// Boundary check
			if (col < 0 || row < 0 || col >= this->quantized.cols || row >= this->quantized.rows) {
				return false;
			}
			Scalar c(this->quantized.at<Vec3b>(row, col));
			if (!isSameScalar(c, colour)) {
				return false;
			}
		}
	}
	return true;
}

Point Legend::findTextSpot(const Segment s, const Size& textSize) const {
	// Set as an invalid point
	// That way if we any point in the segment is too small for text then don't draw it
	Point ret(-1, -1);
	Scalar colour = Vec3b(s.getColour());

	// Go through all pixels of the segment
	for (auto p : s.getPoints()) {
		if (validTextArea(p, textSize, colour)) {
			ret.x = p.x;
			ret.y = p.y + textSize.height;
			break;
		}
	}
	return ret;
}
