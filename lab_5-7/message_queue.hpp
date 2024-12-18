#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <zmq.hpp>
#include <string>

class MessageQueue {
public:
    MessageQueue(const std::string& address, int type);
    ~MessageQueue();

    bool send(const std::string& message, int timeout_ms = 1000);
    std::string receive(int timeout_ms = 1000);

    void close();

private:
    zmq::context_t context;
    zmq::socket_t socket;
};

#endif // MESSAGE_QUEUE_HPP