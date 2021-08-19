#include "CallPointsManager.h"
#include "Renderer.h"

void CallPointsManager::doWork() 
{
    // TODO: background worker implementations
}

void CallPointsManager::stopEvent() 
{
    // TODO: background worker implementations
}

void CallPointsManager::addSession(Session& session) 
{
    for (auto& elem : mSessions) {
        if (elem.status == SessionStatus::DELETED) {
            elem.call_points.clear();
            elem.points = session.points;
            elem.duration = session.duration;
            elem.timestamp = session.timestamp;
            elem.show_sec = session.show_sec;
            elem.add_time_point = session.add_time_point;
            elem.status = SessionStatus::CREATING;
            mHaveNewSessions = true;
            return;
        }
    }
}

void CallPointsManager::addSessionsGuiThreadIfPossible(CoordinateViewerMain::Renderer* guiObject) 
{
    if (mHaveNewSessions.exchange(false)) {
        for (auto& sessionElem : mSessions) {
            if (sessionElem.status == SessionStatus::CREATING) {
                for (auto& singlePoint : sessionElem.points) {
                    auto newPoint = new CallPoint(
                        &guiObject->mManimpulator, 
                        &guiObject->mColoredDrawables, 
                        &guiObject->mColorShader,
                        &guiObject->mPointMesh,
                        singlePoint.lat,
                        singlePoint.lon);
                    newPoint->setImei(singlePoint.imei);
                    newPoint->setNumber(singlePoint.number);
                    sessionElem.call_points.push_back(newPoint);
                }
            }
        }
    }
}

CallPointsManager::CallPointsManager() 
    : mHaveNewSessions(false)
{
    mSessions.resize(SESSION_MAX_SIZE);
    setSleepTime(REFRESH_DURATION);
}
