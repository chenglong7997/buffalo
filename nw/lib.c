#include "citizen.h"

//listening tcp port and udp port
char tcp_port[20] = "";
char udp_port[20] = "";
//store the real ip of this peer
char localip[20] = "";

//sock_self is  the array for storing outgoing socket
int sock_self[MAXCON] = {};
int sock_num = 0;
//con_ip is the array for storing outgoing connection ip
static char con_ip[MAXCON][20] = {};
static char port[MAXCON][20] = {};
//sock_peer is  the array for storing incoming connections socket
int peer_self[MAXCON] = {};
int peer_num = 0;
//acept_ip is the array for storing incoming ip
static char acept_ip[MAXCON][20] = {};
int pport[MAXCON] = {};

extern fd_set master;
//extern fd_set read_fds;
extern int fdmax;

//the peer_token of this peer is defined by self_token
unsigned long self_token = 0;

//to store receve msg, including private msg and brocast msg
char recpmsg[MSGNUM][MSGSIZE] = {};

//store the private msg produced by this peer
char pmsg[MSGSIZE] = {};
//store the brocast msg produced by this peer
char bmsg[MSGSIZE] = {};
//salute msg
char smsg[MSGSIZE] = {};

//peer_token of sending broadcast msg and salute msg
char peer_token[MAXCON][20] = {};

//leader ip port
unsigned char leader_ip[20] = {};
unsigned char leader_port[20] = {};
//information about init_token used by all-tokens()
char all_ip[MAXCON][20] = {};
int all_port[MAXCON] = {};
unsigned long all_token[MAXCON] = {};
int all_num = 0;

//*************************************************************************************************//

/*called at the starting of citizen to check arguments
**@argc is the number of arguments
**@argv is the pointer array of arguments
*/
void checkoption(int argc, char *argv[])
{
	int connections = 0;
	if (argc == 4)
	{
		//argv[1] is the number of clients
		if (atoi(argv[1]) > 3)
			Perror("Sorry, i cannot handle more than 3 connections");
		else if (atoi(argv[1]) < 2)
			Perror("Please input more than 1 connection");
		else
			connections = atoi(argv[1]);
		//copy port
		strcpy(tcp_port, argv[2]);
		strcpy(udp_port, argv[3]);
		printf("Listening port is tcp_port %s udp_port is %s\n", tcp_port, udp_port);
	}
	else
		Perror("Bad Argument");
}
//**************************************************************************************//
//library functions are add_fd_set(), GetPrimaryIp(), private_msg(), salute_msg(), brocast_msg()
//and connect_tcp() which are used by checkcommand() and other functions

/*
**@fd is the socket id which need to add to fd_set
*/
void add_fd_set(int fd)
{
	FD_SET(fd, &master);
	fdmax++;
}

/*
**GetPrimaryIp is used to get the real ip
**@buffer used to store the real ip of this peer
**@buflen is the length of variable buffer
*/
void GetPrimaryIp(char* buffer, size_t buflen) 
{
	assert(buflen >= 16);

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	assert(sock != -1);

	const char* kGoogleDnsIp = "8.8.8.8";
	uint16_t kDnsPort = 53;
	struct sockaddr_in serv;
	
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
	serv.sin_port = htons(kDnsPort);

	int err = connect( sock, (void *)&serv, sizeof(serv) );
	assert(err != -1);

	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	err = getsockname(sock, (struct sockaddr*)&name, &namelen);
	assert(err != -1);

	const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, buflen);
	assert(p);
	close(sock);
}

/*
**private_msg is used to create private message
**@token is the init_token which will be included in pricate msg
**@return is the private msg pointer
*/
char *private_msg(char *token)
{
	//create id
	memset(pmsg, '0', MSGSIZE);
	char msgid[9] = {'0','0','0','0','0','0','0','0','0'};
	//create a rand num as a msg id
	int idrand = rand();
	msgid[0] = (idrand>>24) & 0xFF;
	msgid[1] = (idrand>>16) & 0xFF;
	msgid[2] = (idrand>>8) & 0xFF;
	msgid[3] = (idrand) & 0xFF;
	msgid[8] = '\0';
	//create type
	char *type = "0";
	char length[3] = {0,0,0};
	//since we should add '\0' to the end , so len = 10+1
	int len = 10 + 1;
	length[0] = '0';
	length[1] = (len) & 0xFF;
	length[2] = '\0';
//	printf("in private_msg token is %s\n", token);
	strcpy(pmsg, msgid);
	strcat(pmsg, type);
	strcat(pmsg, length);
	strcat(pmsg, token);
	return pmsg;
}

