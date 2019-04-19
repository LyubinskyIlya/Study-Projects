#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

class Semaphore
{
    std::mutex mutex_;
    std::condition_variable condition_;
    int counter_;
public:
    explicit Semaphore(int initialValue = 1)
        : counter_(initialValue)
    {
    }

    void enter()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, 
            [this]()
            { 
                return counter_ > 0;
            });
        --counter_;
    }

    void leave()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        ++counter_;
        condition_.notify_one();
    }
};

Semaphore ping(1);
Semaphore pong(0);
const int steps = 500000;

void printping()
{
	for (int i = 0; i < steps; i++)
	{
		ping.enter();
		std::cout << "ping\n";
		pong.leave();

	}
}

void printpong()
{
	for (int i = 0; i < steps; i++)
	{
		pong.enter();
		std::cout << "pong\n";
		ping.leave();
	}
}

int main()
{
	std::thread ping(printping);
	std::thread pong(printpong);
	ping.detach();
	pong.join();
	return 0;
}
