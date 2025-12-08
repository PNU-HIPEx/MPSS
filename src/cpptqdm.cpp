#include "cpptqdm.h"
#include <mutex>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>

// 전역 변수 정의
std::atomic<int> ProgressBar::global_counter{0};
std::shared_ptr<ProgressBar> ProgressBar::global_instance = nullptr;
std::mutex ProgressBar::output_mutex;
static std::mutex global_mutex;

// 전역 인스턴스 생성
void ProgressBar::createGlobal(int setSize) {
    std::lock_guard<std::mutex> lock(global_mutex);
    if (!global_instance) {
        global_counter = 0;
        global_instance = std::make_shared<ProgressBar>(setSize);
    }
}

// 전역 카운터 증가
void ProgressBar::incrementGlobal() {
    if (global_instance) {
        global_counter++;
    }
}

// 전역 인스턴스 제거
void ProgressBar::destroyGlobal() {
    std::lock_guard<std::mutex> lock(global_mutex);
    if (global_instance) {
        // 진행 스레드 먼저 정지
        global_instance->isRunning = false;
        if (global_instance->progressThread.joinable()) {
            global_instance->progressThread.join();
        }
        
        // 최종 100% 진행률 바 강제 출력
        auto now = std::chrono::system_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - global_instance->start_time).count();
        
        // 뮤텍스 없이 직접 출력 (이미 스레드가 종료됨)
        {
            std::lock_guard<std::mutex> output_lock(output_mutex);
            
            // 터미널 폭 가져오기
            struct winsize w;
            int terminalWidth = 120;
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
                terminalWidth = w.ws_col;
            }
            
            // 시간 포매팅 - 소요 시간에 맞춰서
            int elapsedSeconds = duration * 0.001;
            bool needHourFormat = global_instance->getHour(elapsedSeconds) > 0;  // 인스턴스를 통해 호출
            std::string elapsedTime = global_instance->formatTime(elapsedSeconds, needHourFormat);
            std::string remainingTime = needHourFormat ? "0:00:00" : "00:00";
            
            // 고정 부분 길이 계산
            std::ostringstream temp_oss;
            temp_oss << "100%[] " << std::setw(7) << global_instance->mSetSize << "/" 
                     << global_instance->mSetSize << " [" << elapsedTime << "<" << remainingTime << "]";
            int fixed_length = temp_oss.str().length();
            
            // 진행률 바 길이 계산
            int barLength = std::max(20, terminalWidth - fixed_length - 5);
            
            // 100% 진행률 바 생성 (모두 채워진 상태)
            std::string progressBar = std::string(barLength, '=');
            
            std::ostringstream oss;
            oss << std::setw(3) << std::right << 100 << "%[" << progressBar << "] "
                << std::setw(7) << std::right << global_instance->mSetSize << "/" 
                << global_instance->mSetSize << " ["
                << elapsedTime << "<" << remainingTime << "]";
            
            std::string output = oss.str();
            
            // 출력
            std::cout << "\r" << output;
            
            // 남은 공간 공백으로 채우기
            if ((int)output.length() < terminalWidth) {
                std::cout << std::string(terminalWidth - output.length(), ' ');
            }
            
            std::cout << std::endl; // 줄바꿈
            std::cout.flush();
        }
        
        global_instance.reset();
    }
}

ProgressBar::ProgressBar(int setSize) : mSetSize(setSize), isRunning(true) {
    start_time = std::chrono::system_clock::now();
    getTerminalLength();
    
    if (isTerminalWidthSufficient()) {
        progressThread = std::thread(&ProgressBar::runProgress, this);
    }
}

ProgressBar::~ProgressBar() {
    finishProgress();
}

bool ProgressBar::isTerminalWidthSufficient() {
    return mTerminalWidth >= 60; // 최소 60자 필요
}

void ProgressBar::getTerminalLength() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        mTerminalWidth = w.ws_col;
    } else {
        mTerminalWidth = 120; // 기본값
    }
}

int ProgressBar::getMinute(int num) {
    return (num / 60) % 60;  // 60으로 나눈 나머지 (시간 제외한 분)
}

int ProgressBar::getSecond(int num) {
    return num % 60;
}

int ProgressBar::getHour(int num) {
    return num / 3600;  // 3600초 = 1시간
}

