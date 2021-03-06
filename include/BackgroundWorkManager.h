#ifndef BACKGROUND_WORK_MANAGER
#define BACKGROUND_WORK_MANAGER
#include <list>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <algorithm>
#include <chrono>

#include "BasicBackgroundWorker.h"

// this class is designed to produce some interface for long time operations, originally created for
// managing CallPoint's, but not needed, since the only operation needed there is time interval check
class BackgroundWorkManager
{
public:
    explicit BackgroundWorkManager();
    ~BackgroundWorkManager();
    void addWorker(BasicBackgroundWorker *item);
    void removeWorker(BasicBackgroundWorker *item);
    void run();
private:
    std::list<BasicBackgroundWorker*> mWorkersList;
    std::mutex mMutex;
    std::atomic<bool> mIsRuning;
    std::unique_ptr<std::thread> mWorkThreadPtr;

    void workThreadFunc();
};

#endif
