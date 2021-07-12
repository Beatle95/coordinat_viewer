#ifndef CALL_POINTS_MANAGER
#define CALL_POINTS_MANAGER
#include <mutex>
#include <list>

#include "CallPoint.h"
#include "BasicBackgroundWorker.h"

struct SingleCallPoint
{
    std::string number;
    std::string imei;
    float lat;
    float lon;
};

struct Session 
{
    unsigned long long timestamp;
    unsigned long long duration;
    std::list<SingleCallPoint> points;
    std::list<CallPoint*> call_points;
};

class CallPointsManager : public BasicBackgroundWorker {
public:
    CallPointsManager() { setSleepTime(250); }
    // main working function
    void doWork();
    // performing actions before detaching worker
    void stopEvent();
    // push session from GRPC server thread to sessions list
    void addSession(Session& session) { mSessions.push_back(std::move(session)); }
private:
    // could be used only in main thread!
    std::list<CallPoint*> mCallPoints;
    // temporary storage for sessions, from here they are being pushed to mCallPoints by main thread
    std::list<Session> mSessions;
};

#endif
