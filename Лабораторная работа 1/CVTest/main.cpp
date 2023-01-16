#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include<iostream>
#include "windows.h"
#include <cmath>                                     
#define PI 3.14159265358979323846

using namespace cv;
using namespace std;

void MyLine(Mat img, Point start, Point end){
    int thickness = 2;
    int lineType = LINE_8;
    line(img,
        start,
        end,
        Scalar(255, 0, 0),
        thickness,
        lineType);
}

int main(void) {
    Mat fon = imread("C:/Users/dimag/Desktop/fon.jpg");
    Mat robot = imread("C:/Users/dimag/Desktop/robot.png");

    Mat fonWithTrace = fon.clone();
    Mat fonWithTraceCopy;

    Mat robotGrey, robotMask, robotMaskInv;
    cvtColor(robot, robotGrey, COLOR_BGR2GRAY);
    threshold(robotGrey, robotMask, 5, 255, THRESH_BINARY_INV);
    cvtColor(robotMask, robotMask, COLOR_GRAY2BGR);

    Mat roiRobotplace;


    const int period = 250;
    const int ampitude = 50;
    const int dx = 10;
    const int startY = fon.rows/2;
    Point start, end(0, startY);
    namedWindow("First lab", WINDOW_AUTOSIZE);

   
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);


    for (int i = 0; i <= fon.cols - robot.cols; i += dx) { // дискретно идем вдоль оси Х
        start = end;

        if (i == fon.cols / 2) { // "скриншот" на середине 
            imwrite("test.png", fonWithTraceCopy, compression_params);
        }

        /* Задаем положение робота */
        end.x = i;
        end.y = sin(i * PI /period) * ampitude + startY;

        /* Задаем траекторию робота */
        line(fonWithTrace, start, end, Scalar(0, 0, 0), 2, LINE_8);

        fonWithTraceCopy = fonWithTrace.clone();

        /* Накладываем робота */
        roiRobotplace = fonWithTraceCopy(Rect(i, end.y - 80, robot.cols, robot.rows));
        bitwise_and(roiRobotplace, robotMask, roiRobotplace);
        add(roiRobotplace, robot, roiRobotplace);

        waitKey(50);
        imshow("First lab", fonWithTraceCopy);
    }


    waitKey(0);
    destroyAllWindows();
    return 0;
}


