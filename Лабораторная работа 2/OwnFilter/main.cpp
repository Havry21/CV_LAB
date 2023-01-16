#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include<iostream>
#include <cmath>                                     

#include"TickMeter.h"

#define PI 3.14159265358979323846

using namespace cv;
using namespace std;


int srednee(Mat img,int i,int j, int _size = 3) {
    int size = _size;
    int summ = 0;
    for (int h = -(size - 1)/2; h <= (size - 1)/2; h++) {
        for (int w = -(size - 1) / 2; w <= (size - 1) / 2; w++) {
            summ += img.at<uchar>(i + h,j + w);
        }
    }
    return summ/9;
}

void myFilter(Mat img,Mat& OutArray, int _size = 3) {
    int size = _size;
    OutArray = img.clone();
    int a = 0;
    for (int i = (size - 1); i < OutArray.rows - (size - 1); i++) {
        for (int j = (size - 1); j < OutArray.cols - (size - 1); j++) {
            a = srednee(img, i, j);
            OutArray.at<uchar>(i, j) = a;
        }
    }
}

int laplas(Mat img, int i, int j) {
    double k = 0.3;
    const int kernel[3][3] = {
        {0, 1, 0},
        {1,-4, 1},
        {0, 1, 0}
    };

    int size = 3;
    int summ = 0;
    for (int h = -(size - 1) / 2; h <= (size - 1) / 2; h++) {
        for (int w = -(size - 1) / 2; w <= (size - 1) / 2; w++) {
            summ += img.at<uchar>(i + h, j + w) * kernel[h + (size - 1)/2][w + (size - 1)/2];
        }
    }
    return summ * k;
}

void myLaplas(Mat img, Mat& OutArray) {
    OutArray = img.clone();
    int a = 0;

    for (int i = 2; i < OutArray.rows - 2; i++) {
        for (int j = 2; j < OutArray.cols - 2; j++) {
            a = laplas(img, i, j);
            OutArray.at<uchar>(i, j) += a;
        }
    }
}

int main(void) {
    Mat origin = imread("C:/Users/dimag/Desktop/Lenna.png");
    Mat greyLenna;
    Mat newLenna, normFiltr;

    cvtColor(origin, greyLenna, COLOR_BGR2GRAY);
    TickMeters sredneeFiler;

    sredneeFiler.start();
    myFilter(greyLenna, newLenna);
    sredneeFiler.showTime("My filter");

    sredneeFiler.start();
    blur(greyLenna, normFiltr, Size(3, 3));
    sredneeFiler.showTime("Blur filter");

    imshow("norm", normFiltr);
    imshow("origin", greyLenna);
    imshow("First lab", newLenna);

    float diffpct = norm(newLenna,normFiltr) / (newLenna.rows * newLenna.cols);
    cout << diffpct;

    Mat gausissianLenna;
    GaussianBlur(greyLenna, gausissianLenna, Size(5, 5), 4);

    Mat diffLennaGaus, diffLennaBoxfiltr;

    absdiff(greyLenna, gausissianLenna, diffLennaGaus);
    addWeighted(greyLenna, 1, diffLennaGaus, 2,0, diffLennaGaus);

    absdiff(greyLenna, normFiltr, diffLennaBoxfiltr);
    addWeighted(greyLenna, 1, diffLennaBoxfiltr, 2, 0, diffLennaBoxfiltr);

    Mat itogImg;
    absdiff(diffLennaBoxfiltr, diffLennaGaus, itogImg);
    imshow("diff", itogImg);

    Mat myLaplasImg;
    myLaplas(greyLenna, myLaplasImg);
    imshow("My laplas", myLaplasImg);

    Mat diffmyLaplasImg;

    absdiff(greyLenna, myLaplasImg, diffmyLaplasImg);
    addWeighted(greyLenna, 1, diffmyLaplasImg, 1.5, 0, diffmyLaplasImg);
    imshow("My laplas diff", diffmyLaplasImg);

    waitKey(0);
    destroyAllWindows();
    return 0;
}


