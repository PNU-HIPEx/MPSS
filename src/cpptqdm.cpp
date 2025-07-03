#include "cpptqdm.h"

ProgressBar::ProgressBar(int setSize) : mSetSize(setSize) {
	getTerminalLength();
	start_time = std::chrono::system_clock::now();
	printPoint = start_time;
	std::cout << "\x1b[?25l";
}

ProgressBar::ProgressBar() {
	getTerminalLength();
	start_time = std::chrono::system_clock::now();
	printPoint = start_time;
}

ProgressBar::~ProgressBar() {
	std::cout << "\x1b[?25h" << std::endl;
}

void ProgressBar::getTerminalLength() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	mTerminalWidth = w.ws_col;
}

int ProgressBar::getMinute(int num) {
	return (num % 3600) / 60;
}

int ProgressBar::getSecond(int num) {
	return num % 60;
}

void ProgressBar::printProgress() {
	called++;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	if ( std::chrono::duration_cast<std::chrono::milliseconds>(now - printPoint).count() > 1 || called == mSetSize || called == 1 ) {
		double percent = (double) called / mSetSize * 100;
		double duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
		double speed = 1000. * called / duration;
		int left = (mSetSize - called) / speed;
		int iBar = mTerminalWidth - (32 + 2 * (floor(log10(mSetSize)) + 1) + (getMinute(duration * 0.001) > 99 ? floor(log10(getMinute(duration * 0.001))) + 1 : 2) + (getMinute(left) > 99 ? floor(log10(getMinute(left))) + 1 : 2));
		printPoint = now;
		std::cout << "\r" << std::setw(3) << std::setfill(' ') << std::fixed << (int) percent
			<< "%[" << std::setw(floor(iBar * percent / 100 - 0.000001)) << std::setfill('=') << ""
			<< std::setw(floor(iBar * (100 - percent) / 100) + 1) << std::setfill(' ') << "]"
			<< std::setw(floor(log10(mSetSize)) + 1) << std::setfill(' ') << called << "/" << mSetSize
			<< " ["
			<< std::setw(2) << std::setfill('0') << getMinute(duration * 0.001) << ":" << std::setw(2) << std::setfill('0') << getSecond(duration * 0.001) << "<"
			<< std::setw(2) << std::setfill('0') << getMinute(left) << ":" << std::setw(2) << std::setfill('0') << getSecond(left) << ", "
			<< std::setprecision(10 - 1 - (speed == 0 ? 1 : (int) log10(std::abs((int) speed)) + 1)) << speed << "it/s]";
	}

}