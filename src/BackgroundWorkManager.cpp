#include "BackgroundWorkManager.h"

BackgroundWorkManager::BackgroundWorkManager() 
    : mIsRuning(true), mWorkThreadPtr(nullptr) {}
    
BackgroundWorkManager::~BackgroundWorkManager() 
{
    mIsRuning = false;
    if (mWorkThreadPtr.get() == nullptr)
        return;
    mWorkThreadPtr->join();
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
    mWorkThreadPtr = std::make_unique<std::thread>(new std::thread(this, &BackgroundWorkManager::workThreadFunc));
}

void BackgroundWorkManager::workThreadFunc() 
{
    // TODO: work function
}
