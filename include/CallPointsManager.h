#ifndef CALL_POINTS_MANAGER
#define CALL_POINTS_MANAGER
#include <mutex>
#include <list>
#include <atomic>

#include "CallPoint.h"
#include "BasicBackgroundWorker.h"

#define REFRESH_DURATION    250
#define SESSION_MAX_SIZE    150

namespace CoordinateViewerMain {
class CoordinateViewer;
}

enum class SessionStatus{
    CREATING,
    CREATED,
    DELETING,
    DELETED
};

struct SingleCallPoint {
    std::string number;
    std::string imei;
    float lat;
    float lon;
};

struct Session {
    SessionStatus status = SessionStatus::DELETED;
    std::chrono::steady_clock::time_point add_time_point;
    unsigned long long show_sec = 120;

    unsigned long long timestamp;
    unsigned long long duration;
    std::list<SingleCallPoint> points;
    std::list<CallPoint*> call_points;
};

class CallPointsManager : public BasicBackgroundWorker {
public:
    CallPointsManager();
    // main working function
    void doWork() override;
    // performing actions before detaching worker
    void stopEvent() override;
    // push session from GRPC server thread to sessions list
    void addSession(Session& session);
    void addSessionsGuiThreadIfPossible(CoordinateViewerMain::CoordinateViewer* guiObject);

private:
    // temporary storage for sessions, from here they are being pushed to mCallPoints by main thread
    std::vector<Session> mSessions;

    std::atomic<bool> mHaveNewSessions;
};

#endif
