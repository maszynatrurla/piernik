#ifndef LWIP_HTTPSERVER_NETCONN_H
#define LWIP_HTTPSERVER_NETCONN_H

#include <stdbool.h>

void http_server_netconn_init(void);

bool http_server_cycle(void);

void http_server_netconn_deinit(void);

#endif /* LWIP_HTTPSERVER_NETCONN_H */
