#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


#include <iostream>
#include <time.h>
#include <cmath>    
#include <complex>
#include <vector>

using namespace std;

#define PI 3.14159265358979323846

class myDiscrFurie {
    const int _M;
    const complex<double> ci = complex<double>(std::polar(1.0,(2 * PI / _M))); // получение константы из методички
    bool WCalc = false;

    void calkW() // расчет матрицы комплесных экспонент
    {
        WCalc = true; // значит что матрица уже посчитана
         
        for (int i = 0; i < _M; i++) {
            for (int j = 0 + i; j < _M; j++) { 
                W(i, j) = pow(ci, i * j); // я так и не смог понять, как это делать рекурсивно
                W(j, i) = W(i, j);        // так как матрица симметрична относительно главной диагонали 
            }
        }
    }
public:
    cv::Mat_<complex<double>> F;
    cv::Mat_<complex<double>> W;
    cv::Mat_<complex<double>> f;

    myDiscrFurie(vector<double>* _inputArray, int size) : _M(size)
    {
        W = cv::Mat_<complex<double>>(_M, _M);
        F = cv::Mat_<complex<double>>(_M,  1);
        f = cv::Mat_<complex<double>>(_M,  1);
        
        for (auto j = 0; j < _M; j++) {
            f(j,0) = complex<double>(_inputArray->at(j), 0);// делаем комлпексное число из обычного
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

void fftRecurs(vector<complex<double>>* arr) 
{
    int M = arr->size();
    if (M <= 1) { return; } // значит на уровне выше двухточеные ДПФ 

    vector< complex<double>> odd(M / 2);
    vector< complex<double>> even(M / 2);

    // сам процесс дробления
    for (int i = 0; i < M / 2; i++) {
        even.at(i) = arr->at(i * 2);
        odd.at(i) = arr->at(i * 2 + 1);
    }

    fftRecurs(&even);
    fftRecurs(&odd);

    // само ДПФ
    for (int k = 0; k < M / 2; k++) {
        std::complex<double> bw = odd.at(k) * exp(std::complex<double>(0, -2 * PI * k / M)); // B * Wn 
        arr->at(k) = even.at(k) + bw;
        arr->at(M / 2 + k) = even.at(k) - bw;
    }
}

void fastDTF(vector<double>* _inputArray, vector<complex<double>>* _outArray) 
{
    for (int i = 0; i < _inputArray->size(); i++) {
        _outArray->push_back(complex<double>(_inputArray->at(i), 0));
    }

    fftRecurs(_outArray); // начало разделения
}

int main(void) {
    int size = 8000;
    vector<double> a;
    vector<complex<double>> out;
    for (int i = 0; i < size; i++) {
        a.push_back(i);
    }

    clock_t begin = clock();
    myDiscrFurie focuPocus(&a, size);
    focuPocus.DTF();
    clock_t endMyDTF = clock() - begin;

    begin = clock();
    fastDTF(&a,&out);
    clock_t endMyTTF = clock() - begin;

    begin = clock();
    cv::dft(a, out, cv::DFT_COMPLEX_OUTPUT);
    clock_t endCVDTF = clock() - begin;

    cout << "DTF - " << endMyDTF << endl;           // получилось около 4500, все-таки возведение в степень отвратительная идея
    cout << "Fast DTF - " << endMyTTF <<endl;       // получилось около 4
    cout << "OpenCV DTF - " << endCVDTF << endl;    // получилось около 1

    return 0;
}


