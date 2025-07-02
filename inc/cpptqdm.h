#ifndef __CPPTQDM__
#define __CPPTQDM__

#include <chrono>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <iomanip>
#include <math.h>
#include <thread>
#include <atomic>

class ProgressBar {
private:
	std::chrono::system_clock::time_point start_time;
	int mTerminalWidth;
	int mSetSize;
	std::chrono::system_clock::time_point printPoint;
	int called = 0;
	std::thread progressThread;

	std::atomic<bool> isRunning;

public:
	ProgressBar();
	ProgressBar(int setSize);
	~ProgressBar();

	void getTerminalLength();
	void runProgress();
	void countUp() { called++; }
	void printProgress();
	int getSecond(int num);
	int getMinute(int num);
	void finishProgress();
};


#endif
