#include <memory>
#include <chrono>
#include <thread>
#include <iostream>


template<typename duration_t>
class StopWatch{
    using clock_t = std::chrono::steady_clock;
    using time_point_t = clock_t::time_point;

public:
    StopWatch(): start_time(clock_t::now()) {
    }

    duration_t elapsed(){
        time_point_t current_time = clock_t::now();
        return std::chrono::duration_cast<duration_t>(current_time - start_time);
    }

private:
    time_point_t start_time;
    duration_t duration;
};

template<typename Task/*, typename duration_t = std::chrono::seconds*/>
class TaskTimer{
    using clock_t = std::chrono::steady_clock;
    using time_point_t = clock_t::time_point;
    using duration_t = std::chrono::seconds; 

public:
    TaskTimer(Task task, duration_t duration)
    : task_(std::move(task)),
      duration_(duration) {
    }

    void start(){
        std::thread timer_t([self = this](){
            self->cycling();
        });
        timer_t.detach();
    }

    void stop(){
        execute_flag = false;
    }

private:
    void cycling(){
        while(execute_flag){
            start_time_ = current_time();
            auto current_time_ = current_time();

            duration_t passed = std::chrono::duration_cast<duration_t>(current_time_ - start_time_);

            StopWatch<std::chrono::milliseconds> stop_watch;

            while(passed < duration_){
                passed = std::chrono::duration_cast<duration_t>(current_time_ - start_time_);
                current_time_ = current_time();
            }

            std::cout << "Time passed: " << stop_watch.elapsed().count() << std::endl;

            execute();
        }
    }

    void start_with_std(){
        // while(execute_flag){
        //     std::this_thread::sleep_for(duration_);
        //     execute();
        // }
    }

    void execute(){
        task_();
    }

    inline time_point_t current_time() const{
        return clock_t::now();
    }

    Task task_;
    time_point_t start_time_;
    duration_t duration_;

    std::thread cycle_thread;
    std::atomic<bool> execute_flag{true};
};

void task(){
    std::cout << "Hello world" << std::endl;
}

void test(){
    using namespace std::chrono_literals;
    TaskTimer timer(task, 5s);

    timer.start();
    std::this_thread::sleep_for(30s);
    timer.stop();
}

int main() {

    test();
    return 0;
}
