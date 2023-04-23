#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(void) {
    Mat A = imread("/home/dima/Desktop/img/portrait.jpg", IMREAD_GRAYSCALE);
    Mat B = imread("/home/dima/Desktop/img/glaz.png", IMREAD_GRAYSCALE);
	imshow("Portrait", A);
	imshow("Glaza", B);
    int dft_M = getOptimalDFTSize(A.rows);
    int dft_N = getOptimalDFTSize(A.cols);

    Mat dft_A = Mat::zeros(dft_M, dft_N, CV_32F);
    Mat dft_B = Mat::zeros(dft_M, dft_N, CV_32F);

    Mat dft_A_part = dft_A(Rect(0, 0, A.cols, A.rows));
    Mat dft_B_part = dft_B(Rect(0, 0, B.cols, B.rows));
    // это пороговое преобразование 
    A.convertTo(dft_A_part, dft_A_part.type(), 1, -mean(A)[0]);
    B.convertTo(dft_B_part, dft_B_part.type(), 1, -mean(B)[0]);

    dft(dft_A, dft_A, 0, A.rows);
    dft(dft_B, dft_B, 0, B.rows);

    mulSpectrums(dft_A, dft_B, dft_A, 0, true);
    idft(dft_A, dft_A, DFT_SCALE, A.rows + B.rows - 1);

    Mat corr = dft_A(Rect(0, 0, A.cols , A.rows));
    normalize(corr, corr, 0, 1, NORM_MINMAX, corr.type());
    pow(corr, 3.0, corr);

    imshow("Correlation", corr);
    waitKey(-1);

    return 0;
}