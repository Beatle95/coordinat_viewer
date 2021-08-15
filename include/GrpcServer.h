#ifndef GRPC_SERVER
#define GRPC_SERVER
#include <chrono>
#include <thread>
#include <memory>

#include <grpcpp/grpcpp.h>
#include <CoordinateViewer.grpc.pb.h>
#include <CoordinateViewer.pb.h>

class CallPointsManager;

class GrpcServerImpl {
private:
    std::unique_ptr<std::thread> mGrpcThread;
    std::unique_ptr<grpc::Server> mServer;
public:
    GrpcServerImpl() = delete;
    GrpcServerImpl(CallPointsManager* manager);
    ~GrpcServerImpl();
};

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
