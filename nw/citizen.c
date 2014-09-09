#include "citizen.h"

fd_set master;
fd_set read_fds;
int fdmax;
extern peer_self[MAXCON];	//store peer's sockets,these peers are who connects to this peer.This peer works as a server
extern peer_num;		//store peer's total num
extern sock_self[MAXCON];	//store sockets which used by this peer to connect to other peers.This peer works as a client
extern sock_num;		//store total num of sock_self

int main(int argc, char *argv[])
{
	int tcp_fd = 0;
	int udp_fd = 0;
	//check options like tcp_port, udp_port and max connections
	checkoption(argc, argv);
	printf("\n");
	printf("=============================================================\n");
	printf("Welcome to use p2p!\n");
	printf("-------------------------------------------------------------\n");
	printf("\n");

	//setup the tcp and udp listen connection
	setup(&tcp_fd, &udp_fd);
	//handle select
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	//add the tcp_fd to master set
	FD_SET(tcp_fd, &master);
	FD_SET(udp_fd, &master);
	FD_SET(0, &master);
	fdmax = tcp_fd > udp_fd ? tcp_fd : udp_fd;

	//main loop
	while(1)
	{
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) < 0)
		{
			/*if(errno==EINTR)
				continue;
			else*/
				Perror("select!");
		}
		//some peer connect to me at tcp port, so i will accept it			
		if (FD_ISSET(tcp_fd, &read_fds))
			tcp_handler(tcp_fd);
		//some peer send pkt to me at udp port, so i will receive it
		if (FD_ISSET(udp_fd, &read_fds))
			udp_handler(udp_fd);
		//read command from stdin
		if (FD_ISSET(0, &read_fds))
			checkcommand();
		//receive msg from socket in peer_self, call recv_msg to receive it.
		int i = 0;	
		for (; i < peer_num; i++)
		{
			if (FD_ISSET(peer_self[i], &read_fds))
				recv_msg(peer_self[i]);
		}
		i = 0;
		//receive msg from socket in sock_self, call recv_msg to receive it.
		for (; i < sock_num; i++)
		{	
			if (FD_ISSET(sock_self[i], &read_fds))
				recv_msg(sock_self[i]);
		}
	}
}

