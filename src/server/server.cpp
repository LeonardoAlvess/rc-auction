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

using namespace std;

//call this func right after socket chosen and messaeg received 
int send_verbose_msg(string uid, string req, string ip, string port){
  string msg = "User "+uid+" made a "+req+" request from "+ip+":"+port;
  cout << msg;
  return 0;
}

int main(int argc, char *argv[]){ //adicionar args e processar
  string port = PORT;
  bool verbose = false;
  for(int i = 1; i < argc; i++){
    if (!strcmp(argv[i],"-p")){
      i++;
      port = argv[i];
    }
    else if (!strcmp(argv[i],"-v")){
      verbose = true; 
    }
  }
  
      
    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;

    int out_fds,errcode, ret;

    char prt_str[90];


    // socket variables
    struct addrinfo hints_udp, hints_tcp, *res_udp, *res_tcp;
    struct sockaddr_in udp_useraddr, tcp_useraddr;
    socklen_t addrlen;
    int ufd, tfd;

    char host[NI_MAXHOST], service[NI_MAXSERV];


    // UDP SERVER SECTION
    memset(&hints_udp,0,sizeof(hints_udp));
    hints_udp.ai_family=AF_INET;
    hints_udp.ai_socktype=SOCK_DGRAM;
    hints_udp.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

    if((errcode=getaddrinfo(NULL,&port[0],&hints_udp,&res_udp))!=0)
        exit(1);// On error

    ufd=socket(res_udp->ai_family,res_udp->ai_socktype,res_udp->ai_protocol);
    if(ufd==-1)
        exit(1);

    if(bind(ufd,res_udp->ai_addr,res_udp->ai_addrlen)==-1)
    {
        sprintf(prt_str,"Bind error UDP server\n");
        write(1,prt_str,strlen(prt_str));
        exit(1);// On error
    }
    if(res_udp!=NULL)
        freeaddrinfo(res_udp);

    // TCP SERVER SECTION
    memset(&hints_tcp,0,sizeof(hints_tcp));
    hints_tcp.ai_family=AF_INET;
    hints_tcp.ai_socktype=SOCK_STREAM;
    hints_tcp.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

    if((errcode=getaddrinfo(NULL,&port[0],&hints_tcp,&res_tcp))!=0)
        exit(1);// On error

    tfd=socket(res_tcp->ai_family,res_tcp->ai_socktype,res_tcp->ai_protocol);
    if(tfd==-1)
        exit(1);

    if(bind(tfd,res_tcp->ai_addr,res_tcp->ai_addrlen)==-1)
    {
        sprintf(prt_str,"Bind error tcp server\n");
        write(1,prt_str,strlen(prt_str));
        exit(1);// On error
    }
    if(res_tcp!=NULL)
        freeaddrinfo(res_tcp);  
      
    FD_ZERO(&inputs); // Clear input mask
    FD_SET(0,&inputs); // Set standard input channel on
    FD_SET(ufd,&inputs); // Set UDP channel on
    FD_SET(tfd,&inputs); // Set TCP channel on

    //    printf("Size of fd_set: %d\n",sizeof(fd_set));
    //    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    sv_login_process("190256","password","50000","localhost");
    sv_open_process("190256 password mauracio 100 1000 preacher.png 324000","50000","localhost");
    sv_unregister_process("190256","password","50000","localhost");
    sv_login_process("290256","password","50000","localhost");
    sv_bid_process("290256","password","001","300","50000","localhost");
    sv_unregister_process("290256","password","50000","localhost");

    while(1)
    {
        testfds=inputs; // Reload mask
    //        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=10;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
    // testfds is now '1' at the positions that were activated
    //        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        switch(out_fds)
        {
            case 0:
                printf("\n ---------------Timeout event-----------------\n");
                break;
            case -1:
                perror("select");
                exit(1);
            default:
                if(FD_ISSET(0,&testfds))
                {
                    fgets(in_str,50,stdin);
                    printf("---Input at keyboard: %s\n",in_str);
                }
                if(FD_ISSET(ufd,&testfds))
                {
                    addrlen = sizeof(udp_useraddr);
                    ret=recvfrom(ufd,prt_str,80,0,(struct sockaddr *)&udp_useraddr,&addrlen);
                    if(ret>=0)
                    {
                        if(strlen(prt_str)>0)
                            prt_str[ret-1]=0;
                        printf("---UDP socket: %s\n",prt_str);
                        errcode=getnameinfo( (struct sockaddr *) &udp_useraddr,addrlen,host,sizeof host, service,sizeof service,0);
                        if(errcode==0)
                            printf("       Sent by [%s:%s]\n",host,service);

                    }
                    string token, ip = host, port = service;
                    vector<string> args;      
                    istringstream iss(prt_str);
                    while(iss >> token){      
                        args.push_back(token);  //reading from line stream and filling argument vector
                    }
                    //verify N args
                    int code = getProtocolType(args[0]);

                    switch (code)
                    {
                    case LOGIN:
                        sv_login_process(args[1], args[2], port, ip);
                        break;
                    
                    case LOGOUT:
                        sv_logout_process(args[1], args[2], port, ip);
                        break;
                    
                    case UNREGISTER:
                        sv_unregister_process(args[1], args[2], port, ip);
                        break;
                    
                    case MY_AUCTIONS:
                        sv_myauctions_process(args[1], port, ip);
                        break;
                    
                    case MY_BIDS:
                        sv_mybids_process(args[1], port, ip);
                        break;
                    
                    case LIST:
                        sv_list_process(port, ip);
                        break;
                    
                    case SHOW_RECORD:
                        sv_show_record_process(args[1], port, ip);
                        break;
                    
                    default:
                        send_message_udp(port, ip, "ERR\n", SERVER_MODE);
                        break;
                    }
                    
                }
                if(FD_ISSET(tfd,&testfds))
                {
                    //tcp shit
                }
        }
    }
  
  return 0;
}