#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

//#define DEBUG 

using namespace cv;
using namespace std;

static Mat origin = imread("C:/Users/dimag/Desktop/img_zadan/gk/gk.jpg"); 
static Mat sample = imread("C:/Users/dimag/Desktop/img_zadan/gk/gk_tmplt.jpg");

#ifdef DEBUG
int thresh = 200;

static void on_low_H_thresh_trackbar(int, void*){
	thresh = min(255 - 1, thresh);
	setTrackbarPos("treshhold", "treshholdWind", thresh);
}
#endif // DEBUG


int main(void) {
	Mat greyOrigin,greySample;
	cvtColor(sample, greySample, COLOR_BGR2GRAY);
	cvtColor(origin, greyOrigin, COLOR_BGR2GRAY);
	
	Mat workimg, workSample;
	threshold(greySample, workSample, 230, 255, THRESH_BINARY);
	threshold(greyOrigin, workimg, 238, 255, THRESH_BINARY_INV);

	/*Получаею контуры*/
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;

	vector<Vec4i> hierarchySample;
	vector<vector<Point> > contoursSample;

	findContours(workimg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	findContours(workSample, contoursSample, hierarchySample, RETR_TREE, CHAIN_APPROX_SIMPLE);


	/*Тут я вытаскиваю индекс самого большого конутра который есть на шаблоне*/
	vector<double> arrOfContours;

	for (unsigned int i = 0; i < contoursSample.size(); i++) { 
		arrOfContours.push_back(contourArea(contoursSample[i]));
	}
	auto max = max_element(arrOfContours.begin(), arrOfContours.end());
	auto it = distance(arrOfContours.begin(), max);

	for (size_t i = 0; i < contours.size(); i++) {
			double match = matchShapes(contours[i], contoursSample[it], CONTOURS_MATCH_I1, 0);
			Scalar color = Scalar(0, 0, 255);
			if (match < 5) {// выставляем цвет контура, если есть подхядщее совпадение
				color = Scalar(0, 255, 0);
			}
			if(contourArea(contours[i]) > 100) // откидываем всякий мусор
				drawContours(origin, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
	}

#ifdef DEBUG
	namedWindow("treshholdWind");
	createTrackbar("treshhold", "treshholdWind", &thresh, 255, on_low_H_thresh_trackbar);
	Mat test;
	while (1) {
		threshold(greyOrigin, test, thresh, 255, THRESH_BINARY_INV);
		imshow("work", test);
		waitKey(100);
	}
#endif // DEBUG

	imshow("work", origin);
	waitKey(0);
	destroyAllWindows();
	return 0;
}

