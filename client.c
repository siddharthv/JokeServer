// Simple client to retrieve a joke from the server.

#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>

// Wrapper to display error messages with error-number.
void error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

// Function to open socket and to return it.
int open_socket(char *host, char *port)
{
	struct addrinfo *res;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, port, &hints, &res) == -1)
		error("Cannot resolve the address");
	int d_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (d_sock == -1)
		error("Cannot open socket");

	int c = connect(d_sock, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	if (c == -1)
		error("Cannot connect to socket");
	
	return d_sock;
}

// Function to send information over a socket.
/*
int say(int socket, char *s)
{
	int result = send(socket, s, strlen(s), 0);
	if (result == -1)
		fprintf(stderr, "%s: %s\n", "Error talking to server", strerror(errno));
	return result;
}
*/

int main(int argc, char **argv)
{
	int d_sock;
	
	// Hardcoded to communicate with JokeServer
	d_sock = open_socket("127.0.0.1", "30000");
	char buf[255];

	char rec[256];
	int bytesRcvd = recv(d_sock, rec, 255, 0);
	while (bytesRcvd)
	{
		if (bytesRcvd == -1)
			error("Can't read from server");

		rec[bytesRcvd] = '\0';
		printf("%s", rec);
		bytesRcvd = recv(d_sock, rec, 255, 0);
	}
	close(d_sock);
	return 0;
}
