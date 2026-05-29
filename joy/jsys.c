#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsys.h"


#if defined(JOY_WIN)
#	include <time.h>
#	include <WinSock2.h>
#	include <mswsock.h>
#	include <Windows.h>
#	include <ws2tcpip.h>
#       include <DbgHelp.h>
#	pragma comment(lib, "ws2_32.lib")
#       pragma comment(lib, "DbgHelp.lib")



static LONG WINAPI crashHandler(EXCEPTION_POINTERS* ep) 
{
        char dumpPath[MAX_PATH];
        SYSTEMTIME st;
        GetLocalTime(&st);
        snprintf(dumpPath, sizeof(dumpPath), "crash_%04d%02d%02d_%02d%02d%02d.dmp",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        HANDLE hFile = CreateFileA(dumpPath, GENERIC_WRITE, 0, NULL,
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
                MINIDUMP_EXCEPTION_INFORMATION ei = { GetCurrentThreadId(), ep, FALSE };
                MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                        hFile, MiniDumpWithDataSegs, &ei, NULL, NULL);
                CloseHandle(hFile);
        }
        return EXCEPTION_CONTINUE_SEARCH;
}


#elif defined(JOY_LINUX)
#	include<unistd.h>
#	include<sys/time.h>
#	include<sys/socket.h>
#	include<arpa/inet.h>
#	include<errno.h>
#	include<sys/ioctl.h>
#	include<sys/fcntl.h>
#       include<signal.h>
#       include<execinfo.h>
#       include <fcntl.h>
#       include <time.h>


static void crashHandler(int sig) 
{
        // 打印信号信息
        char msg[128];
        snprintf(msg, sizeof(msg), "\n======== CRASH ======== signal=%d (%s)\n",
                sig, sig == SIGSEGV ? "SIGSEGV" : sig == SIGABRT ? "SIGABRT" : "other");
        write(STDERR_FILENO, msg, strlen(msg));

        // 采集调用堆栈
        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);

        // 写入日志文件
        time_t t = time(nullptr);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
                backtrace_symbols_fd(buffer, frames, fd);
                close(fd);
        }

        // 恢复默认信号处理并重新触发，让系统生成 core dump
        signal(sig, SIG_DFL);
        raise(sig);
}

#elif defined(JOY_MACOS)
#   include <unistd.h>
#   include <sys/time.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <errno.h>
#   include <sys/ioctl.h>
#   include <sys/fcntl.h>
#   include <signal.h>
#   include <execinfo.h>
#   include <fcntl.h>
#   include <time.h>

static void crashHandler(int sig)
{
        char msg[128];
        snprintf(msg, sizeof(msg), "\n======== CRASH ======== signal=%d (%s)\n",
                sig, sig == SIGSEGV ? "SIGSEGV" : sig == SIGABRT ? "SIGABRT" : "other");
        write(STDERR_FILENO, msg, strlen(msg));

        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);

        time_t t = time(nullptr);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
                backtrace_symbols_fd(buffer, frames, fd);
                close(fd);
        }

        signal(sig, SIG_DFL);
        raise(sig);
}

#elif defined(JOY_UNIX)
// 适用于 FreeBSD, Solaris 等支持 execinfo.h 的 Unix 系统
#   include <unistd.h>
#   include <sys/time.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <errno.h>
#   include <sys/ioctl.h>
#   include <sys/fcntl.h>
#   include <signal.h>
#   include <execinfo.h>
#   include <fcntl.h>
#   include <time.h>

static void crashHandler(int sig)
{
        char msg[128];
        snprintf(msg, sizeof(msg), "\n======== CRASH ======== signal=%d (%s)\n",
                sig, sig == SIGSEGV ? "SIGSEGV" : sig == SIGABRT ? "SIGABRT" : "other");
        write(STDERR_FILENO, msg, strlen(msg));

        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);

        time_t t = time(nullptr);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
                backtrace_symbols_fd(buffer, frames, fd);
                close(fd);
        }

        signal(sig, SIG_DFL);
        raise(sig);
}
#else

static void crashHandler(int sig)
{
       // unkonwn 
}

#endif


bool sys_init_netenv(void)
{
#if defined(JOY_WIN)
	WSADATA data;
	int r = WSAStartup(0x0201, &data);
	return r == 0;
#elif defined(JOY_LINUX)
	return true;
#elif defined(JOY_MACOS)
	return true;
#elif defined(JOY_UNIX)
	return true;
#else
	return false;
#endif
}

