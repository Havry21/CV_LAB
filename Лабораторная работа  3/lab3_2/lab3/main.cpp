#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int thresh = 200;
int thresh2 = 200;
Scalar color = Scalar(120, 120, 120);



Mat secondExc = imread("C:/Users/dimag/Desktop/img_zadan/teplovizor/ntcs_quest_measurement.png"); 
Mat out = secondExc.clone();
Mat contour = secondExc.clone();

int main(void) {

	cvtColor(secondExc, secondExc, COLOR_BGR2HSV);
	inRange(secondExc, Scalar(0, 0, 0), Scalar(25, 255, 255), contour);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<double> arrOfContours;

	findContours(contour, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	for (unsigned int i = 0; i < contours.size(); i++){
		arrOfContours.push_back(contourArea(contours[i]));
	}
	auto max = max_element(arrOfContours.begin(), arrOfContours.end());
	auto it = distance(arrOfContours.begin(), max);



	Moments mnts = moments(contours[it]);
	Point center(int(mnts.m10 / mnts.m00), int(mnts.m01 / mnts.m00));
	Scalar color = Scalar(0, 0, 0);

	circle(out, center, 2, color, 2);

	imshow("My laplas diff1", out);


	waitKey(0);
	destroyAllWindows();
	return 0;
}

