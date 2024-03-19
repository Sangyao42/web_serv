#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};

*/

/**
 *        int getaddrinfo(const char *restrict node,
                       const char *restrict service,
                       const struct addrinfo *restrict hints,
                       struct addrinfo **restrict res);
*/
int main(void)
{
	struct addrinfo hints, *res;
	int status;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	//hints.ai_flags = AI_CANONNAME;

	if ((status = getaddrinfo(NULL, "443", &hints, &res)) != 0) {
   		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    	exit(1);
	}
	struct addrinfo *p;
	p = res;
	char ipstr[INET6_ADDRSTRLEN];
	while (p != NULL)
	{
		void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = (char *)"IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = (char *)"IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver, ipstr);
		//printf("canonname: %s\n", p->ai_canonname);
		p = p->ai_next;
	}
	struct addrinfo *p_v4, *p_v6;
	int sockfd_v4, sockfd_v6;
	p_v4 = res;
	p_v6 = res->ai_next;
	sockfd_v4 = socket(p_v4->ai_family, p_v4->ai_socktype, p_v4->ai_protocol);
	if (sockfd_v4 == -1) {
		perror("socket_v4");
	}

	sockfd_v6 = socket(p_v6->ai_family, p_v6->ai_socktype, p_v6->ai_protocol);
	if (sockfd_v6 == -1) {
		perror("socket_v6");
	}
	// bind the socket to the port we passed in to getaddrinfo():
	if (bind(sockfd_v4, p_v4->ai_addr, p_v4->ai_addrlen) == -1) {
		perror("bind_v4");
	}
	if (bind(sockfd_v6, p_v6->ai_addr, p_v6->ai_addrlen) == -1) {
		// bind_v6: Address already in use when binding to ipv6 address
		perror("bind_v6");
	}

	freeaddrinfo(res); // free the linked list
}