bool sys_release_netenv(void)
{
#if defined(JOY_WIN)
	int r;
	r = WSACleanup();
	return r != 0;
#elif defined(JOY_LINUX)
	return true;
#elif defined(JOY_MACOS)
	return true;
#elif defined(JOY_UNIX)
	return true;
#else
	return false;
#endif
	return false;
}

int sys_anyaddr(void)
{
#if defined(JOY_WIN)
	int anyaddr = htonl(INADDR_ANY);
	return anyaddr;
#elif defined(JOY_LINUX)
	int anyaddr = (int)htonl(INADDR_ANY);
	return anyaddr;
#elif defined(JOY_MACOS)
	return true;
#elif defined(JOY_UNIX)
	return true;
#else
	return false;
#endif
}

bool sys_closesocket(int64_t sockfd)
{
#if defined(JOY_WIN)
	return closesocket(sockfd) == 0;
#elif defined(JOY_LINUX)
	return close(sockfd) == 0;
#elif defined(JOY_MACOS)
	return true;
#elif defined(JOY_UNIX)
	return true;
#else
	return false;
#endif
}

int sys_set_sock_sndtimeo(int64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(JOY_WIN)
	sz = (int)sizeof(int);
	r = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&ms, sz);
#elif defined(JOY_LINUX)
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;
	sz = (int)sizeof(tv);
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sz);
#elif defined(JOY_MACOS)

#elif defined(JOY_UNIX)

#else

#endif
	return r;
}

int sys_set_sock_rcvtimeo(int64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(JOY_WIN)
	sz = (int)sizeof(int);
	r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sz);
#elif defined(JOY_LINUX)
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(int));
#elif defined(JOY_MACOS)
#elif defined(JOY_UNIX)
#else
#endif
	return r;
}

int sys_set_sock_accpettimeo(int64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(JOY_WIN)
	sz = (int)sizeof(int);
        r = ioctlsocket(sockfd, FIONBIO, (u_long*)&ms);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sz);
#elif defined(JOY_LINUX)
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(int));
#elif defined(JOY_MACOS)
#elif defined(JOY_UNIX)
#else
#endif
	return r;
}

