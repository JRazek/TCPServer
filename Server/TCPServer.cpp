//
// Created by user on 17.07.2021.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <zconf.h>
#include <iostream>
#include "TCPServer.h"
#include "../transferUtils/TransferObjectData.h"
#include "Socket.h"
#include "Listener.h"

TCPServer::TCPServer(){}

int TCPServer::killSocket(int socketID) {
    delete this->sockets[socketID];
    return 0;
}

int TCPServer::socketsCount() {
    return this->sockets.size();
}

TCPServer::~TCPServer() {
    std::unique_lock<std::mutex> uniqueLock (this->mutex);
    this->serverDoneCondition.wait(uniqueLock, [](){
        //todo condition for server death
        return 1;
    });

    for(int i = 0; i < this->listeners.size(); i ++){
        auto p = listeners[i];
        delete p; //releases the accept hold state ( kills socket )
    }
    for(int i = 0; i < this->sockets.size(); i ++){
        delete this->sockets[i];
    }
}

void TCPServer::notifyAccept(int socketFileDescriptor) {
    std::lock_guard guard(this->mutex);
    Socket * socket = new Socket(this->sockets.size(), socketFileDescriptor, this, 4096);
    this->sockets[socket->id] = socket;
    socket->run();
}

std::thread * TCPServer::run() {
    std::thread * run = new std::thread([this]() {
        std::vector<std::thread *> threads;
        for(auto l : listeners){
            threads.push_back(l.second->run());
        }
        for(auto t : threads){
            t->join();
        }
    });
    return run;
}

void TCPServer::addListener(in_port_t port) {
    Listener *listener = new Listener(this->listeners.size(), this, port, 4);
    this->listeners[listener->getId()] = listener;
}

void TCPServer::notifyNewPacket(int socketID, std::vector<byte> &data) {
    //override it
}
