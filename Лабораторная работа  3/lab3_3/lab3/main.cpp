#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

static Mat exc = imread("C:/Users/dimag/Desktop/img_zadan/roboti/roi_robotov.jpg"); 

Mat out = exc.clone();
Mat excHSV = exc.clone();
static const int max_value_H = 360 / 2;
static const int max_value = 255;


const String window_detection_name = "Object Detection";

static Point lampOrigin;

int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

int high = 10;
//int widht = high;

int lowArray = 0, highArray = 1500;

class SearchColorObj{
public:
	Mat inputImg, workImage, origin;

	int color[3];
	int lowHSV[3];
	int highHSV[3];

	int minArray = 0;
	int kernelSize = 0;

	vector<vector<Point> > contours;
	vector<double> arrOfContours;

private:
	vector<Vec4i> hierarchy;
	
public:
	SearchColorObj(const int _color[], const int _lowHSV[], const int _highHSV[], int _minArray, int _kernelSize = 0) {
		for (int i = 0; i < 3; i++) {
			color[i]   = _color[i];
			lowHSV[i]  = _lowHSV[i];
			highHSV[i] = _highHSV[i];
		}
		minArray = _minArray;
		kernelSize = _kernelSize;
	}

	void search() { // делает пророговую фильтрацию, а потом находит все походящие контуры
		workImage = inputImg.clone();
		//inRange(workImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), workImage);

		inRange(workImage, Scalar(lowHSV[0], lowHSV[1], lowHSV[2]), Scalar(highHSV[0], highHSV[1], highHSV[2]), workImage);
		
		if (kernelSize > 1) {
			Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(kernelSize, kernelSize));; //
			dilate(workImage, workImage, kernel);
			erode(workImage, workImage, kernel);
		}
		findContours(workImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	}

	void draw() { // рисует контур отсекая неподходящие по площади: слишком маленькие или слишком большие
		origin = out;
		Point center, newPoint(0,0);
		double mindist = origin.rows; // начальное расстояние - расстояния с угла изображения до лампы
		int minIdex = 0;

		for (size_t i = 0; i < contours.size(); i++) {
			int array = contourArea(contours[i]);
			if (array > minArray /*lowArray */&& array < 1200) {
				double newdist = 0;
				Moments mnts = moments(contours[i]);
				center.x = int(mnts.m10 / mnts.m00);
				center.y = int(mnts.m01 / mnts.m00);
				newdist = calcDist(center);
				if (mindist > newdist) {
					mindist = newdist;
					newPoint = center;
					minIdex = (int)i;
				}
				//drawContours(origin, contours, (int)i, Scalar(color[0], color[1], color[2]), 2, LINE_8, hierarchy, 0);

			}


		}
		circle(origin, newPoint, 2, Scalar(color[0], color[1], color[2]), 2);  // рисуем самый ближайший центр к лампе
		drawContours(origin, contours, minIdex, Scalar(color[0], color[1], color[2]), 2, LINE_8, hierarchy, 0);

	}

	void work() {
		search();
		draw();
	}

private:

	double calcDist(Point point) {
		return sqrt(pow((lampOrigin.x - point.x), 2) + pow((lampOrigin.y - point.y), 2));
	}

};
Scalar test(1, 2, 3);



static const int blueHSV_H[3] = { 157, 255, 255 };
static const int blueHSV_L[3] = { 96, 0, 0 };
static const int blueHSV_C[3] = { 255, 0, 0 };

static const int redHSV_H[3] = { 15, 255, 255 };
static const int redHSV_L[3] = { 0, 56, 61 };
static const int redHSV_C[3] = { 0, 0, 255 };

static const int greenHSV_H[3] = { 83, 160, 243 };
static const int greenHSV_L[3] = { 71, 36, 135 };
static const int greenHSV_C[3] = { 0, 255, 0 };

static const int lampHSV_H[3] = { 180, 10, 255 };
static const int lampHSV_L[3] = { 0, 0, 0 };
static const int lampHSV_C[3] = { 0, 0, 0 };


