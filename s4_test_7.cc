#include <cstdio>
#include <unistd.h>
#include "acceptor.h"
#include "eventloop.h"

void newConnection(socket_t* sock, const netaddr_t& peerAddr)
{
    printf("newConnection(): accepted a new connection from");
    write(*sock, "How are you?\n", 13);
    delete sock;
}

void newConnection1(socket_t* sock, const netaddr_t& peerAddr)
{
    printf("newConnection(): accepted a new connection from");
    write(*sock, "You are how?\n", 13);
    delete sock;
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    netaddr_t listenAddr(9981);
    netaddr_t listenAddr1(9980);
    eventloop_t loop;

    acceptor_t acceptor(&loop, listenAddr);
    acceptor.set_newconn_cb(newConnection);
    acceptor.start_listen();

    acceptor_t acceptor1(&loop, listenAddr1);
    acceptor1.set_newconn_cb(newConnection1);
    acceptor1.start_listen();

    loop.start_loop();

    return 0;
}