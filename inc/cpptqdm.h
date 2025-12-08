#ifndef CPPTQDM_H
#define CPPTQDM_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

class ProgressBar {
private:
    int mSetSize;
    std::atomic<int> called{0};
    std::chrono::system_clock::time_point start_time;
    std::atomic<bool> isRunning;
    std::thread progressThread;
    int mTerminalWidth;
    
    // 멀티스레딩을 위한 뮤텍스
    static std::mutex output_mutex;
    
    // 전역 변수들
    static std::atomic<int> global_counter;
    static std::shared_ptr<ProgressBar> global_instance;

public:
    ProgressBar(int setSize);
    ~ProgressBar();
    
    // 전역 인스턴스 관리
    static void createGlobal(int setSize);
    static void incrementGlobal();
    static void destroyGlobal();
    
    bool isTerminalWidthSufficient();
    void getTerminalLength();
    int getMinute(int num);
    int getSecond(int num);
    int getHour(int num);  // 새로운 함수
    std::string formatTime(int totalSeconds);  // 새로운 함수
    std::string formatTime(int totalSeconds, bool forceHourFormat = false);  // 매개변수 추가
    void runProgress();
    void finishProgress();
    void safeOutput(const std::string& output);
    std::string truncateString(const std::string& str, int maxLength);
    void safeProgressOutput(double percent, int current, int total, double duration, double speed, int timeLeft);
};

#endif