SearchColorObj redRobots(redHSV_C,     redHSV_L,   redHSV_H, 300,5);
SearchColorObj blueRobots(blueHSV_C,   blueHSV_L,  blueHSV_H, 151, 19);
SearchColorObj greenRobots(greenHSV_C, greenHSV_L, greenHSV_H, 337, 19);

SearchColorObj lamp(lampHSV_C, lampHSV_L, lampHSV_H, 151);



static void on_low_H_thresh_trackbar(int, void*){
	low_H = min(high_H - 1, low_H);
	setTrackbarPos("Low H", window_detection_name, low_H);
}
static void on_high_H_thresh_trackbar(int, void*){
	high_H = max(high_H, low_H + 1);
	setTrackbarPos("High H", window_detection_name, high_H);
}
static void on_low_S_thresh_trackbar(int, void*){
	low_S = min(high_S - 1, low_S);
	setTrackbarPos("Low S", window_detection_name, low_S);
}
static void on_high_S_thresh_trackbar(int, void*){
	high_S = max(high_S, low_S + 1);
	setTrackbarPos("High S", window_detection_name, high_S);
}
static void on_low_V_thresh_trackbar(int, void*){
	low_V = min(high_V - 1, low_V);
	setTrackbarPos("Low V", window_detection_name, low_V);
}
static void on_high_V_thresh_trackbar(int, void*){
	high_V = max(high_V, low_V + 1);
	setTrackbarPos("High V", window_detection_name, high_V);
}
static void on_low_V_array_trackbar(int, void*) {
	lowArray = min(highArray - 1, lowArray);
	setTrackbarPos("Low Array", window_detection_name, lowArray);
}
static void on_high_V_array_trackbar(int, void*){
	highArray = max(highArray, lowArray + 1);
	setTrackbarPos("High Array", window_detection_name, highArray);
}
static void on_high_size(int, void*) {
	high = min(50 - 1, high );
	setTrackbarPos("Kernel size", window_detection_name, high);
}

int main(void) {
	namedWindow(window_detection_name, WINDOW_NORMAL);
	//imshow("origin", exc);
	//createTrackbar("Low H",  window_detection_name, &low_H,  max_value_H, on_low_H_thresh_trackbar);
	//createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	//createTrackbar("Low S",  window_detection_name, &low_S,  max_value,   on_low_S_thresh_trackbar);
	//createTrackbar("High S", window_detection_name, &high_S, max_value,   on_high_S_thresh_trackbar);
	//createTrackbar("Low V",  window_detection_name, &low_V,  max_value,   on_low_V_thresh_trackbar);
	//createTrackbar("High V", window_detection_name, &high_V, max_value,   on_high_V_thresh_trackbar);
	//createTrackbar("Low Array", window_detection_name, &lowArray,  highArray, on_low_V_array_trackbar);
	//createTrackbar("High Array",window_detection_name, &highArray, highArray, on_high_V_array_trackbar);
	//createTrackbar("Kernel size", window_detection_name, &high, 50, on_high_size);

	cvtColor(exc, excHSV, COLOR_BGR2HSV);


	lamp.inputImg = excHSV.clone();
		greenRobots.inputImg = excHSV.clone();

	redRobots.inputImg = excHSV.clone();
	blueRobots.inputImg = excHSV.clone();

	lamp.search();
	for (unsigned int i = 0; i < lamp.contours.size(); i++) {
		lamp.arrOfContours.push_back(contourArea(lamp.contours[i]));
	}
	auto max = max_element(lamp.arrOfContours.begin(), lamp.arrOfContours.end());
	auto it = distance(lamp.arrOfContours.begin(), max);

	Moments mnts = moments(lamp.contours[it]);
	Point center(int(mnts.m10 / mnts.m00), int(mnts.m01 / mnts.m00) + 100);
	lampOrigin = center;

	circle(out, lampOrigin, 2, (lamp.color[0],lamp.color[1],lamp.color[2]), 2);
/*	Mat test1 = out.clone();

	while (1) {
		out = test1.clone()*/;
		redRobots.work();
		greenRobots.work();
		blueRobots.work();
		imshow("itog", out);
	//	waitKey(100);
	//}

	waitKey(0);
	destroyAllWindows();
	return 0;
}

