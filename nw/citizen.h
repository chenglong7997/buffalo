#ifndef CITIZEN_H
#define CITIZEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#define TOKENLEN 20
//define the MSGSIZE used by private, broadcast and salute msg
#define MSGSIZE 2000
#define CMDSIZE 1000
#define HOSTNAME 200
#define MSGNUM 300
//used by sock_self array and ip array and port array to show connections
#define MAXCON 3
#define LOCAL_HOST 127.0.0.1
//used for request and resopnse string
#define str_size 1000


/*called at the starting of citizen to check arguments
**@argc is the number of arguments
**@argv is the pointer array of arguments
*/
void checkoption(int argc, char *argv[]);

/*
**@fd is the socket id which need to add to fd_set
*/
void add_fd_set(int fd);

/*
**GetPrimaryIp is used to get the real ip
**@buffer used to store the real ip of this CITIZEN
**@buflen is the length of variable buffer
*/
void GetPrimaryIp(char* buffer, size_t buflen) ;

/*
**private_msg is used to create private message
**@token is the init_token which will be included in pricate msg
**@return is the private msg pointer
*/
char *private_msg(char *token);

/*
**show all the init_tokens received by this peer
*/
void all_tokens();

/*
**salute_msg is used to create salute message
**@return is the pointer to the salute msg which i created
*/
char *salute_msg();

/*
**brocast_msg is used to create brocast message
**@token is the CITIZEN_token of this CITIZEN which will be included in pricate msg
**@return is the brocast msg pointer
*/
char *brocast_msg(unsigned long token);

/*
**connect_tcp is used to create a new tcp connection to a specific CITIZEN
**called by checkcommand() to connect a tcp connection
**@ip is the ip of CITIZEN which i should connect to
**@port is the port of CITIZEN which i should connect to
*/
int connect_tcp(char *ip, char *port);

/*
**checkcommand() is used to check user's command and call the corresponding functions
*/
void checkcommand();

/*
**salute() send a saule msg to leader
*/
void salute();

/*
**print_init_token is used to print init_token when this CITIZEN receive a private msg
**@socket_fd is the socket which the pricate msg comes from
**@msg is the private msg which is used to find the init_token
*/
void print_init_token(int socket_fd, char *msg);

/*
**self-token is used to decide CITIZEN's CITIZEN_token and brocast this token
*/
void selftoken();

/*
**brocast msg
**@msg is the pointer to brocast msg
*/
void brocast_token(char *msg);

/*
**used to handle the received brocast msg
**@index is the index of brocast msg
*/
void brocast_handler(int index);

/*
**ready is called when user input a ready command.It asks user to input init_token for every connections
**and create the corresponding private msg and send msgs to corresponding CITIZEN
**@num is the total num of socketed which this CITIZEN works like a client.It equals to sock_self
**@con_ip is a 2-dimensional array which stores ip
*/
void ready(char con_ip[][20]);

/*
**show connections
**@index of showing information
**@sock_self is the socket of outgoing TCP connections
**@ip is the ip of corresponding ip
**@port is the port of corresponding ip
*/
void show_conn(int index, int sock_self, char *ip, char *port);

/*
**info used to show information of this CITIZEN
*/
void info();

/*
**setup is called at the starting of citizen is used to call init_tcp
**and init_udp to listen connections
**@tcp_fd is used to store the reture value return by init_tcp.That is the listen socket
**@tudp_fd is used to store the reture value return by udp_tcp.
*/
void setup(int *tcp_fd, int *udp_fd);

/*
**udp_send is used to send msg like salute msg via udp connections
**@ip is the ip of remote CITIZEN
**@port is the udp port of remote CITIZEN
**@msg is the message which will be sent
*/
void udp_send(char *ip, int port, char *msg);

/*
**send_msg is used to send tcp msg like private and broadcast msg
**@socket_fd is the socket which will be used to send msg
**@msg is the msg which will be send
*/
void send_msg(int socket_fd, char *msg);

/*
**recv_msg is used to receive tcp msg like private and broadcast msg
**via udp and tcp connections
**@socket_fd is the socket which msg is received from
*/
void recv_msg(int socket_fd);

/*
**tcp_handler is used to accept remote tcp connections.And 
**store the corresponding ip and port.
**@tcp_fd is the listend socket of tcp
*/
void tcp_handler(int tcp_fd);

/*
**udp_handler is used to receive msg and handle the msg
**@udp_fd is the udp socket which used to receive msg
*/
void udp_handler(int udp_fd);

/*
**init_udp is used to create a udp socket and bind local ip and port
*/
int init_udp();

/*
**init_tcp is used to create a tcp listening socket
*/
int init_tcp();

/*
**Perror error hand function
*/
void Perror(char *msg);

#endif

