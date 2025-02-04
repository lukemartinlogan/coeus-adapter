//
// Created by Hua on 2/4/2025.
//
#include <iostream>
#ifndef COEUS_ADAPTER_TIMER_H
#define COEUS_ADAPTER_TIMER_H
class Timer {
public:
    explicit Timer(const std::string& timer_name, bool start_now = false)
            : name(timer_name), time_elapsed(0), running(false) {
        if (start_now) {
            startTime();
        }
    }

    void startTime() {
        t1 = std::chrono::high_resolution_clock::now();
        time_elapsed = 0;
        running = true;
    }

    __attribute__((always_inline)) void resumeTime() {
        if (!running) {
            t1 = std::chrono::high_resolution_clock::now();
            running = true;
        }
    }

    __attribute__((always_inline)) void pauseTime() {
        if (running) {
            auto t2 = std::chrono::high_resolution_clock::now();
            time_elapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            running = false;
        }
    }

    double endTime() {
        if (running) {
            pauseTime();
        }
        return time_elapsed / 1000000.0;
    }

    double getTimeElapsed() {
        return time_elapsed / 1000000.0;
    }

    void print_csv() {
        if (running) {
            pauseTime();
        }
        std::cout << name << "," << getTimeElapsed() << std::endl;
    }

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point t1;
    double time_elapsed;
    bool running;
};

#endif //COEUS_ADAPTER_TIMER_H
