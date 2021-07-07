#include "GrpcServer.h"

grpc::Status GrpcServer::add_session(::grpc::ServerContext* context, const ::CoordinateViewer::Session* request, ::CoordinateViewer::ReturnStatus* response) 
{
    unsigned long long timestamp = request->timestamp();
    unsigned long long duration = request->duration();
    for (auto& point : request->points()) {
        
    }
    return grpc::Status::OK;
}
