#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include<iostream>

class TickMeters
{
public:
	TickMeters() { reset(); }
	//~TickMeter();

	void start() { startTime = cv::getTickCount(); }
	void stop() {
		int64 time = cv::getTickCount();
		if (startTime == 0)
			return;

		++counter;
		sumTime += (time - startTime);
		startTime = 0;
		reset();
	}

	int64 getTimeTicks()  const { return sumTime;}
	double getTimeMicro() const { return  getTimeMilli() * 1e3; }
	double getTimeMilli() const { return getTimeSec() * 1e3;	}
	double getTimeSec()   const { return (double)getTimeTicks() / cv::getTickFrequency(); }
	int64 getCounter()    const { return counter; }

	void showTime(const char str[]) { stop(); std::cout << str << " " << getTimeMicro() << " mc" << std::endl; 	reset(); }
	void reset() { startTime = 0; sumTime = 0; counter = 0; }

private:
	int64 counter;
	int64 sumTime;
	int64 startTime;
};

