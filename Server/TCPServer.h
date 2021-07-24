//
// Created by user on 17.07.2021.
//

#ifndef TCPROTOCOLS_TCPSERVER_H
#define TCPROTOCOLS_TCPSERVER_H
#include <vector>
#include <netinet/in.h>
#include <unordered_map>
#include <thread>
#include <future>

struct Socket;
struct Listener;

typedef unsigned short u_short;
typedef unsigned char byte;

class TCPServer {
private:

    std::mutex mutex;

    std::unordered_map<int, Socket *> sockets;//id, val

    std::unordered_map<int, Listener *> listeners;//id, val

    std::condition_variable serverDoneCondition;

    /**
     *
     * @param socketID
     * kills the socket.
     * @return 0 on success
     */
    int killSocket(int socketID);


public:

    /**
     * init
     * */
    TCPServer();

    /**
     * accessed from other threads. Must be secure.
     * @param socketFileDescriptor
     */

    void notifyAccept(int socketFileDescriptor);


    void notifyNewPacket(int socketID, std::vector<byte> &data);

    /**
     * inits listeners to run
     */
    void addListener(in_port_t port);


    /**
     * runs the server
     */
    void run();


   /**
    * destructor, manages all the dynamic data
    */
   ~TCPServer();

/**
 *
 * @return sockets count
 */
int socketsCount();
};


#endif //OPENCV_TCPSERVER_H
