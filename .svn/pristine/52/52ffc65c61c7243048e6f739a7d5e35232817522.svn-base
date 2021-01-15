/*
 * inet_sockets.c
 *
 *  Created on: May 11, 2014
 *      Author: Nguyen Minh Tan
 */

#include "inet_sockets.h"

int inet_connect(const char *host, const char *service, int type) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;		// allows both IPv4 and IPv6
	hints.ai_socktype = type;

	s = getaddrinfo(host, service, &hints, &result);
	if (s != 0) {
		errno = ENOSYS;
		return -1;
	}

	/* walk through returned list until we find an address structure that can be used to successfully connect a socket */
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1) {
			continue;			// on error, try next address
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		// connect failed, close this socket and try the next address
		close(sfd);
	}
	return (rp == NULL) ? -1 : sfd;
}

static int inet_passive_socket (const char *service, int type, socklen_t *addrlen, int do_listen, int backlog) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd = -1, optval, s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_socktype = type;
	hints.ai_family = AF_UNSPEC;		// allows IPv4 and IPv6
	hints.ai_flags = AI_PASSIVE;		// use wildcard IP address

	s = getaddrinfo(NULL, service, &hints, &result);
	if (s != 0)
		return -1;
	/* walk through returned list until we find an address structure that can be used to successfully create and bind a socket */
	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;					// on error, try next address
		if (do_listen) {
			if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
				close(sfd);
				freeaddrinfo(result);
				return -1;
			}
		}
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		// bind failed, close this socket and try next address
		close(sfd);
	}

	if (rp != NULL && do_listen) {
		if (listen(sfd, backlog) == -1) {
			freeaddrinfo(result);
			return -1;
		}
	}

	if (rp != NULL && addrlen != NULL)
		*addrlen = rp->ai_addrlen;
	freeaddrinfo(result);
	return (rp == NULL) ? -1 : sfd;
}

int inet_listen(const char *service, int backlog, socklen_t *addrlen) {
	return inet_passive_socket(service, SOCK_STREAM, addrlen, 1, backlog);
}

int inet_bind(const char *service, int type, socklen_t *addrlen) {
	return inet_passive_socket(service, type, addrlen, 0, 0);
}

char *inet_addr_str(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen) {
	char host[NI_MAXHOST], service[NI_MAXSERV];
	if (getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) == 0)
		snprintf(addrStr, addrStrLen, "(%s, %s)", host, service);
	else
		snprintf(addrStr, addrStrLen, "(?UNKNOWN?)");

	addrStr[addrStrLen - 1] = '\0';
	return addrStr;
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
