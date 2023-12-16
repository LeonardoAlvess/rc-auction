#include "../common/utils.h"
#include "sv_processes.h"
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sstream>
#include <vector>

#define PORT "58039"

using namespace std;

// call this func right after socket chosen and messaeg received
int send_verbose_msg(string uid, string req, string ip, string port)
{
    string msg = "User " + uid + " made a " + req + " request from " + ip + ":" + port;
    cout << msg;
    return 0;
}

int main(int argc, char *argv[])
{ // adicionar args e processar
    string port = PORT;
    bool verbose = false;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-p"))
        {
            i++;
            port = argv[i];
        }
        else if (!strcmp(argv[i], "-v"))
        {
            if (verbose == false)
                verbose = true;
        }
    }

    fd_set inputs, testfds;
    struct timeval timeout;
    int out_fds, errcode;
    ssize_t size;
    char buffer[BUFFER_SIZE];

    // socket variables
    struct addrinfo hints_udp, hints_tcp, *res_udp, *res_tcp;
    struct sockaddr_in udp_useraddr, tcp_useraddr;
    socklen_t addrlen;
    int ufd, tfd, newfd;

    char host[NI_MAXHOST], service[NI_MAXSERV];

    // UDP SERVER SECTION
    memset(&hints_udp, 0, sizeof(hints_udp));
    hints_udp.ai_family = AF_INET;
    hints_udp.ai_socktype = SOCK_DGRAM;
    hints_udp.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    if ((errcode = getaddrinfo(NULL, &port[0], &hints_udp, &res_udp)) != 0)
        exit(1); // On error

    if ((ufd = socket(res_udp->ai_family, res_udp->ai_socktype, res_udp->ai_protocol)) == -1)
        exit(1);

    if (bind(ufd, res_udp->ai_addr, res_udp->ai_addrlen) == -1)
        exit(1); // On error

    if (res_udp != NULL)
        freeaddrinfo(res_udp);

    // TCP SERVER SECTION
    memset(&hints_tcp, 0, sizeof(hints_tcp));
    hints_tcp.ai_family = AF_INET;
    hints_tcp.ai_socktype = SOCK_STREAM;
    hints_tcp.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    if ((errcode = getaddrinfo(NULL, &port[0], &hints_tcp, &res_tcp)) != 0)
        exit(1); // On error

    tfd = socket(res_tcp->ai_family, res_tcp->ai_socktype, res_tcp->ai_protocol);
    if (tfd == -1)
        exit(1);

    if (bind(tfd, res_tcp->ai_addr, res_tcp->ai_addrlen) == -1)
        exit(1); // On error

    if (res_tcp != NULL)
        freeaddrinfo(res_tcp);

    if (listen(tfd, 5) == -1) /*error*/
        exit(1);

    FD_ZERO(&inputs);     // Clear input mask
    FD_SET(ufd, &inputs); // Set UDP channel on
    FD_SET(tfd, &inputs); // Set TCP channel on

    //    printf("Size of fd_set: %d\n",sizeof(fd_set));
    //    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    while (1)
    {
        testfds = inputs; // Reload mask
                          //        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        memset((void *)&timeout, 0, sizeof(timeout));
        timeout.tv_sec = 10;

        out_fds = select(FD_SETSIZE, &testfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)&timeout);
        // testfds is now '1' at the positions that were activated
        //        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        switch (out_fds)
        {
        case 0:
            printf("\n ---------------Timeout event-----------------\n");
            break;
        case -1:
            perror("select");
            exit(1);
        default:
            if (FD_ISSET(ufd, &testfds))
            {
                addrlen = sizeof(udp_useraddr);
                size = recvfrom(ufd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&udp_useraddr, &addrlen);
                if (size >= 0)
                {
                    printf("---UDP socket: %s\n", buffer);
                    errcode = getnameinfo((struct sockaddr *)&udp_useraddr, addrlen, host, sizeof host, service, sizeof service, 0);
                    if (errcode == 0)
                        printf("       Sent by [%s:%s]\n", host, service);
                }
            }
            // TCP
            if (FD_ISSET(tfd, &testfds))
            {
                addrlen = sizeof(tcp_useraddr);
                if ((newfd = accept(tfd, (struct sockaddr *)&tcp_useraddr, &addrlen)) == -1) /*error*/
                    exit(1);
                memset(buffer, '\0', sizeof(buffer));
                if (((size = read(newfd, buffer, BUFFER_SIZE)) == -1)) /*error*/
                    exit(1);
                if (size >= 0)
                {
                    printf("---TCP socket: %s\n", buffer);
                    errcode = getnameinfo((struct sockaddr *)&tcp_useraddr, addrlen, host, sizeof host, service, sizeof service, 0);
                    if (errcode == 0)
                        printf("       Sent by [%s:%s]\n", host, service);
                }
            }
            string token, ip = host, port = service;
            vector<string> args;
            istringstream iss(buffer);
            while (iss >> token)
            {
                args.push_back(token); // reading from line stream and filling argument vector
            }
            // verify N args
            int code = getProtocolType(args[0]);
            string msg;
            switch (code)
            {
            case LOGIN:
                send_verbose_msg(args[1], "LIN", ip, port);
                msg = sv_login_process(args[1], args[2]);
                break;

            case LOGOUT:
                send_verbose_msg(args[1], "LOU", ip, port);
                msg = sv_logout_process(args[1], args[2]);
                break;

            case UNREGISTER:
                send_verbose_msg(args[1], "UNR", ip, port);
                msg = sv_unregister_process(args[1], args[2]);
                break;

            case MY_AUCTIONS:
                send_verbose_msg(args[1], "LMA", ip, port);
                msg = sv_myauctions_process(args[1]);
                break;

            case MY_BIDS:
                send_verbose_msg(args[1], "LMB", ip, port);
                msg = sv_mybids_process(args[1]);
                break;

            case LIST:
                send_verbose_msg("Unknown", "LST", ip, port);
                msg = sv_list_process(port, ip);
                break;

            case SHOW_RECORD:
                send_verbose_msg("Unknown", "SRC", ip, port);
                msg = sv_show_record_process(args[1]);
                break;
            case SHOW_ASSET:
                msg = sv_show_asset(buffer, port, ip, newfd, verbose);
                break;
            case OPEN_AUCTION:
                msg = sv_open_process(buffer, size, port, ip, newfd, verbose);
                break;
            case CLOSE_AUCTION:
                send_verbose_msg(args[1], "CLS", ip, port);
                msg = sv_close_process(args[1], args[2], args[3]);
                break;
            case BID:
                send_verbose_msg(args[1], "BID", ip, port);
                msg = sv_bid_process(args[1], args[2], args[3], args[4]);
                break;
            default:
                send_verbose_msg("Unknown", "Unknown", ip, port);
                if (FD_ISSET(tfd, &testfds))
                {
                    send_message_tcp(newfd, "ERR\n");
                    close(newfd);
                }
                else
                    send_message_udp(port, ip, "ERR\n"); // adaptar para tcp
                break;
            }

            if (FD_ISSET(ufd, &testfds))
            {
                if (sendto(ufd, &msg[0], msg.size(), 0, (struct sockaddr *)&udp_useraddr, addrlen) == -1) /*error*/
                    exit(1);
            }
            if (FD_ISSET(tfd, &testfds))
            {
                send_message_tcp(newfd, msg);
                close(newfd);
            }

            break;
        }
    }

    return 0;
}
