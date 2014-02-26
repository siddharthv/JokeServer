#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sqlite3.h>
#include<sys/socket.h>
#include<arpa/inet.h>

// GLOBAL DS to hold joke
char msg[1000];

static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for(i=0; i<argc; i++){
	printf("Random Joke of the Day(DEBUG): %s\n", argv[i] ? argv[i] : "NULL");
    }
    	strcpy(msg, argv[0]);
	printf("\n");
    return 0;
}

void get_joke ()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *query;
    const char* data = "Callback function called";

    rc = sqlite3_open("cj_6_1.sqlite", &db);

    if( rc ){
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    else{
        fprintf(stderr, "Opened DB successfully\n");
    }
    query = "select text from jokes order by RANDOM() limit 1";

    rc = sqlite3_exec(db, query, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL Error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
}
//--------------
void error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

int open_listener_socket()
{
	int s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
		error("Cannot open socket");

	return s;
}

void bind_to_port(int socket, int port)
{
	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = (in_port_t)htons(30000);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	int reuse = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
		error("Cannot set the reuse option on the socket");

	int c = bind (socket, (struct sockaddr *) &name, sizeof(name));
	if (c == -1)
		error("Cannot bind to socket");
}

int say(int socket, char *s)
{
	int result = send(socket, s, strlen(s), 0);
	if (result == -1)
		fprintf(stderr, "%s: %s\n", "Error talking to the client", strerror(errno));

	return result;
}

int listener_d;

void handle_shutdown(int sig)
{
	if (listener_d)
		close(listener_d);

	fprintf(stderr, "Bye!\n");
	exit(0);
}

int main(int argc, char **argv)
{
	listener_d = open_listener_socket();
	bind_to_port(listener_d, 30000);
	if (listen(listener_d, 10) == -1)
		error("Cannot listen");

	struct sockaddr_storage client_addr;
	unsigned int address_size = sizeof(client_addr);
	puts("Waiting for connection\n");
	char buf[500];
	while(1)
	{
		int connect_d = accept(listener_d, (struct sockaddr *) &client_addr, &address_size);
		if (connect_d == -1)
			error("Cannot open secondary socket");

		char *greeting = "Fetching Joke!\n";
		send(connect_d, greeting, strlen(greeting), 0);
		
		get_joke();

		send(connect_d, msg, strlen(msg), 0);
		
		close(connect_d);
	}
return 0;
}
