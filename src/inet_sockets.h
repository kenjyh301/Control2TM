/*
 * inet_sockets.h
 *
 *  Created on: May 11, 2014
 *      Author: Nguyen Minh Tan
 */

#ifndef INET_SOCKETS_H_
#define INET_SOCKETS_H_

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#define _BSD_SOURCE							// to get NI_MAXHOST and NI_MAXSERV definitions from <netdb.h>
#define IS_ADDR_STR_LEN			4096

int inet_connect(const char *host, const char *service, int type);
int inet_listen(const char *service, int backlog, socklen_t *addrlen);
int inet_bind(const char *service, int type, socklen_t *addrlen);
char *inet_address_str(const struct sockaddr *addr, socklen_t addrlen, char *addr_str, int addr_str_len);
void *get_in_addr(struct sockaddr *sa);
#endif /* INET_SOCKETS_H_ */
