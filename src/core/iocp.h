#ifndef CORE_IOCP_H
#define CORE_IOCP_H
#include "config.h"

typedef struct iocp_server iocp_server_t, * iocp_server_p;

iocp_server_p iocp_server_create(const char *bind_ip, int bind_port);
void iocp_server_destroy(iocp_server_p server);
void iocp_process_events(iocp_server_p server, int timeout_ms);

#endif // !CORE_IOCP_H
