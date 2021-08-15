#include <iostream>
#include <memory>
#include <ctime>

#include <grpcpp/grpcpp.h>
#include <CoordinateViewer.grpc.pb.h>
#include <CoordinateViewer.pb.h>

class CoordinateViewerGrpcClient {
private:
    std::unique_ptr<CoordinateViewer::CoordinateViewerService::Stub> mStub;
    CoordinateViewer::Session mSession;
public:
    CoordinateViewerGrpcClient() = delete;

    explicit CoordinateViewerGrpcClient(std::shared_ptr<grpc::Channel> ch)
        : mStub(CoordinateViewer::CoordinateViewerService::NewStub(ch)) {}

    void addPoint(const std::string& imei, 
        const std::string& number,
        float lat,
        float lon)
    {
        CoordinateViewer::CallPoint* new_point = mSession.add_points();
        new_point->set_imei(imei);
        new_point->set_number(number);
        new_point->set_lat(lat);
        new_point->set_lon(lon);        
    }

    void sendSession(unsigned long long timestamp, unsigned long long duration)
    {
        mSession.set_timestamp(timestamp);
        mSession.set_duration(duration);

        grpc::ClientContext ctx;
        CoordinateViewer::ReturnStatus ack;

        grpc::Status status = mStub->add_session(&ctx, mSession, &ack);
        if (status.ok()) {
            mSession.clear_points();
        } else {
            throw std::string("add_session error!");
        }
    }

    void clearSession() 
    {
        mSession.clear_points();
    }
};

int main(int argc, char** argv) {
    CoordinateViewerGrpcClient client(grpc::CreateChannel("localhost:1057", grpc::InsecureChannelCredentials()));
    client.addPoint("AA-BBBBBB-CCCCCC-D", "+7(911)908-46-53", 59.93f, 30.36f);
    client.addPoint("BB-BBBBBB-CCCCCC-D", "+7(911)908-00-00", 55.00f, 37.00f);
    unsigned long long timestamp, duration;
    timestamp = time(nullptr);
    duration = 1000 * 68;
    try {
        client.sendSession(timestamp, duration);
    } catch (std::string& exception) {
        std::cout << exception << "\n";
        return 1;
    }
    std::cout << "New session successfully added\n";
    return 0;
}