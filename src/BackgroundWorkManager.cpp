#include "BackgroundWorkManager.h"

BackgroundWorkManager::BackgroundWorkManager() 
    : mIsRuning(true), mWorkThreadPtr(nullptr) {}
    
BackgroundWorkManager::~BackgroundWorkManager() 
{
    mIsRuning = false;
    if (mWorkThreadPtr.get() == nullptr)
        return;
    mWorkThreadPtr->join();
    for (auto worker : mWorkersList) {
        removeWorker(worker);
    }
}

void BackgroundWorkManager::addWorker(BasicBackgroundWorker *item) 
{
    std::lock_guard<std::mutex> lock(mMutex);
    mWorkersList.push_back(item);
}

void BackgroundWorkManager::removeWorker(BasicBackgroundWorker *item) 
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto eraseElement = std::find(mWorkersList.begin(), mWorkersList.end(), item);
    if (eraseElement == mWorkersList.end())
        return;
    item->stopEvent();
    mWorkersList.erase(eraseElement);
}

void BackgroundWorkManager::run() 
{
    if (mWorkThreadPtr.get() == nullptr)
        return;
    mWorkThreadPtr = std::make_unique<std::thread>(std::thread(&BackgroundWorkManager::workThreadFunc, this));
}

void BackgroundWorkManager::workThreadFunc() 
{
    for (auto& elem : mWorkersList) {
        auto curTime = std::chrono::steady_clock::now();
        if (curTime - elem->getLastTimestamp() > std::chrono::milliseconds(elem->getSleepTime())) {
            elem->doWork();
            elem->setLastTimestamp(curTime);
        }
    }
}
