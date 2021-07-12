#ifndef BASIC_BACKGROUND_WORKER
#define BASIC_BACKGROUND_WORKER
#include <chrono>

class BasicBackgroundWorker 
{
public:
    BasicBackgroundWorker() { mTimePoint = std::chrono::steady_clock::now(); }
    virtual ~BasicBackgroundWorker() {}

    // main working function
    virtual void doWork() = 0;
    // performing actions before detaching worker
    virtual void stopEvent() = 0;

    // returning sleep duration in milliseconds
    int getSleepTime() { return mSleepTime; }
    void setSleepTime(int sleepTime) { mSleepTime = sleepTime; }
    // functions for time periods measurment
    std::chrono::steady_clock::time_point getLastTimestamp() { return mTimePoint; }
    void setLastTimestamp(std::chrono::steady_clock::time_point timePoint) { mTimePoint = timePoint; }
private:
    std::chrono::steady_clock::time_point mTimePoint;
    int mSleepTime = 1000;
};

#endif
