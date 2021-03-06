#include <iostream>
#include <zmq.h>
#include <unistd.h>
#include <fcntl.h>
#include <zmq.hpp>
#include <unistd.h>
#include <string>
#include "server.hpp"

using namespace std;

int TopVertex = 0;

void create(int port, zmq::socket_t& socket)
{
    int NodeId;
    cin >> NodeId;
    if (TopVertex == 0) {
        int NodePid = fork();
        if (NodePid == -1) {
            printf("Error: fail fork()\n");
            return;
        } else if (NodePid == 0) {
            execl("./server", "server", to_string(NodeId).c_str(), to_string(port).c_str(), NULL);
        } else {
            cout << "Ok: " << NodePid << "\n";
        }
        TopVertex = 1;
    } else {
        sendMessage(socket, "create " + to_string(NodeId));
        string Ans = receiveMessage(socket);
        cout << Ans;
    }
}

void exec(zmq::socket_t& socket)
{
    int NodeId;
    string text, pattern;
    cin >> NodeId;
    if (TopVertex == 0) {
        cout << "Error: " + std::to_string(NodeId) + ": Not found\n";
        return;
    }
    cin >> text >> pattern;
    sendMessage(socket, "exec " + to_string(NodeId) + " " + text + " " + pattern);
    string Ans = receiveMessage(socket);
    cout << Ans;
}

void heartbit(zmq::socket_t& socket)
{
    int time;
    cin >> time;
    if (TopVertex == 0) {
        cout << "No one avaible node\n";
        return;
    }
    sendMessage(socket, "heartbit " + to_string(time));
}

void pingid(zmq::socket_t& socket)
{
    int NodeId;
    cin >> NodeId;
    if (TopVertex == 0) {
        cout << "No one avaible node\n";
        return;
    }
    sendMessage(socket, "pingid " + to_string(NodeId));
    string ans = receiveMessage(socket);
    cout << ans;
}

int main()
{
    zmq::context_t context(1);
    zmq::socket_t Rule_socket(context, ZMQ_REQ);
    int port = bindSocket(Rule_socket);
    string s, text, pattern;
    int id, parent, time;
    while (true) {
        cin >> s;
        if (s == "create") {
            create(port, Rule_socket);
        } else if (s == "exec") {
            exec(Rule_socket);
        } else if (s == "heartbit") {
            heartbit(Rule_socket);
        } else if (s == "pingid") {
            pingid(Rule_socket);
        } else {
            cout << "Error Command\n";
        }
    }
    return 0;
}   