/*
**salute_msg is used to create salute message
**@return is the pointer to the salute msg which i created
*/
char *salute_msg()
{
	char msgid[9] = {'0','0','0','0','0','0','0','0','0'};
	//create a rand num as a msg id
	int idrand = rand();
	msgid[0] = (idrand>>24) & 0xFF;
	msgid[1] = (idrand>>16) & 0xFF;
	msgid[2] = (idrand>>8) & 0xFF;
	msgid[3] = (idrand) & 0xFF;
	msgid[8] = '\0';
	char type = (char)16;
	char length[3] = {0,0,0};
	//since we should add '\0' to the end , so len=10+1
	int len= 47 + 1;
	length[0] = '0';
	length[1] = (len) & 0xFF;
	length[2] = '\0';
	//add all the information to smsg
	sprintf(smsg, "%s%c%s%lu%s", msgid, type, length, self_token, "ALL HAIL THE MIGHTY LEADER");
	return smsg;
}

/*
**brocast_msg is used to create brocast message
**@token is the peer_token of this peer which will be included in pricate msg
**@return is the brocast msg pointer
*/
char *brocast_msg(unsigned long token)
{
	char msgid[9] = {'0','0','0','0','0','0','0','0','0'};
	int idrand = rand();
	//produce a rand num as a msg id
	msgid[0] = (idrand>>24) & 0xFF;
	msgid[1] = (idrand>>16) & 0xFF;
	msgid[2] = (idrand>>8) & 0xFF;
	msgid[3] = (idrand) & 0xFF;
	msgid[8] = '\0';
	//create type
	char *type = "1";
	//payload
	char payload[MSGSIZE] = {};	
	//change ip format
	char str[20] = {};
	GetPrimaryIp(localip, sizeof(localip));
	strcpy(str, localip);
	
	char *point1 = NULL;
	char *point2 = NULL;	
	char *point3 = NULL;
	char *tmp = NULL;
	point3 = strrchr(str,'.');
	if (point3 != NULL)	
	{
	    tmp = point3++;
	    *tmp='\0';	
	}
	point2 = strrchr(str,'.');
	if (point2 != NULL)
	{
	    tmp=point2++;
	    *tmp='\0';	
	}
	point1 = strrchr(str,'.');
	if (point1 != NULL)
	{
	    tmp=point1++;
	    *tmp='\0';	
	}
	
	int num1 = atoi(str);
	int num2 = atoi(point1);
	int num3 = atoi(point2);
	int num4 = atoi(point3);
	unsigned char n1 = (num1) & 0xFF;
	unsigned char n2 = (num2) & 0xFF;
	unsigned char n3 = (num3) & 0xFF;
	unsigned char n4 = (num4) & 0xFF;
	int uport = atoi(udp_port);
//	printf("ip %d.%d.%d.%d\n", n1,n2,n3,n4);
	unsigned char port1 = (uport>>8) &0xFF;
	unsigned char port2 = (uport) &0xFF;
//	printf("port port1 %d port2%d\n", port1, port2);
	sprintf(payload, "%lu%c%c%c%c%c%c", token, n1, n2, n3, n4, port1, port2);
	char length[3] = {'0','0','0'};
	//since we should add '\0' to the end , so len=15+1
	int len = 15 + 1;
	length[0] = '0';
	length[1] = (len) & 0xFF;
	length[2] = '\0';
	sprintf(bmsg, "%s%s%s%s", msgid, type, length, payload);
	return bmsg;
}