int64_t sys_tcp()
{
	int64_t sockfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

int64_t sys_udp()
{
	int64_t sockfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

bool sys_bind(int64_t sockfd, const char* ip, int port)
{
	bool resval;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(ip);
	peer.sin_port = htons(port);
	resval = bind(sockfd, (struct sockaddr*)&peer, sizeof(peer)) == 0;
#else
	resval = false;
#endif
	return resval;
}

bool sys_connect(int64_t sockfd, const char* ip, int port)
{
	bool resval;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(ip);
	peer.sin_port = htons(port);
	resval = connect(sockfd, (struct sockaddr*)&peer, sizeof(peer)) == 0;
#else
	resval = false;
#endif
	return resval;
}

bool sys_listen(int64_t sockfd)
{
	bool resval;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	resval = listen(sockfd, SOMAXCONN) == 0;
#else
	resval = false;
#endif
	return resval;
}

int64_t sys_accept(int64_t sockfd, char* ip, int* port)
{
	int64_t acceptfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in client;
	int len = sizeof(client);
	acceptfd = accept(sockfd, (struct sockaddr*)&client, &len);
	strncpy(ip, inet_ntoa(client.sin_addr), JOY_MAX_IP);
	*port = ntohs(client.sin_port);
#else
	acceptfd = 0;
#endif
	return acceptfd;
}

int sys_send(int64_t sockfd, const char* buf, int len)
{
	int n;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	n = send(sockfd, buf, len, 0);
#else
	n = -1;
#endif
	return n;
}

int sys_recv(int64_t sockfd, char* buf)
{
	int n;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	n = recv(sockfd, buf, JOY_MAX_BUFFER, 0);
	if (n > 0) buf[n] = 0;
#else
	n = -1;
#endif
	return n;
}

int sys_recvfrom(int64_t sockfd, char* buf, char* ip, int* port)
{
	int n, len;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	len = sizeof(peer);
	n = recvfrom(sockfd, buf, JOY_MAX_BUFFER, 0,
		(struct sockaddr*)&peer, &len);
	strcpy(ip, inet_ntoa(peer.sin_addr));
	*port = ntohs(peer.sin_port);
#else
	n = -1;
#endif
	return n;
}

int sys_sendto(int64_t sockfd, const char* buf, int len, 
	const char* ip, int port)
{
	int n;
	size_t sz;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(ip);
	peer.sin_port = htons(port);
	sz = sizeof(peer);
	n = sendto(sockfd, buf, len, 0, (struct sockaddr*)&peer, sz);
#else
	n = -1;
#endif	
	return n;
}

int64_t sys_current_time()
{
	int64_t current_time;
#if defined(JOY_WIN)
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	int64_t sec = mktime(&tm);
	current_time = sec * 1000 + wtm.wMilliseconds;
#elif defined(JOY_LINUX)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	current_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#elif defined(JOY_MACOS)
	current_time = 0;
#elif defined(JOY_UNIX)
	current_time = 0;
#else
	current_time = 0;
#endif
	return current_time;
}

void sys_sleep(int ms)
{
#if defined(JOY_WIN)
	Sleep(ms);
#elif defined(JOY_LINUX)
	usleep(ms * 1000);
#elif defined(JOY_MACOS)
#elif defined(JOY_UNIX)
#else
#endif
}


void sys_install_crash_handlers()
{
#if defined(JOY_WIN)
        SetUnhandledExceptionFilter(crashHandler);
#elif defined(JOY_LINUX)
        signal(SIGSEGV, crashHandler);
        signal(SIGABRT, crashHandler);
        signal(SIGFPE, crashHandler);
        signal(SIGILL, crashHandler);
#elif defined(JOY_MACOS)
        signal(SIGSEGV, crashHandler);
        signal(SIGABRT, crashHandler);
        signal(SIGFPE, crashHandler);
        signal(SIGILL, crashHandler);
#elif defined(JOY_UNIX)
        signal(SIGSEGV, crashHandler);
        signal(SIGABRT, crashHandler);
        signal(SIGFPE, crashHandler);
        signal(SIGILL, crashHandler);
#else
	printf("unknown platform, crash handlers not installed");
#endif
}


#ifdef JOY_WIN

// 客户端会话结构
typedef struct iocp_client_session
{
        char client_ip[JOY_MAX_IP];
        int client_port;
        SOCKET client_socket;
        int heartbeat_count;          // 心跳计数器
        bool is_handshaked;           // 是否已完成握手
        bool is_active;               // 会话是否活跃
        // unsigned char send_buffer[MAX_SEND_PKG];
} iocp_client_session_t, * iocp_client_session_ptr;

// IOCP操作包结构
typedef struct iocp_operation_packet {
        WSAOVERLAPPED overlapped;
        WSABUF wsabuffer;
        int operation_type;           // 操作类型
        SOCKET target_socket;         // 目标套接字
        int bytes_received;           // 已接收字节数
        unsigned char* long_buffer;   // 长包缓冲区
        int max_buffer_length;        // 缓冲区最大长度
        unsigned char buffer[JOY_MAX_BUFFER + 1];
        int direction;                // 0:接收, 1:发送
        iocp_client_session_ptr associated_session; // 关联的客户端会话
} iocp_operation_packet_t, * iocp_operation_packet_ptr;

// IOCP服务器实例结构
typedef struct iocp_server
{
        HANDLE completion_port;               // IOCP完成端口句柄
        SOCKET listen_socket;                 // 监听套接字
        enum packet_format {
                PACKET_FORMAT_TCP_BINARY = 0, // TCP二进制封包
                PACKET_FORMAT_TCP_JSON,       // TCP JSON封包
                PACKET_FORMAT_WEBSOCKET_JSON  // WebSocket JSON封包
        } packet_format;
        int heartbeat_interval;               // 心跳检测间隔(秒)
        enum iocp_operation {
                IOCP_OPERATION_ACCEPT = 0,
                IOCP_OPERATION_RECEIVE,
                IOCP_OPERATION_SEND,
        } iocp_operation_type;
        LPFN_ACCEPTEX acceptex_function;
        LPFN_GETACCEPTEXSOCKADDRS get_acceptex_sockaddrs_function;
        bool is_running;                      // 服务器运行状态
        CRITICAL_SECTION critical_section;    // 线程同步临界区

} iocp_server_t, * iocp_server_p;

// 函数声明
static void iocp_post_accept_operation(iocp_server_p server, iocp_operation_packet_ptr packet);
static int iocp_post_receive_operation(iocp_client_session_ptr session);
static void iocp_handle_accept_completion(iocp_server_p server, iocp_operation_packet_ptr packet);
static void iocp_handle_receive_completion(iocp_operation_packet_ptr packet, DWORD bytes_transferred, iocp_client_session_ptr session);
static void iocp_cleanup_client_session(iocp_client_session_ptr session);

// 清理客户端会话资源
static void iocp_cleanup_client_session(iocp_client_session_ptr session)
{
        if (!session) return;

        if (session->client_socket != INVALID_SOCKET) {
                closesocket(session->client_socket);
                session->client_socket = INVALID_SOCKET;
        }

        free(session);
}

// 投递AcceptEx操作
static void iocp_post_accept_operation(iocp_server_p server, iocp_operation_packet_ptr packet)
{
        if (!server || !packet) return;

        packet->wsabuffer.buf = (CHAR*)packet->buffer;
        packet->wsabuffer.len = JOY_MAX_BUFFER;
        packet->operation_type = IOCP_OPERATION_ACCEPT;

        // 创建用于AcceptEx的客户端套接字
        packet->target_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                NULL, 0, WSA_FLAG_OVERLAPPED);
        if (packet->target_socket == INVALID_SOCKET) {
                //printf("WSASocket for client failed: %d", WSAGetLastError());
                return;
        }

        DWORD bytes_received = 0;
        int address_size = (sizeof(struct sockaddr_in) + 16);

        // 使用AcceptEx异步接受连接
        if (!server->acceptex_function(server->listen_socket,
                packet->target_socket,
                packet->wsabuffer.buf, 0,
                address_size, address_size,
                &bytes_received, &packet->overlapped)) {
                if (WSAGetLastError() != ERROR_IO_PENDING) {
                        //printf("AcceptEx operation failed: %d", WSAGetLastError());
                        closesocket(packet->target_socket);
                }
        }
}

// 投递接收数据操作
static int iocp_post_receive_operation(iocp_client_session_ptr session)
{
        if (!session || session->client_socket == INVALID_SOCKET) return -1;

        iocp_operation_packet_ptr packet = (iocp_operation_packet_ptr)calloc(1, sizeof(iocp_operation_packet_t));
        if (!packet) return -1;

        packet->operation_type = IOCP_OPERATION_RECEIVE;
        packet->wsabuffer.buf = (CHAR*)packet->buffer;
        packet->wsabuffer.len = JOY_MAX_BUFFER;
        packet->max_buffer_length = JOY_MAX_BUFFER;
        packet->associated_session = session;
        packet->target_socket = session->client_socket;

        DWORD bytes_received = 0;
        DWORD flags = 0;

        int result = WSARecv(session->client_socket, &(packet->wsabuffer),
                1, &bytes_received, &flags, &(packet->overlapped), NULL);

        if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                //printf("WSARecv operation failed: %d", WSAGetLastError());
                free(packet);
                return -1;
        }

        return 0;
}

