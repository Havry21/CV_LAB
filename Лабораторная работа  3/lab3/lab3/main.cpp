#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
using namespace cv;
using namespace std;
int thresh = 200;

Scalar color = Scalar(120, 120, 120);

Mat firstExc;


#ifdef DEBUG
void thresh_callback(int, void*) {
	threshold(firstExc, canny_output, thresh, 210, THRESH_BINARY);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		cout << " Area: " << contourArea(contours[i]) << endl;
	}

	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(120, 120, 120);
		//drawContours(drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
		polylines(drawing, contours, true, color);
	}
	imshow("My laplas diff1", drawing);
}
#endif // DEBUG


VideoCapture cap("C:/Users/dimag/Desktop/img_zadan/1 - Trim.mp4");

int main(void) {
	cap >> firstExc;
	Mat out = firstExc.clone();
	Mat firstExcGrey = firstExc.clone();

	// запись видео 
	bool isColor = (firstExc.type() == CV_8UC3);
	VideoWriter writer;
	int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
	double fps = 25.0;                          
	string filename = "./video.avi";             
	writer.open(filename, codec, fps, firstExc.size(), isColor);
	// check if we succeeded
	if (!writer.isOpened()) {
		cerr << "Could not open the output video file for write\n";
		return -1;
	}

	while (1) {

		if (!cap.read(firstExc)) {
			cerr << "That`s all\n";
			break;
		}

		cvtColor(firstExc, firstExcGrey, COLOR_RGB2GRAY);

#ifdef DEBUG
		createTrackbar("Canny thresh:", "source_window", &thresh, 255, thresh_callback);
		thresh_callback(0, 0);
#endif // DEBUG
		Mat trh = firstExcGrey.clone();

		threshold(firstExcGrey, trh, thresh, 210, THRESH_BINARY);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		vector<double> arrOfContours;

		findContours(trh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		// тут вытаскиваем самый большой контур
		Mat drawing = Mat::zeros(firstExcGrey.size(), CV_8UC3);
		for (unsigned int i = 0; i < contours.size(); i++) {
			arrOfContours.push_back(contourArea(contours[i]));
		}
		auto max = max_element(arrOfContours.begin(), arrOfContours.end());
		auto it = distance(arrOfContours.begin(), max);

		//ищем и рисуем центр
		Moments mnts = moments(contours[it]);
		Point center(int(mnts.m10 / mnts.m00), int(mnts.m01 / mnts.m00));
		Scalar red = Scalar(0, 0, 255);
		circle(firstExc, center, 2, red, 2);
		writer.write(firstExc);
		imshow("My laplas diff1", firstExc);
		if (waitKey(5) >= 0)
			break;
	}
	waitKey(0);
	destroyAllWindows();
	return 0;
}

