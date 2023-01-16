#pragma once

int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

int high = 10;
//int widht = high;

int lowArray = 0, highArray = 1500;

class SearchColorObj {
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
			color[i] = _color[i];
			lowHSV[i] = _lowHSV[i];
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
		Point center, newPoint(0, 0);
		double mindist = origin.rows; // начальное расстояние - расстояния с угла изображения до лампы
		int minIdex = 0;

		for (size_t i = 0; i < contours.size(); i++) {
			int array = contourArea(contours[i]);
			if (array > minArray /*lowArray */ && array < 1200) {
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