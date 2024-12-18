#include "message_queue.hpp"
#include <iostream>

MessageQueue::MessageQueue(const std::string& address, int type)
    : context(1), socket(context, type) {
    try {
        if (type == ZMQ_REQ) {
            socket.connect(address);
        } else if (type == ZMQ_REP) {
            socket.bind(address);
        }
        
        int timeout = 1000;
        socket.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
        socket.setsockopt(ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
        
    } catch (const zmq::error_t& e) {
        std::cerr << "Ошибка ZeroMQ: " << e.what() << std::endl;
    }
}

MessageQueue::~MessageQueue() {
    close();
}

bool MessageQueue::send(const std::string& message, int timeout_ms) {
    try {
        zmq::message_t msg(message.size());
        memcpy(msg.data(), message.data(), message.size());
        return socket.send(msg, zmq::send_flags::none).has_value();
    } catch (const zmq::error_t&) {
        return false;
    }
}

std::string MessageQueue::receive(int timeout_ms) {
    try {
        zmq::message_t msg;
        zmq::recv_result_t result = socket.recv(msg, zmq::recv_flags::none);
        if (result) {
            return std::string(static_cast<char*>(msg.data()), msg.size());
        }
    } catch (const zmq::error_t&) {}
    return "";
}

void MessageQueue::close() {
    socket.close();
    context.close();
}