// 시간 포매팅 함수 수정 - 두 시간 중 더 긴 형식에 맞춤
std::string ProgressBar::formatTime(int totalSeconds, bool forceHourFormat) {
    int hours = getHour(totalSeconds);
    int minutes = getMinute(totalSeconds);
    int seconds = getSecond(totalSeconds);
    
    std::ostringstream oss;
    if (hours > 0 || forceHourFormat) {
        // 시간이 있거나 강제로 시간 형식을 사용하면 h:mm:ss 형식
        oss << hours << ":" 
            << std::setw(2) << std::setfill('0') << minutes << ":"
            << std::setw(2) << std::setfill('0') << seconds;
    } else {
        // 시간이 없으면 mm:ss 형식
        oss << std::setw(2) << std::setfill('0') << minutes << ":"
            << std::setw(2) << std::setfill('0') << seconds;
    }
    return oss.str();
}

void ProgressBar::runProgress() {
    while (isRunning) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        
        // 전역 인스턴스인지 확인 (shared_ptr을 get()으로 포인터 비교)
        int current_count = (global_instance && global_instance.get() == this) ? 
                           global_counter.load() : called.load();
        
        double percent = static_cast<double>(current_count) / mSetSize * 100;
        double duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        double speed = (duration > 0) ? 1000.0 * current_count / duration : 0;
        int left = (speed > 0 && current_count < mSetSize) ? (mSetSize - current_count) / speed : 0;
        
        if (percent > 0 && duration > 0) {
            safeProgressOutput(percent, current_count, mSetSize, duration, speed, left);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // 100%에 도달하면 종료하지만 destroyGlobal()에서 최종 출력 처리
        if (current_count >= mSetSize) {
            isRunning = false;
        }
    }
}

void ProgressBar::finishProgress() {
    isRunning = false;
    
    if (progressThread.joinable()) {
        progressThread.join();
    }
    
    // destroyGlobal()에서 이미 줄바꿈을 처리하므로 여기서는 하지 않음
}

void ProgressBar::safeOutput(const std::string& output) {
    std::lock_guard<std::mutex> lock(output_mutex);
    std::cout << output;
    std::cout.flush();
}

std::string ProgressBar::truncateString(const std::string& str, int maxLength) {
    if ((int)str.length() <= maxLength) {
        return str;
    }
    return str.substr(0, maxLength - 3) + "...";
}

void ProgressBar::safeProgressOutput(double percent, int current, int total, double duration, double /* speed */, int timeLeft) {
    std::lock_guard<std::mutex> lock(output_mutex);
    
    // 첫 번째 스레드만 출력하도록 제한
    static std::thread::id first_thread_id = std::this_thread::get_id();
    if (std::this_thread::get_id() != first_thread_id) {
        return;
    }
    
    // 터미널 폭을 다시 가져오기 (리사이즈 대응)
    struct winsize w;
    int terminalWidth = mTerminalWidth;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        terminalWidth = w.ws_col;
    }
    
    // 둘 중 하나라도 시간 단위가 필요한지 확인
    int elapsedSeconds = duration * 0.001;
    bool needHourFormat = (getHour(elapsedSeconds) > 0) || (getHour(timeLeft) > 0);
    
    // 시간 포매팅 - 둘 다 같은 형식으로
    std::string elapsedTime = formatTime(elapsedSeconds, needHourFormat);
    std::string remainingTime = formatTime(timeLeft, needHourFormat);
    
    // 고정 부분 길이 계산 (퍼센트, 카운터, 시간 정보)
    std::ostringstream temp_oss;
    temp_oss << "100%[] " << std::setw(7) << total << "/" << total 
             << " [" << elapsedTime << "<" << remainingTime << "]";
    int fixed_length = temp_oss.str().length();
    
    // 진행률 바 길이 계산 (터미널 폭에서 고정 부분 빼기)
    int barLength = std::max(20, terminalWidth - fixed_length - 5); // 최소 20자, 여유 5자
    
    // 진행률 바 생성
    int filled = std::max(0, std::min(barLength, (int)(barLength * percent / 100)));
    std::string progressBar = std::string(filled, '=') + std::string(barLength - filled, ' ');
    
    std::ostringstream oss;
    oss << std::setw(3) << std::right << (int)percent << "%[" << progressBar << "] "
        << std::setw(7) << std::right << current << "/" << total << " ["
        << elapsedTime << "<" << remainingTime << "]";
    
    std::string output = oss.str();
    
    // 터미널 폭을 초과하면 자르기
    if ((int)output.length() > terminalWidth) {
        output = output.substr(0, terminalWidth - 3) + "...";
    }
    
    std::cout << "\r" << output;
    
    // 이전 출력보다 짧으면 남은 공간을 공백으로 채우기
    if ((int)output.length() < terminalWidth) {
        std::cout << std::string(terminalWidth - output.length(), ' ');
    }
    
    std::cout.flush();
}