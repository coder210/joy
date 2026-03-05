#ifndef IOCP_H
#define IOCP_H
#include "config.h"

typedef struct iocp_server iocp_server_t, * iocp_server_p;

#ifdef __cplusplus
extern "C" {
#endif

	JOY_API iocp_server_p iocp_server_create(const char *bind_ip, int bind_port);
	JOY_API void iocp_server_destroy(iocp_server_p server);
	JOY_API void iocp_process_events(iocp_server_p server, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // !IOCP_H
