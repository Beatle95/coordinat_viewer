#ifndef GRPC_SERVER
#define GRPC_SERVER
#include <grpcpp/grpcpp.h>
#include <CoordinateViewer.grpc.pb.h>
#include <CoordinateViewer.pb.h>

#include <chrono>

class CallPointsManager;

class GrpcServer final : public CoordinateViewer::CoordinateViewerService::Service 
{
public:
    GrpcServer() = delete;
    explicit GrpcServer(CallPointsManager *manager) : mCallPointsManager(manager) {}
    ::grpc::Status add_session(::grpc::ServerContext* context, const ::CoordinateViewer::Session* request, ::CoordinateViewer::ReturnStatus* response) override;
private:
    CallPointsManager *mCallPointsManager;
};

#endif
