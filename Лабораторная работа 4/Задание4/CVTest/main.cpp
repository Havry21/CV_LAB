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

void easyFT(Mat& I, Mat& magI, Mat& out) {
    Mat padded;                            
    int m = getOptimalDFTSize(I.rows);
    int n = getOptimalDFTSize(I.cols); 

    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
    dft(complexI, complexI);            // this way the result may fit in the source matrix
                                        // compute the magnitude and switch to logarithmic scale
                                        // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))

    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude

    magI = planes[0].clone();
    magI += Scalar::all(1);                    // switch to logarithmic scale
    log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
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
    normalize(magI, magI, 0, 1, NORM_MINMAX); // Transform the matrix with float values into a
                                              // viewable image form (float between values 0 and 1).

    // IDFT
    cv::dft(complexI, out, DFT_INVERSE | DFT_REAL_OUTPUT);
    normalize(out, out, 0, 1, NORM_MINMAX);
}

//void easyRDTF(Mat& I, Mat& magI) {
//    normalize()
//}

int main(void) {
    //std::string image_path = samples::findFile("starry_night.jpg");
    std::string image_path = samples::findFile("lenna.png");
    Mat origin = imread(image_path, IMREAD_GRAYSCALE);

    Mat dftImg[3];
    easyFT(origin, dftImg[0], dftImg[1]);
    hconcat(dftImg[0], dftImg[1], dftImg[2]);
    imshow("Origin", dftImg[2]);

    Mat sobelImg[4], sobelOut, sobelImg_x, sobelImg_y, absSobelImg_x, absSobelImg_y;
    sobelImg[0] = origin.clone();
    Sobel(origin, sobelImg_y, -1, 0,1);
    Sobel(origin, sobelImg_x, -1, 0, 1);
    convertScaleAbs(sobelImg_x, absSobelImg_x);
    convertScaleAbs(sobelImg_y, absSobelImg_y);
    addWeighted(absSobelImg_x, 0.5, absSobelImg_y, 0.5, 0, sobelImg[0]);
    easyFT(sobelImg[0], sobelImg[1], sobelImg[2]);
    hconcat(sobelImg[1], sobelImg[2], sobelImg[3]);
    imshow("Sobel", sobelImg[3]);

    Mat boxImg[4];
    boxFilter(origin, boxImg[0], -1, Size(5, 5));
    easyFT(boxImg[0], boxImg[1], boxImg[2]);
    hconcat(boxImg[1], boxImg[2], boxImg[3]);
    imshow("BoxFiler", boxImg[3]);

    Mat laplasImg[4];
    Laplacian(origin, laplasImg[0], -1, 3);
    easyFT(laplasImg[0], laplasImg[1], laplasImg[2]);
    hconcat(laplasImg[1], laplasImg[2], laplasImg[3]);
    imshow("Laplas", laplasImg[3]);


    waitKey();
    return 0;
}


