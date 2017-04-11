
#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "suresecureivs.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using suresecureivs::Event;
using suresecureivs::HeartbeatRequest;
using suresecureivs::GeneralReply;
using suresecureivs::SurvCenterService;
using suresecureivs::AnnotatedImage;
using suresecureivs::Target;

class EventReportingClient {
public:
  EventReportingClient(std::shared_ptr<Channel> channel)
      : stub_(SurvCenterService::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string ReportEvent(const std::string &user) {
    // Data we are sending to the server.
    Event request;
    request.set_description(user);
    AnnotatedImage *anno_img = request.add_anno_imgs();
    Target *ntarget = anno_img->add_targets();
    ntarget->set_x(0);
    ntarget->set_y(1);
    ntarget->set_w(2);
    ntarget->set_h(3);
    ntarget->set_type(suresecureivs::Target_Type_PERSON);
    anno_img->set_img("123");
    request.set_type(0);

    // Container for the data we expect from the server.
    GeneralReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ReportEvent(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }
  std::string Heartbeat(const std::string &device_ident) {
    GeneralReply reply;
    ClientContext context;
    HeartbeatRequest request;
    request.set_device_ident(device_ident);
    Status status = stub_->Heartbeat(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      return "RPC failed";
    }
  }

private:
  std::unique_ptr<SurvCenterService::Stub> stub_;
};

int main(int argc, char **argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  // EventReportingClient reporter(grpc::CreateChannel(
  //    "192.168.3.42:50051", grpc::InsecureChannelCredentials()));
  EventReportingClient reporter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = reporter.ReportEvent(user);
  std::cout << "Reporter received: " << reply << std::endl;
  reply = reporter.Heartbeat("kkk");
  std::cout << "Heartbeat received: " << reply << std::endl;

  return 0;
}