// 处理接收完成事件
static void iocp_handle_receive_completion(iocp_operation_packet_ptr packet, DWORD bytes_transferred, iocp_client_session_ptr session)
{
        if (!packet || !session) return;

        packet->bytes_received += bytes_transferred;

        // 处理接收到的数据
        if (bytes_transferred > 0) {
                //printf("Received %lu bytes from client %s:%d",
                 //       bytes_transferred, session->client_ip, session->client_port);

                // 示例：回显数据
                WSABUF wsa_buffer;
                wsa_buffer.buf = packet->buffer;
                wsa_buffer.len = bytes_transferred;

                iocp_operation_packet_ptr send_packet = (iocp_operation_packet_ptr)calloc(1, sizeof(iocp_operation_packet_t));
                if (send_packet) {
                        send_packet->operation_type = IOCP_OPERATION_SEND;
                        send_packet->wsabuffer = wsa_buffer;
                        send_packet->associated_session = session;
                        send_packet->target_socket = session->client_socket;

                        DWORD bytes_sent = 0;
                        if (WSASend(session->client_socket,
                                &send_packet->wsabuffer,
                                1, &bytes_sent,
                                0, &send_packet->overlapped, NULL) == SOCKET_ERROR) {
                                if (WSAGetLastError() != WSA_IO_PENDING) {
                                        printf("WSASend operation failed: %d", WSAGetLastError());
                                        free(send_packet);
                                }
                        }
                }
        }

        // 继续接收数据
        if (session->is_active) {
                iocp_post_receive_operation(session);
        }

        // 释放操作包
        free(packet);
}

