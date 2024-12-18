#include <iostream>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "message_queue.hpp"

using namespace std;

struct Node {
    pid_t pid;
    string address;
    int parent_id;
    vector<int> children;
    bool available;
};

unordered_map<int, Node> nodes;
set<int> unavailable_nodes;

void mark_subtree_unavailable(int node_id) {
    if (nodes.find(node_id) == nodes.end()) return;
    
    nodes[node_id].available = false;
    unavailable_nodes.insert(node_id);
    
    for (int child_id : nodes[node_id].children) {
        mark_subtree_unavailable(child_id);
    }
}

void handle_child_exit(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (auto& node : nodes) {
            if (node.second.pid == pid) {
                mark_subtree_unavailable(node.first);
                break;
            }
        }
    }
}

void create_node(int id, int parent_id);
void exec_command(int id, const string& command);
void pingall();

int main() {
    signal(SIGCHLD, handle_child_exit);
    
    string input;
    while (getline(cin, input)) {
        if (input.empty()) continue;

        if (input.substr(0, 6) == "create") {
            int id, parent_id = -1;
            sscanf(input.c_str(), "create %d %d", &id, &parent_id);
            create_node(id, parent_id);
        } else if (input.substr(0, 4) == "exec") {
            int id;
            char cmd[256];
            sscanf(input.c_str(), "exec %d %s", &id, cmd);
            exec_command(id, string(cmd));
        } else if (input == "pingall") {
            pingall();
        } else if (input == "exit") {
            break;
        } else {
            cout << "Неизвестная команда" << endl;
        }
    }

    // Завершаем все дочерние процессы
    for (const auto& node : nodes) {
        kill(node.second.pid, SIGTERM);
    }
    
    return 0;
}

void create_node(int id, int parent_id) {
    if (nodes.find(id) != nodes.end()) {
        cout << "Error: Already exists" << endl;
        return;
    }

    if (parent_id != -1) {
        if (nodes.find(parent_id) == nodes.end()) {
            cout << "Error: Parent not found" << endl;
            return;
        }
        if (!nodes[parent_id].available) {
            cout << "Error: Parent is unavailable" << endl;
            return;
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        cout << "Error: Failed to fork" << endl;
    } else if (pid == 0) {
        execl("./worker", "./worker", to_string(id).c_str(), nullptr);
        exit(0);
    } else {
        Node node;
        node.pid = pid;
        node.address = "tcp://localhost:" + to_string(5550 + id);
        node.parent_id = parent_id;
        node.available = true;
        nodes[id] = node;

        if (parent_id != -1) {
            nodes[parent_id].children.push_back(id);
        }
        cout << "Ok: " << pid << endl;
    }
}

void exec_command(int id, const string& command) {
    if (nodes.find(id) == nodes.end()) {
        cout << "Error:" << id << ": Not found" << endl;
        return;
    }

    if (!nodes[id].available) {
        cout << "Error:" << id << ": Node is unavailable" << endl;
        return;
    }

    MessageQueue mq(nodes[id].address, ZMQ_REQ);
    if (!mq.send(command)) {
        cout << "Error:" << id << ": Node is unavailable" << endl;
        return;
    }

    string reply = mq.receive();
    if (reply.empty()) {
        cout << "Error:" << id << ": Node is unavailable" << endl;
    } else {
        cout << reply << endl;
    }
}

void pingall() {
    string result;
    for (const auto& node : nodes) {
        if (!node.second.available) {
            if (!result.empty()) result += ";";
            result += to_string(node.first);
            continue;
        }

        MessageQueue mq(node.second.address, ZMQ_REQ);
        if (!mq.send("ping") || mq.receive().empty()) {
            if (!result.empty()) result += ";";
            result += to_string(node.first);
            mark_subtree_unavailable(node.first);
        }
    }

    if (result.empty()) {
        cout << "Ok: -1" << endl;
    } else {
        cout << "Ok: " << result << endl;
    }
}