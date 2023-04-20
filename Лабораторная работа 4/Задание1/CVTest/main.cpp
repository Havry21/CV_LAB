#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include<iostream>
#include <cmath>
#include<complex>
#include<vector>

using namespace std;

#define PI 3.14159265358979323846

class myDiscrFurie {
    const int _M;
    const complex<double> ci = complex<double>(std::polar(1.0,(2 * PI / _M))); // получение константы из методички
    bool WCalc = false;

    void calkW() // расчет матрицы комплесных экспонент
    {
        cout << endl;
        cout << endl;

        WCalc = true;
         
        for (int i = 0; i < _M; i++) {
            for (int j = 0 + i; j < _M; j++) { 
                W(i, j) = pow(ci, i * j); // я так и не смог понять, как это делать рекурсивно
                W(j, i) = W(i, j);        // так как матрица симметрична относительно главной диагонали 
            }
        }

        // for (int i = 0; i < _M; i++) {
        //     for (int j = 0; j < _M; j++)
        //     {
        //         cout << W(i, j) << " ";
        //     }
        //     cout << endl;
        // }
        // cout << endl;
        // cout << endl;
    }
public:
    cv::Mat_<complex<double>> F;
    cv::Mat_<complex<double>> W;
    cv::Mat_<complex<double>> f;

    myDiscrFurie(double _inputArray[], int size) : _M(size)
    {
        W = cv::Mat_<complex<double>>(_M, _M);
        F = cv::Mat_<complex<double>>(_M,  1);
        f = cv::Mat_<complex<double>>(_M,  1);
        cout << "Input Array" << endl;
        for (auto j = 0; j < _M; j++) {
            f(j,0) = complex<double>(_inputArray[j], 0);// делаем комлпексное число из обычного
            cout << f[j][0] << endl;
        }

    }

    void IDFT() 
    {
        if (!WCalc) { calkW(); }
        f = W * F;
        f /= _M;
    }

    void DTF() {
        if (!WCalc) { calkW(); }
        F = W * f;
    }

};


int main(void) {
    double a[3] = { 1,2,3 };
    myDiscrFurie focuPocus(a, 3);

    focuPocus.DTF();

    cout << "my DTF" << endl;
    for (auto i = 0; i < 3; i++) { 
        cout << focuPocus.F[i][0] << endl;
    }
    cout << endl;

    vector<double> openCvArray = {1, 2, 3 };
    vector<complex<double>> outArray;
    cv::dft(openCvArray, outArray, cv::DFT_COMPLEX_OUTPUT);

    cout << "openCV DTF" << endl;
    std::for_each(outArray.begin(), outArray.end(),
            [](const complex<double>& n)
            {
                cout << n.real() << " " << n.imag() << endl;
            }
    );
    cout << endl;



    focuPocus.IDFT();
    cout << "After IDTF, only real" << endl;
    for (auto i = 0; i < 3; i++) { // действительная часть сходится с изначально заданной, значит алгоритм работает 
        cout << focuPocus.f[i][0].real() << endl;
    }

    return 0;
}