/*
**connect_tcp is used to create a new tcp connection to a specific peer
**called by checkcommand() to connect a tcp connection
**@ip is the ip of peer which i should connect to
**@port is the port of peer which i should connect to
*/
int connect_tcp(char *ip, char *port)
{
	int socket_self = 0;
	//peer's sockaddr
	struct sockaddr_in pin;	
	struct hostent *s_host_info;
	char request[str_size];
	char response[str_size];
	//the len of data which are returned by server
	int len = 0;
	uint16_t Port = atoi(port);
	bzero(&pin, sizeof(pin) );
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_addr.s_addr = inet_addr(ip);
	pin.sin_port = htons(Port);

	if ((socket_self = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		Perror("failed socket()");
	if (connect(socket_self, (void *)&pin, sizeof(pin)) == -1)
		Perror("failed connnect()");
	printf("successfully connected to %s, %s\n", ip, port);
	//add this socket to fd_set
	add_fd_set(socket_self);
	return socket_self;
}

//**************************************************************************************************//
//functions about commands like checkcommand(), salute(), print_init_token(), selftoken(), brocast_token(),
//brocast_handler(), ready(), shoe-conn() and info() which are used to deal with commands 

/*
**checkcommand() is used to check user's command and call the corresponding functions
*/
void checkcommand()
{
	//str is  the string which user has input
	char str[CMDSIZE] = {};
	char cmd[CMDSIZE] = {};
	
	//index of connnection
	static int index = 0;
	if (ferror(stdin))
		printf("error stdin\n");
	else if (feof(stdin))
		printf("eof\n");
	if (fgets(str, CMDSIZE, stdin) != NULL)
	{
		if (sscanf(str, "%s %*s", cmd) == 0)
			printf("sscanf\n");
		if (strcmp(cmd, "connect") == 0)
		{
			if (sscanf(str, "%s %s %s\n", cmd, con_ip[index], port[index]) != 3)
				Perror("sscanf");
			//call connect_tcp to create connection
			sock_self[index] = connect_tcp(con_ip[index], port[index]);
//			printf("index is %d\n", index);
			index++;
			sock_num = index;
		}
		else if (strcmp(cmd, "info") == 0)
			info();
		else if (strcmp(cmd, "show-conn") == 0)
		{
			if (index == 0)
				printf("No outgoing TCP connections at this peer\n");
			else
			{
				int j = 0;			
				for(; j < index; j++)
					show_conn(j, sock_self[j], con_ip[j], port[j]);
			}
		}
		else if (strcmp(cmd, "ready") == 0)
			ready(con_ip);
		else if (strcmp(cmd, "self-token") == 0)
			selftoken();
		else if (strcmp(cmd, "exit") == 0)
		{
			//close all the sockets which are related to this peer
			int i = 0;
			for (; i < sock_num; i++)
				close(sock_self[i]);
			i = 0;
			for (; i < peer_num; i++)
				close(peer_self[i]);
			exit(0);
		}
		else if (strcmp(cmd, "salute") == 0)
			salute();
		else if (strcmp(cmd, "all-tokens") == 0)
			all_tokens();
		else
			printf("UNKNOWN COMMAND\n");
	}
}

/*
**show all the init_tokens received by this peer
*/
void all_tokens()
{
	int index = 0;
	printf("IP | remote port | token \n");
	printf("-----------------------------------------------\n");
	for (; index < all_num; index++)
	{
		printf("%s | %d | %lu\n", all_ip[index], all_port[index], all_token[index]);
	}
}

/*
**salute() send a saule msg to leader
*/
void salute()
{
	char ip[20] = {};
	sprintf(ip, "%u.%u.%u.%u", leader_ip[0], leader_ip[1], leader_ip[2], leader_ip[3]);
	printf("%s is the leader\n", ip);
	int port = 0;
	port = (int)(leader_port[0] << 8) + (int)(leader_port[1]);
//	printf("port %d\n", port);
	//call saulte_msg to create a salute msg and use udp to send this msg
	udp_send(ip, port, salute_msg());
}

/*
**print_init_token is used to print init_token when this peer receive a private msg
**@socket_fd is the socket which the pricate msg comes from
**@msg is the private msg which is used to find the init_token
*/
void print_init_token(int socket_fd, char *msg)
{
	//search sock_self and peer_self to find this socket and 
	//then find the corresponding ip and port
	int i = 0;
	char ip[20] = {};
	int intport = 0;
	for (; i < peer_num; i++)
	{
		if (peer_self[i] == socket_fd)
		{
			strcpy(ip, acept_ip[i]);
			intport = pport[i];
		}
	}
	i = 0;
	for (; i < sock_num; i++)
	{
		if (sock_self[i] == socket_fd)
		{
			strcpy(ip, con_ip[i]);
			intport=atoi(port[i]);
		}
	}
	//find init_token in this msg
//	printf("in print init msg %s, size is %d\n", msg, strlen(msg));
	
	unsigned long tmp = 0;
	int j = 0;
	for(; j < 10; j++)
		tmp = (unsigned long)((unsigned long)tmp * 10 + msg[j + 11] - '0');
	printf("Ip %s port %d send init_token %lu to me\n", ip, intport, tmp);
	static int index = 0;
	strcpy(all_ip[index], ip);
	all_port[index] = intport;
	all_token[index] = tmp;
	index++;
	all_num = index;
}

/*
**self-token is used to decide peer's peer_token and brocast this token
*/
void selftoken()
{
	int num = 0, i = 0, j = 0;
	unsigned long tmp = 0;
	self_token = 0;
	while (strcmp(recpmsg[num], "") != 0)
		num++;
//	printf("num %d peer_num %d sock_num %d\n", num, peer_num, sock_num);
	//i have not receive all the init_tokens
	if (num < peer_num + sock_num)
		printf("WAITING ON PEER_TOKEN\n");
	else
	{
		for (; i < peer_num+sock_num; i++)
		{	
			j = 0;
			for (; j<10; j++)
				tmp = tmp * 10 + recpmsg[i][j + 11] - '0';
			if (self_token < tmp)
				self_token = tmp;
			tmp = 0;
		}
		printf("My peer_token S is %lu\n", self_token);
		//create brocast_msg
		brocast_msg(self_token);
		//brocast this msg
		brocast_token(bmsg);
	}
}

/*
**brocast msg
**@msg is the pointer to brocast msg
*/
void brocast_token(char *msg)
{
	int i = 0;
//	printf("sock_num %d, peer_num %d\n", sock_num, peer_num);
	//send to all the peers which connects to this peer
	for(; i < sock_num; i++)
		send_msg(sock_self[i], bmsg);
	i = sock_num;
	int j = 0;
	for(; i < peer_num + sock_num; i++,j++)
	{	
		if (peer_self[j] > 0)
			send_msg(peer_self[j], bmsg);
	}
}

/*
**used to handle the received brocast msg
**@index is the index of brocast msg
*/
void brocast_handler(int index)
{
	//find if this msg is an old msg
	int start = peer_num + sock_num;
	int i = start;
	int flag = 0;
	for (; i < index; i++)
	{
		if (strcmp(recpmsg[index], recpmsg[i]) == 0 || (strcmp(recpmsg[index], bmsg) == 0))
			flag = 1;
	}
	if (flag == 0)	//new brocast msg, store the peer_token and forward that msg
	{
		int j = 0;
		for(; j < 10; j++)
			peer_token[index-start][j]=recpmsg[index][j+11];
		printf("Receive a broadcast msg from peer_token is %s\n", peer_token[index-start]);
//		printf("Brocast msg %s\n", recpmsg[index]);
		brocast_token(recpmsg[index]);
		//save leader's ip and port
		j = start;		
		for (; j < index; j++)
		{
			if (strcmp(peer_token[index-start], peer_token[j]) < 0)
				break;	
		}
		if (j == index)	//peer_token[index-start] is the largest among all the token
		{
			int k = 0;
			for (; k < 4; k++)
				leader_ip[k] = recpmsg[index][k+21];
			leader_port[0] = recpmsg[index][25];
			leader_port[1] = recpmsg[index][26];
		}
	}
	//old brocast msg ,discard that
}

/*
**ready is called when user input a ready command.It asks user to input init_token for every connections
**and create the corresponding private msg and send msgs to corresponding peer
**@num is the total num of socketed which this peer works like a client.It equals to sock_self
**@con_ip is a 2-dimensional array which stores ip
*/
void ready(char con_ip[][20])
{
	int num = sock_num;
	static char init_token[MAXCON][TOKENLEN] = {};
	//there are MAXCON connections, so pmsg are totally MAXCON
	static char pmsg[MAXCON][MSGSIZE] = {};
	//for peers which this peer connects to
	printf("Please input init_token for each peer\n");			
	
	int j = 0;
	for (j; j < num; j++)
	{
		printf("for peers which this peer connects to\n");
		printf("Ip: %s  :", con_ip[j]);
		scanf("%s", init_token[j]);
//		printf("strlen token is %d\n", strlen(init_token[j]));
		if (strlen((char *)(init_token[j])) != 10)
		{
			printf("Input error, Please input a 10-digit number\n");
			strcpy(init_token[j], "");
			--j;
		}
		else
			printf("Init_token is %s\n", init_token[j]);
	}
	//for peers which are accepted by this peer
	int k = num;
	j = 0;
	for (; j < peer_num; j++,k++)
	{
		printf("for peers which are accepted by this peer\n");
		printf("Ip: %s  :", acept_ip[j]);
		scanf("%s", init_token[k]);
//		printf("strlen token is %d\n", strlen(init_token[k]));
		if (strlen((char *)(init_token[k])) != 10)
		{
			printf("Input error, Please input a 10-digit number\n");
			strcpy(init_token[k], "");
			--j;
		}
		else
			printf("Init_token is %s\n", init_token[k]);
	}
	
	//create the private_msg
	j = 0;
	for (j; j < num; j++)
		strcpy( pmsg[j], private_msg(init_token[j]) );
	k = num;
	for (k; k < peer_num+num; k++)
		strcpy( pmsg[k], private_msg(init_token[k]) );
	
	//send this msg call the fd return by connect_tcp to send these tokens.
	int i = 0;
	for (; i < num; i++)
	{
		send_msg(sock_self[i], pmsg[i]);
	}
	i = num;
	j = 0;
	for (; i < peer_num+num; i++,j++)
	{
		if (peer_self[j] > 0)
			send_msg(peer_self[j], pmsg[i]);
	}
}

/*
**show connections
**@index of showing information
**@sock_self is the socket of outgoing TCP connections
**@ip is the ip of corresponding ip
**@port is the port of corresponding ip
*/
void show_conn(int index, int sock_self, char *ip, char *port)
{
//	printf("sock_self is %d\n", sock_self);
	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	int err = getsockname(sock_self, (struct sockaddr*)&name, &namelen);
	assert(err != -1);
	u_short local_port = 0;
	local_port = htons((int)&name.sin_port);

	char hostname[HOSTNAME] = "";
	char service[20] = "";
	struct sockaddr_in pin;	//peer's sockaddr
	uint16_t Port = atoi(port);
	bzero(&pin, sizeof(pin) );
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_addr.s_addr = inet_addr(ip);
	pin.sin_port = htons(Port);

	getnameinfo(&pin, sizeof(pin), hostname, sizeof(hostname), service, sizeof(service), 0);
//	printf("service is %s\n", service);
	printf("cnnID | IP | hostname | local port | remote port\n");
	printf("------------------------------------------------\n");
	printf("%d | %s | %s | %d | %s\n", index+1, ip, hostname, local_port, port);
	index++;
}

/*
**info used to show information of this peer
*/
void info()
{
	char hostname[HOSTNAME] = "";
	//get the host name of this peer
	if (gethostname(hostname, sizeof(hostname)) == -1)
		Perror("gethostname");
	hostname[HOSTNAME-1] = '\0';
	//get the real ip of this peer
	GetPrimaryIp(localip, sizeof(localip));
	printf("IP address | hostname | TCP port | UDP port\n");
	printf("----------------------------------------------\n");
	printf("%s | %s | %s | %s \n", localip, hostname, tcp_port, udp_port);
}

//**************************************************************************************************//
//socket related functions like setup(), tcp_handler(), udp_handler(), init_tcp(), init_udp() functions for socket part

/*
**setup is called at the starting of citizen is used to call init_tcp
**and init_udp to listen connections
**@tcp_fd is used to store the reture value return by init_tcp.That is the listen socket
**@tudp_fd is used to store the reture value return by udp_tcp.
*/
void setup(int *tcp_fd, int *udp_fd)
{
	*tcp_fd = init_tcp();
	*udp_fd = init_udp();
}

/*
**udp_send is used to send msg like salute msg via udp connections
**@ip is the ip of remote peer
**@port is the udp port of remote peer
**@msg is the message which will be sent
*/
void udp_send(char *ip, int port, char *msg)
{
	int s = 0;
//	printf("In send ip %s, port %d\n", ip, port);
	struct sockaddr_in addr;
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		Perror("fail socket()");
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	if (sendto(s, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)
		Perror("fail sendto");
	
}

/*
**send_msg is used to send tcp msg like private and broadcast msg
**@socket_fd is the socket which will be used to send msg
**@msg is the msg which will be send
*/
void send_msg(int socket_fd, char *msg)
{
	if (send(socket_fd, msg, strlen(msg) + 1, 0) == -1)
		Perror("Error in send()");
}

/*
**recv_msg is used to receive tcp msg like private and broadcast msg
**via udp and tcp connections
**@socket_fd is the socket which msg is received from
*/
void recv_msg(int socket_fd)
{
	char msg[MSGSIZE] = {};
	int i = 0;
	static int index = 0;
	if ((i = recv(socket_fd, msg, MSGSIZE, 0)) == -1)
		Perror("fail rcv()");
	else if (i == 0)
	{
		printf("peer has close connection\n");
		FD_CLR(socket_fd, &master);
		close(socket_fd);	
	}
	else
	{
//		printf("msg is %s\n", msg);		
		//if index < sock_num+peer_num, that is a private msg
		if (index < (sock_num+peer_num))
		{
			strcpy(recpmsg[index], msg);
			print_init_token(socket_fd, msg);
		}
		//if index >= sock_num+peer_num, that is a brocast msg
		else
		{
			strcpy(recpmsg[index], msg);
			brocast_handler(index);
		}
		index++;
	}
}

/*
**tcp_handler is used to accept remote tcp connections.And 
**store the corresponding ip and port.
**@tcp_fd is the listend socket of tcp
*/
void tcp_handler(int tcp_fd)
{
	static int index = 0;
	struct sockaddr_in pin;		//peer's sockaddr
	char msg[MSGSIZE] = "";
	char *ip;
	int cport;
	ssize_t recvs;
	int sock_main, tcp_connfd;
	unsigned int add_size = sizeof(struct sockaddr);
	
	tcp_connfd = accept(tcp_fd, (struct sockaddr*)&pin, &add_size);
	ip = inet_ntoa(pin.sin_addr);
	cport = (pin.sin_port);
	if (tcp_connfd == -1)
		Perror("fail accept tcp!");
	else
		printf("successfully connected from %s, %d\n", ip, cport);
	add_fd_set(tcp_connfd);
	if (index == MAXCON)
		Perror("connections too many, cannot create this connection!");
	//store ip and port
	strcpy(acept_ip[index], ip);
	pport[index] = cport;
	peer_self[index++] = tcp_connfd;
	peer_num++;
}

/*
**udp_handler is used to receive msg and handle the msg
**@udp_fd is the udp socket which used to receive msg
*/
void udp_handler(int udp_fd)
{
	int n = 0;
	char peer_tk[20] = {};
	char msg[MSGSIZE] = {};
	struct sockaddr_in remote_address;
	socklen_t len;
	len = sizeof(remote_address);
	if ((n = recvfrom(udp_fd, msg, sizeof(msg), 0, (struct sockaddr *)&remote_address, &(len))) < 0)
		Perror("recvfrom failed!");
	//show peer's peer_token
	int i = 0;	
	for (; i < 10; i++)
		peer_tk[i] = msg[i + 11];
	printf("Peer token NO.%s send one salute msg to me\n", peer_tk);
}

/*
**init_udp is used to create a udp socket and bind local ip and port
*/
int init_udp()
{
	struct sockaddr_in local_address;
	int udp_fd;
	if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		Perror("socket() failed!");
	memset( (void*)&local_address, 0, sizeof(local_address) );
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = htonl(INADDR_ANY);
	local_address.sin_port = htons(atoi(udp_port));
	if (bind(udp_fd, (struct sockaddr *)&local_address, sizeof(local_address)) < 0)
		Perror("bind() failed!");
	return udp_fd;
}

/*
**init_tcp is used to create a tcp listening socket
*/
int init_tcp()
{	
	struct sockaddr_in sin;		//tcp connection's sockaddr
	int sock_main, add_size;
	int opt = 1;
	//create the socket
	if ((sock_main = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		Perror("fail socket");
	if (setsockopt(sock_main, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))	
		Perror("setsockopt");
	bzero(&sin, sizeof(sin));
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	//check which port we should use
	sin.sin_port = htons(atoi(tcp_port));
	//bind addr to socket
	if (bind(sock_main, (struct sockaddr*)&sin, sizeof(sin) ) == -1)
		Perror("fail bind");
	//listen on this socket
	if ((listen(sock_main, 10)) == -1)
		Perror("failed listen");
	return sock_main;
}

/*
**Perror error hand function
*/
void Perror(char *msg)
{
	perror(msg);
	exit(1);
}
