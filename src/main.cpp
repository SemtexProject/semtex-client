#include <iostream>
#include <limits>
#include "ExchangeClient.h"
#include <grpcpp/grpcpp.h>
#include "generated/order/order.pb.h"
#include "generated/order/order.grpc.pb.h"

using exchange::Order;

int main() {
    ExchangeClient client(grpc::CreateChannel(
        "exchange-server:50051", grpc::InsecureChannelCredentials()));

    std::cout << "Welcome to the trading desk.\n";

    while (true) {
        std::cout << "Enter orders as: [ID] [SIDE] [SYMBOL] [QUANTITY] [PRICE] ";
        std::cout << "(Enter 'exit' to quit):\n";

        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") {
            break; // Exit the loop if the user types 'exit'
        }

        std::istringstream iss(input);
        float quantity, price;
        std::string symbol, side;
        int id;
        
        if (!(iss >> id >> side >> symbol >> quantity >> price)) {
            std::cerr << "Invalid input format.\n";
            continue; // Skip the rest of the loop and start over
        }

        Order order;
        order.set_id(id);
        order.set_quantity(quantity);
        order.set_price(price);
        order.set_symbol(symbol);
        if (side == "BUY") order.set_side(0);
        else if (side == "SELL") order.set_side(1);
        else {
            std::cerr << "Invalid side. Use 'BUY' or 'SELL'.\n";
            continue; // Skip the rest of the loop and start over
        }

        // Submit the order
        client.SubmitOrder(order);
    }

    return 0;
}
