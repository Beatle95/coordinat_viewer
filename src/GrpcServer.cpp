#include "GrpcServer.h"
#include "CallPointsManager.h"

grpc::Status GrpcServer::add_session(::grpc::ServerContext* context, const ::CoordinateViewer::Session* request, ::CoordinateViewer::ReturnStatus* response) 
{
    Session newSession;
    newSession.timestamp = request->timestamp();
    newSession.duration = request->duration();
    newSession.add_time_point = std::chrono::steady_clock::now();
    for (auto& point : request->points()) {
        SingleCallPoint callPoint;
        callPoint.lat = point.lat();
        callPoint.lon = point.lon();
        callPoint.imei = point.imei();
        callPoint.number = point.number();
        newSession.points.push_back(std::move(callPoint));
    }
    mCallPointsManager->addSession(newSession);
    return grpc::Status::OK;
}