// 处理Accept完成事件
static void iocp_handle_accept_completion(iocp_server_p server, iocp_operation_packet_ptr packet)
{
        if (!server || !packet) return;

        SOCKET client_socket = packet->target_socket;
        int address_size = (sizeof(struct sockaddr_in) + 16);
        struct sockaddr_in* local_address = NULL;
        int local_address_length = sizeof(struct sockaddr_in);
        struct sockaddr_in* remote_address = NULL;
        int remote_address_length = sizeof(struct sockaddr_in);

        // 获取客户端地址信息
        server->get_acceptex_sockaddrs_function(packet->wsabuffer.buf,
                0,
                address_size, address_size,
                (struct sockaddr**)&local_address, &local_address_length,
                (struct sockaddr**)&remote_address, &remote_address_length);

        // 创建客户端会话
        iocp_client_session_ptr session = (iocp_client_session_ptr)calloc(1, sizeof(iocp_client_session_t));
        if (!session) {
                closesocket(client_socket);
                return;
        }

        session->client_socket = client_socket;
        session->is_active = true;
        session->heartbeat_count = 5; // 初始化心跳计数器

        // 获取客户端IP和端口
        char client_ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &remote_address->sin_addr, client_ip_address, sizeof(client_ip_address));
        snprintf(session->client_ip, sizeof(session->client_ip), "%s", client_ip_address);
        session->client_port = ntohs(remote_address->sin_port);

        printf("New client connected from %s:%d", session->client_ip, session->client_port);

        // 将客户端套接字关联到完成端口
        if (CreateIoCompletionPort((HANDLE)client_socket,
                server->completion_port,
                (ULONG_PTR)session, 0) == NULL) {
                printf("Failed to associate client socket with completion port: %d", GetLastError());
                iocp_cleanup_client_session(session);
                return;
        }

        // 设置套接字上下文
        setsockopt(client_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                (char*)&server->listen_socket, sizeof(server->listen_socket));

        // 开始接收客户端数据
        if (iocp_post_receive_operation(session) != 0) {
                iocp_cleanup_client_session(session);
                return;
        }

        // 重新投递Accept操作，准备接受下一个连接
        iocp_operation_packet_ptr new_packet = (iocp_operation_packet_ptr)calloc(1, sizeof(iocp_operation_packet_t));
        if (new_packet) {
                iocp_post_accept_operation(server, new_packet);
        }

        // 释放当前Accept操作包
        free(packet);
}

