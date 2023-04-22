#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const int max_value_H = 360 / 2;
const int max_value = 255;

enum RobotColor{
	RED,
	GREEN, 
	BLUE
}robotColor;

const String window_detection_name = "Object Detection";

Point lampOrigin;

int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

int high = 10;

int lowArray = 0, highArray = 1500;

class SearchColorObj{
public:
	Mat inputImg, workImage, origin;

	int color[4];
	int lowHSV[3];
	int highHSV[3];

	int minArray = 0;
	int kernelSize = 0;
	int numOfRobot = 0;
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
		color[3]   = _color[3];

		minArray = _minArray;
		kernelSize = _kernelSize;
	}

	void search() { // делает пророговую фильтрацию, а потом находит все походящие контуры
		workImage = inputImg.clone();
		// для тестирования с ползунками 
		//inRange(workImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), workImage);

		inRange(workImage, Scalar(lowHSV[0], lowHSV[1], lowHSV[2]), Scalar(highHSV[0], highHSV[1], highHSV[2]), workImage);
		findContours(workImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	}

	void draw() { // рисует контур отсекая неподходящие по площади: слишком маленькие или слишком большие
		for (size_t i = 0; i < contours.size(); i++) {
			int array = contourArea(contours[i]);
			if (array > minArray && array < 1200) {
				drawContours(origin, contours, (int)i, Scalar(color[0], color[1], color[2]), 2, LINE_8, hierarchy, 0);
				numOfRobot++;
			}
		}
		if(color[3] == RED) //  вычитаем на единицу, так как всегда находим еще и лампу
			numOfRobot--;
	}

	cv::String num;
	int a = 0;

	void work() {
		int circle = 8;
		search();
		draw();
		a++;
		
		if(a == circle){
			a = 0;
			numOfRobot /= circle;
			num = format("Num %d", numOfRobot);
			numOfRobot = 0;
		}

		Scalar ScalarColor;
		ScalarColor.val[0] = color[0];
		ScalarColor.val[1] = color[1];
		ScalarColor.val[2] = color[2];

		switch (color[3])
		{
		case RED:
			cv::putText(origin, num, Point(20,20), FONT_HERSHEY_SIMPLEX,1, ScalarColor);
			break;
		case BLUE:
			cv::putText(origin, num, Point(20,50), FONT_HERSHEY_SIMPLEX,1, ScalarColor);
			break;
		case GREEN:
			cv::putText(origin, num, Point(20,80), FONT_HERSHEY_SIMPLEX,1, ScalarColor);
			break;		
		default:
			break;
		}
	}
};

const int blueHSV_H[3] = { 150, 255, 255 };
const int blueHSV_L[3] = { 80, 0, 0 };
const int blueHSV_C[4] = { 255, 0, 0, BLUE };

const int redHSV_H[3] = { 180, 255, 255 };
const int redHSV_L[3] = { 150, 0, 0 };
const int redHSV_C[4] = { 0, 0, 255, RED };

const int greenHSV_H[3] = { 92, 255, 255 };
const int greenHSV_L[3] = { 68, 54, 0 };
const int greenHSV_C[4] = { 0, 255, 0, GREEN};

const int lampHSV_H[3] = { 180, 10, 255 };
const int lampHSV_L[3] = { 0, 0, 0 };
const int lampHSV_C[3] = { 0, 0, 0 };


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

	// low_H 	= blueHSV_L[0];
	// low_S 	= blueHSV_L[1];
	// low_V 	= blueHSV_L[2];
	// high_H	= blueHSV_H[0];
	// high_S	= blueHSV_H[1];
	// high_V 	= blueHSV_H[2];

	namedWindow(window_detection_name, WINDOW_NORMAL);
	//imshow("origin", exc);
	// createTrackbar("Low H",  window_detection_name, &low_H,  max_value_H, on_low_H_thresh_trackbar);
	// createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	// createTrackbar("Low S",  window_detection_name, &low_S,  max_value,   on_low_S_thresh_trackbar);
	// createTrackbar("High S", window_detection_name, &high_S, max_value,   on_high_S_thresh_trackbar);
	// createTrackbar("Low V",  window_detection_name, &low_V,  max_value,   on_low_V_thresh_trackbar);
	// createTrackbar("High V", window_detection_name, &high_V, max_value,   on_high_V_thresh_trackbar);
	// createTrackbar("Low Array", window_detection_name, &lowArray,  highArray, on_low_V_array_trackbar);
	// createTrackbar("High Array",window_detection_name, &highArray, highArray, on_high_V_array_trackbar);
	// createTrackbar("Kernel size", window_detection_name, &high, 50, on_high_size);

    Mat frame;
	Mat frameHSV;
    VideoCapture cap;
    cap.open("/home/dima/Desktop/Robot.mp4");

	int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    VideoWriter video("result.avi", cv::VideoWriter::fourcc('M','J','P','G'), 20, Size(frame_width,frame_height));
	
	cout << "Start grabbing" << endl;
    for (;;)
    {
		cap.read(frame);
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
			cap.set(CAP_PROP_POS_FRAMES, 1);
			continue;
        }
		cvtColor(frame, frameHSV, COLOR_BGR2HSV);

		greenRobots.inputImg  	= frameHSV;
		greenRobots.origin  	= frame;

		redRobots.inputImg   	= frameHSV;
		redRobots.origin   		= frame;

		blueRobots.inputImg  	= frameHSV;
		blueRobots.origin   	= frame;

		greenRobots.work();
		blueRobots.work();
		redRobots.work();

		video.write(frame);
        imshow("Live", frame);
        if (waitKey(5) >= 0)
            break;
    }
	return 0;
}