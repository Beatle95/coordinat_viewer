#ifndef GRPC_SERVER
#define GRPC_SERVER
#include <grpcpp/grpcpp.h>
#include <CoordinateViewer.grpc.pb.h>
#include <CoordinateViewer.pb.h>

class CallPointsStorage;

class GrpcServer final : public CoordinateViewer::CoordinateViewerService::Service {
public:
    GrpcServer(CallPointsStorage *storage) : mPointsStorage(storage) {}
    ::grpc::Status add_session(::grpc::ServerContext* context, const ::CoordinateViewer::Session* request, ::CoordinateViewer::ReturnStatus* response) override;
private:
    CallPointsStorage *mPointsStorage;
};

#endif