// 创建IOCP服务器
iocp_server_p iocp_server_create(const char* ip, int port)
{
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
                printf("WSAStartup initialization failed: %d", WSAGetLastError());
                return NULL;
        }

        DWORD bytes_returned = 0;
        GUID guid_acceptex = WSAID_ACCEPTEX;
        GUID guid_get_acceptex_sockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

        iocp_server_p server = (iocp_server_p)calloc(1, sizeof(iocp_server_t));
        if (!server) {
                return NULL;
        }

        InitializeCriticalSection(&server->critical_section);
        server->is_running = true;

        // 创建IOCP完成端口
        server->completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                NULL, 0, 0);
        if (server->completion_port == NULL) {
                printf("Failed to create completion port: %d", GetLastError());
                goto error_cleanup;
        }

        // 创建监听套接字
        server->listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
                WSA_FLAG_OVERLAPPED);
        if (server->listen_socket == INVALID_SOCKET) {
                printf("Failed to create listen socket: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 设置端口复用选项
        int reuse_address = 1;
        setsockopt(server->listen_socket, SOL_SOCKET, SO_REUSEADDR,
                (char*)&reuse_address, sizeof(reuse_address));

        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr(ip);
        server_address.sin_port = htons(port);

        if (bind(server->listen_socket, (struct sockaddr*)&server_address,
                sizeof(server_address)) != 0) {
                printf("Failed to bind socket: %d", WSAGetLastError());
                goto error_cleanup;
        }

        if (listen(server->listen_socket, SOMAXCONN) != 0) {
                printf("Failed to listen on socket: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 获取AcceptEx扩展函数
        if (WSAIoctl(server->listen_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guid_acceptex, sizeof(guid_acceptex),
                &server->acceptex_function, sizeof(server->acceptex_function),
                &bytes_returned, NULL, NULL) != 0) {
                printf("Failed to get AcceptEx function: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 获取GetAcceptExSockaddrs扩展函数
        if (WSAIoctl(server->listen_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guid_get_acceptex_sockaddrs,
                sizeof(guid_get_acceptex_sockaddrs),
                &server->get_acceptex_sockaddrs_function,
                sizeof(server->get_acceptex_sockaddrs_function),
                &bytes_returned, NULL, NULL) != 0) {
                printf("Failed to get GetAcceptExSockaddrs function: %d", WSAGetLastError());
                goto error_cleanup;
        }

        // 将监听套接字关联到完成端口
        if (CreateIoCompletionPort((HANDLE)server->listen_socket,
                server->completion_port,
                (ULONG_PTR)server, 0) == NULL) {
                printf("Failed to associate listen socket with completion port: %d",
                        GetLastError());
                goto error_cleanup;
        }

        // 投递初始的Accept操作
        iocp_operation_packet_ptr packet = (iocp_operation_packet_ptr)calloc(1, sizeof(iocp_operation_packet_t));
        if (packet) {
                iocp_post_accept_operation(server, packet);
        }

        printf("IOCP server created successfully on %s:%d", ip, port);
        return server;

error_cleanup:
        iocp_server_destroy(server);
        return NULL;
}

// 销毁IOCP服务器
void iocp_server_destroy(iocp_server_p server)
{
        if (!server) return;

        EnterCriticalSection(&server->critical_section);
        server->is_running = false;
        LeaveCriticalSection(&server->critical_section);

        if (server->listen_socket != INVALID_SOCKET) {
                closesocket(server->listen_socket);
                server->listen_socket = INVALID_SOCKET;
        }

        if (server->completion_port != NULL) {
                CloseHandle(server->completion_port);
                server->completion_port = NULL;
        }

        DeleteCriticalSection(&server->critical_section);
        free(server);

        WSACleanup();
}

// 处理IOCP事件
void iocp_process_events(iocp_server_p server, int timeout_ms)
{
        if (!server || !server->is_running) return;

        DWORD bytes_transferred = 0;
        ULONG_PTR completion_key = 0;
        iocp_operation_packet_ptr operation_packet = NULL;

        BOOL result = GetQueuedCompletionStatus(server->completion_port,
                &bytes_transferred,
                &completion_key,
                (LPOVERLAPPED*)&operation_packet,
                timeout_ms);

        if (!result) {
                DWORD error_code = GetLastError();
                if (error_code != WAIT_TIMEOUT) {
                        printf("GetQueuedCompletionStatus failed with error: %d", error_code);
                }
                return;
        }

        // 检查是否为取消操作
        if (bytes_transferred == 0 && completion_key == 0 && operation_packet == NULL) {
                return;
        }

        if (completion_key == (ULONG_PTR)server) {
                // 监听套接字事件
                if (operation_packet && operation_packet->operation_type == IOCP_OPERATION_ACCEPT) {
                        iocp_handle_accept_completion(server, operation_packet);
                }
        }
        else {
                // 客户端套接字事件
                iocp_client_session_ptr session = (iocp_client_session_ptr)completion_key;

                if (!session || !session->is_active) {
                        if (operation_packet) free(operation_packet);
                        return;
                }

                // 客户端断开连接
                if (bytes_transferred == 0 && operation_packet && operation_packet->operation_type == IOCP_OPERATION_RECEIVE) {
                        printf("Client disconnected: %s:%d", session->client_ip, session->client_port);
                        iocp_cleanup_client_session(session);
                        free(operation_packet);
                        return;
                }

                // 处理不同类型的操作
                if (operation_packet) {
                        switch (operation_packet->operation_type) {
                        case IOCP_OPERATION_RECEIVE:
                                iocp_handle_receive_completion(operation_packet, bytes_transferred, session);
                                break;
                        case IOCP_OPERATION_SEND:
                                // 发送完成处理
                                free(operation_packet);
                                break;
                        default:
                                free(operation_packet);
                                break;
                        }
                }
        }
}

#else

// 非Windows平台的空实现
iocp_server_p iocp_server_create(const char* bind_ip, int bind_port)
{
        printf("IOCP is only supported on Windows platform");
        return NULL;
}

void iocp_server_destroy(iocp_server_p server)
{
        (void)server;
}

void iocp_process_events(iocp_server_p server, int timeout_ms)
{
        (void)server;
        (void)timeout_ms;
}

#endif // JOY_WIN
