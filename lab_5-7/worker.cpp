#include <iostream>
#include <string>
#include <chrono>
#include <atomic>
#include "message_queue.hpp"

using namespace std;

atomic<bool> running(false);
atomic<long> timer(0);
atomic<long> start_time(0);

long get_current_time_ms() {
    return chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now().time_since_epoch()).count();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: worker <id>" << endl;
        return 1;
    }

    int id = atoi(argv[1]);
    string address = "tcp://*:" + to_string(5550 + id);
    MessageQueue mq(address, ZMQ_REP);

    while (true) {
        string message = mq.receive();
        if (message.empty()) continue;

        if (message == "ping") {
            mq.send("pong");
        } else if (message.substr(0, 5) == "start") {
            if (!running) {
                running = true;
                start_time = get_current_time_ms();
            }
            mq.send("Ok:" + to_string(id));
        } else if (message.substr(0, 4) == "stop") {
            if (running) {
                long current_time = get_current_time_ms();
                timer += current_time - start_time;
                running = false;
            }
            mq.send("Ok:" + to_string(id));
        } else if (message.substr(0, 4) == "time") {
            long total_time = timer;
            if (running) {
                long current_time = get_current_time_ms();
                total_time += current_time - start_time;
            }
            mq.send("Ok:" + to_string(id) + ": " + to_string(total_time));
        } else {
            mq.send("Error:" + to_string(id) + ": Unknown command");
        }
    }

    return 0;
}