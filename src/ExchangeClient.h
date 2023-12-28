#pragma once


#include <grpcpp/grpcpp.h>
#include "generated/order/order.pb.h"
#include "generated/order/order.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using exchange::Order;
using exchange::SubmitOrderResponse;
using exchange::ExchangeService;

class ExchangeClient {
public:
    ExchangeClient(std::shared_ptr<Channel> channel)
    : stub_(ExchangeService::NewStub(channel)) {}
    
    
    void SubmitOrder(const Order& order) {
        SubmitOrderResponse response;
        ClientContext context;

        // The actual RPC.
        Status status = stub_->SubmitOrder(&context, order, &response);

        // Act upon its status.
        if (status.ok()) {
            std::cout << "Order submitted successfully: " << response.status() << std::endl;
        } else {
            std::cerr << "RPC failed: " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<ExchangeService::Stub> stub_;
};