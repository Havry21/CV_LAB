#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include<iostream>
#include <cmath>    
#include<complex>
#include<vector>

using namespace cv;

#define PI 3.14159265358979323846

void krasivSpektr(Mat& magI) {
	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

void lowPassFilter(Mat& I, Mat& out) {
    Mat padded;                            
    int m = getOptimalDFTSize(I.rows);
    int n = getOptimalDFTSize(I.cols); 

    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
    dft(complexI, complexI);            

    Vec2f cmplxValue;
    double cutOff = 350;
    double D; // radius
    double centerI = complexI.rows / 2;
    double centerJ = complexI.cols / 2;
    double H;
    for (int i = 0; i < complexI.rows; i++) {
        for (int j = 0; j < complexI.cols; j++) {
            D = sqrt(pow(i-centerI,2) + pow(j-centerJ,2));
            H = D <= cutOff ? 0 : 1;
            cmplxValue = complexI.at<Vec2f>(i, j);
            cmplxValue.val[0] *= H;
            cmplxValue.val[1] *= H;
            complexI.at<Vec2f>(i, j) = cmplxValue;
        }
    }

    // IDFT
    cv::dft(complexI, out, DFT_INVERSE | DFT_REAL_OUTPUT);
    normalize(out, out, 0, 1, NORM_MINMAX);
}

void hightPassFilter(Mat& I, Mat& out) {
    Mat padded;
    int m = getOptimalDFTSize(I.rows);
    int n = getOptimalDFTSize(I.cols);

    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
    dft(complexI, complexI);

    Vec2f cmplxValue;
    double cutOff = 350;
    double D; // radius
    double centerI = complexI.rows / 2;
    double centerJ = complexI.cols / 2;
    double H;
    for (int i = 0; i < complexI.rows; i++) {
        for (int j = 0; j < complexI.cols; j++) {
            D = sqrt(pow(i - centerI, 2) + pow(j - centerJ, 2));
            H = D >= cutOff ? 0 : 1;
            cmplxValue = complexI.at<Vec2f>(i, j);
            cmplxValue.val[0] *= H;
            cmplxValue.val[1] *= H;
            complexI.at<Vec2f>(i, j) = cmplxValue;
        }
    }

    // IDFT
    cv::dft(complexI, out, DFT_INVERSE | DFT_REAL_OUTPUT);
    normalize(out, out, 0, 1, NORM_MINMAX);
}

int main(void) {
    //std::string image_path = samples::findFile("starry_night.jpg");
    std::string image_path = samples::findFile("lenna.png");
    Mat origin = imread(image_path, IMREAD_GRAYSCALE);

    Mat dftImg[2];
    lowPassFilter(origin, dftImg[0]);
    imshow("LowPass", dftImg[0]);

    hightPassFilter(origin, dftImg[1]);
    imshow("HightPass", dftImg[1]);

    waitKey();
    return 0;
}


