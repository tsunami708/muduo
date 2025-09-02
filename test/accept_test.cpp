#include <format>
#include <arpa/inet.h>
#include "acceptor.h"
#include "eventloop.h"

void netaddr_test();
void socket_test();
void accept_test();

int main()
{
    netaddr_test();
    socket_test();
    accept_test();
    return 0;
}


void netaddr_test()
{
    netaddr_t test_addr0(8080);
    netaddr_t test_addr1(8081, "47.98.216.65");
    const auto& sockaddr0 = test_addr0.get_socketaddr();
    const auto& sockaddr1 = test_addr1.get_socketaddr();
}

void socket_test()
{
    socket_t* test_socket = socket_t::create_socket();
    test_socket->bind_socket({8080});
    test_socket->listen_socket();
    delete test_socket;
}

void accept_test()
{
    eventloop_t test_loop;
    acceptor_t test_acceptor(&test_loop);
    test_acceptor.set_new_conn_cb([](const netaddr_t& peer_addr) {
        const auto& socket_info = peer_addr.get_socketaddr();
        char ipv4[32] = {0};
        inet_ntop(AF_INET, &socket_info.sin_addr.s_addr, ipv4, 32);
        LOG_INFO(
            std::format("peer_ipv4:{}-peer_port:{}", ipv4, ntohs(socket_info.sin_port)).c_str());
    });
    test_acceptor.start_listen({8080, "127.0.0.1"});
    test_loop.start_loop();
}