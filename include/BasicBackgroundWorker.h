#ifndef BASIC_BACKGROUND_WORKER
#define BASIC_BACKGROUND_WORKER

class BasicBackgroundWorker 
{
public:
    virtual void doWork() = 0;
    virtual void stopEvent() = 0;
};

#endif
