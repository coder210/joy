/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-sys.h
Description: ��װһЩ��ƽ̨�ӿ�
Author: ydlc
Version: 1.0
Date: 2021.3.20
History:
*************************************************/
#ifndef LIVNET_SYS_H
#define LIVNET_SYS_H
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "liv-config.h"


#if defined(LIVNET_WIN)
#	include <time.h>
#	include <winsock2.h>
#elif defined(LIVNET_LINUX)
#	include<sys/time.h>
#	include<sys/socket.h>
#	include<arpa/inet.h>
#	include<unistd.h>
#	include<errno.h>
#	include<sys/ioctl.h>
#	include<sys/fcntl.h>
#elif defined(LIVNET_MACOS)

#elif defined(LIVNET_UNIX)

#else

#endif


static bool 
sys_init_netenv(void)
{
#if defined(LIVNET_WIN)
	WSADATA data;
	int r = WSAStartup(0x0201, &data);
	return r == 0;
#elif defined(LIVNET_LINUX)
	return true;
#elif defined(LIVNET_MACOS)
	return true;
#elif defined(LIVNET_UNIX)
	return true;
#else
	return false;
#endif
}

static bool 
sys_release_netenv(void)
{
#if defined(LIVNET_WIN)
	int r;
	r = WSACleanup();
	return r != 0;
#elif defined(LIVNET_LINUX)
	return true;
#elif defined(LIVNET_MACOS)
	return true;
#elif defined(LIVNET_UNIX)
	return true;
#else
	return false;
#endif
	return false;
}

static int 
sys_anyaddr(void)
{
#if defined(LIVNET_WIN)
	int anyaddr = htonl(INADDR_ANY);
	return anyaddr;
#elif defined(LIVNET_LINUX)
	int anyaddr = (int)htonl(INADDR_ANY);
	return anyaddr;
#elif defined(LIVNET_MACOS)
	return true;
#elif defined(LIVNET_UNIX)
	return true;
#else
	return false;
#endif
}

static bool 
sys_closesocket(uint64_t sockfd)
{
#if defined(LIVNET_WIN)
	return closesocket(sockfd) == 0;
#elif defined(LIVNET_LINUX)
	return close(sockfd) == 0;
#elif defined(LIVNET_MACOS)
	return true;
#elif defined(LIVNET_UNIX)
	return true;
#else
	return false;
#endif
}

static int 
sys_set_sock_sndtimeo(uint64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(LIVNET_WIN)
	sz = (int)sizeof(int);
	r = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&ms, sz);
#elif defined(LIVNET_LINUX)
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;
	sz = (int)sizeof(tv);
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sz);
#elif defined(LIVNET_MACOS)

#elif defined(LIVNET_UNIX)

#else

#endif
	return r;
}

static int 
sys_set_sock_rcvtimeo(uint64_t sockfd, int ms)
{
	int r, sz;

	r = 0;
#if defined(LIVNET_WIN)
	sz = (int)sizeof(int);
	r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sz);
#elif defined(LIVNET_LINUX)
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	//r = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(int));
#elif defined(LIVNET_MACOS)
#elif defined(LIVNET_UNIX)
#else
#endif
	return r;
}

static uint64_t 
sys_tcp()
{
	uint64_t sockfd;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

static uint64_t 
sys_udp()
{
	uint64_t sockfd;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

static bool 
sys_bind(uint64_t sockfd, const char* ip, int port)
{
	bool resval;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(ip);
	peer.sin_port = htons(port);
	resval = bind(sockfd, (struct sockaddr*)&peer, sizeof(peer)) == 0;
	//SDL_Log("bind:%d", errno);
#else
	resval = false;
#endif
	return resval;
}

static bool 
sys_connect(uint64_t sockfd, const char* ip, int port)
{
	bool resval;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
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

static bool 
sys_listen(uint64_t sockfd)
{
	bool resval;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	resval = listen(sockfd, SOMAXCONN) == 0;
#else
	resval = false;
#endif
	return resval;
}

static uint64_t 
sys_accept(uint64_t sockfd, char* ip, int* port)
{
	uint64_t acceptfd;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	struct sockaddr_in client;
	int len = sizeof(client);
	acceptfd = accept(sockfd, (struct sockaddr*)&client, &len);
	strncpy(ip, inet_ntoa(client.sin_addr), LIVNET_MAX_IP);
	*port = ntohs(client.sin_port);
#else
	acceptfd = 0;
#endif
	return acceptfd;
}

static int 
sys_send(uint64_t sockfd, const char* buf, int len)
{
	int n;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	n = send(sockfd, buf, len, 0);
#else
	n = -1;
#endif
	return n;
}

static int 
sys_recv(uint64_t sockfd, char* buf)
{
	int n;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	n = recv(sockfd, buf, LIVNET_MAX_BUFFER, 0);
	if (n > 0) buf[n] = 0;
#else
	n = -1;
#endif
	return n;
}

static int 
sys_recvfrom(uint64_t sockfd, char* buf, char* ip, int* port)
{
	int n, len;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
	struct sockaddr_in peer;
	len = sizeof(peer);
	n = recvfrom(sockfd, buf, LIVNET_MAX_BUFFER, 0,
		(struct sockaddr*)&peer, &len);
	strcpy(ip, inet_ntoa(peer.sin_addr));
	*port = ntohs(peer.sin_port);
#else
	n = -1;
#endif
	return n;
}

static int 
sys_sendto(uint64_t sockfd, const char* buf,
	int len, const char* ip, int port)
{
	int n;
	size_t sz;
#if defined(LIVNET_WIN) || defined(LIVNET_LINUX)
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

static int64_t 
sys_current_time()
{
	int64_t current_time;
#if defined(LIVNET_WIN)
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
#elif defined(LIVNET_LINUX)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	current_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#elif defined(LIVNET_MACOS)
	current_time = 0;
#elif defined(LIVNET_UNIX)
	current_time = 0;
#else
	current_time = 0;
#endif
	return current_time;
}

static void 
sys_sleep(int ms)
{
#if defined(LIVNET_WIN)
	Sleep(ms);
#elif defined(LIVNET_LINUX)
	usleep(ms * 1000);
#elif defined(LIVNET_MACOS)
#elif defined(LIVNET_UNIX)
#else
#endif
}

static void 
sys_create_iocp(int ms)
{
#if defined(LIVNET_WIN)
	HANDLE completion_port;
	completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (completion_port == NULL) {
		return;
	}
#elif defined(LIVNET_LINUX)

#elif defined(LIVNET_MACOS)

#elif defined(LIVNET_UNIX)

#else

#endif
}

#endif // !LIVNET_SYS_H
