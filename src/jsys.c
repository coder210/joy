/******************************************************************************
 * src/jsys.c —— sys 域唯一实现文件（一个域一个 .c，配 jsys.h）
 * 职责：跨平台 OS 原语 —— socket 封装 / 时间 / 崩溃处理与栈回溯 / 网卡枚举
 * 拆分自：src/jnet.c 的 jsys.c 分片（2026-11-15）。该分片与网络协议段
 *         零私有符号共享、依赖单向（net 调 sys），故整段搬出；IOCP 服务器
 *         属于网络层，留在 jnet.c。
 * 说明：本域公开 API 声明在配套头 src/jsys.h，由 include/joy/japi.h 聚合包含；
 *       平台条件编译与私有宏（JOY_HAVE_BACKTRACE / JOY_HAVE_IFADDRS）在文件末尾
 *       #undef，不外泄到其它翻译单元。
 ******************************************************************************/

#include "jsys.h"

/* ==================== 拆分自 src/jsys.c ==================== */
#include "joy.h"
#include <string.h>

#if defined(__ANDROID__)
#  include <unistd.h>
#  include <net/if.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#  include <arpa/inet.h>
#endif

/* backtrace() 可用性（crashHandler 用它采集调用栈）：
 *  - Linux / macOS / Unix(glic 系) 的 <execinfo.h> 提供 backtrace / backtrace_symbols_fd；
 *  - Android(bionic) 直到 API 33 才在 <execinfo.h> 公开这两个函数，本项目按 API 21 构建，
 *    因此在 Android 上跳过栈回溯（仍然打印信号信息），否则 C99 下会被判为隐式函数声明。 */
#if defined(__ANDROID__)
#  ifndef __ANDROID_API__
#    define JOY_HAVE_BACKTRACE 0
#  elif __ANDROID_API__ >= 33
#    define JOY_HAVE_BACKTRACE 1
#  else
#    define JOY_HAVE_BACKTRACE 0
#  endif
#else
#  define JOY_HAVE_BACKTRACE 1
#endif

#if defined(JOY_WIN)
#	include <time.h>
#	include <WinSock2.h>
#	include <mswsock.h>
#	include <Windows.h>
#	include <ws2tcpip.h>
#       include <DbgHelp.h>
#	include <Iphlpapi.h>
#	pragma comment(lib, "ws2_32.lib")
#       pragma comment(lib, "DbgHelp.lib")
#	pragma comment(lib, "Iphlpapi.lib")



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
#	include<netdb.h>        // getaddrinfo / struct addrinfo / freeaddrinfo（安卓 bionic 不会自动包含）
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
#if JOY_HAVE_BACKTRACE
        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
#endif

        // 写入日志文件
        time_t t = time(NULL);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
#if JOY_HAVE_BACKTRACE
                backtrace_symbols_fd(buffer, frames, fd);
#endif
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

#if JOY_HAVE_BACKTRACE
        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
#endif

        time_t t = time(NULL);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
#if JOY_HAVE_BACKTRACE
                backtrace_symbols_fd(buffer, frames, fd);
#endif
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

#if JOY_HAVE_BACKTRACE
        void* buffer[128];
        int frames = backtrace(buffer, 128);
        backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
#endif

        time_t t = time(NULL);
        struct tm* lt = localtime(&t);
        char logPath[256];
        snprintf(logPath, sizeof(logPath), "crash_%04d%02d%02d_%02d%02d%02d.log",
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec);

        int fd = open(logPath, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
                write(fd, msg, strlen(msg));
#if JOY_HAVE_BACKTRACE
                backtrace_symbols_fd(buffer, frames, fd);
#endif
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

// 统一为非 Windows（POSIX-like，含 Emscripten / Web）补充网络相关头文件。
// 各平台分支只包含了其中一部分；Emscripten 走上面的 #else（不含任何头文件），
// 导致 getaddrinfo / inet_ntop / gethostname 等未声明，Web CI 编译失败。
// 这里集中补齐，重复包含（含 Linux/macOS/UNIX 已包含的）无害（头文件有 include guard）。
#if !defined(JOY_WIN)
#	include <unistd.h>        // gethostname
#	include <sys/socket.h>    // AF_INET / SOCK_STREAM
#	include <arpa/inet.h>     // inet_ntop
#	include <netdb.h>         // getaddrinfo / struct addrinfo / freeaddrinfo
#endif

bool joy_sys_init_netenv(void)
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

bool joy_sys_release_netenv(void)
{
#if defined(JOY_WIN)
	int r;
	r = WSACleanup();
	return r == 0;   /* WSACleanup 成功返回 0, 失败返回 SOCKET_ERROR */
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

int joy_sys_anyaddr(void)
{
	/* INADDR_ANY 恒为 0, 网络字节序下仍是 0。原来 macOS/UNIX 分支返回
	 * true(1), 会把绑定地址变成 1.0.0.0 导致 bind 失败。 */
	return 0;
}

bool joy_sys_closesocket(int64_t sockfd)
{
#if defined(JOY_WIN)
	return closesocket((SOCKET)sockfd) == 0;
#elif defined(JOY_LINUX) || defined(JOY_MACOS) || defined(JOY_UNIX)
	return close((int)sockfd) == 0;
#else
	return false;
#endif
}

int joy_sys_set_sock_sndtimeo(int64_t sockfd, int ms)
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

int joy_sys_set_sock_rcvtimeo(int64_t sockfd, int ms)
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

int joy_sys_set_sock_accpettimeo(int64_t sockfd, int ms)
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

int joy_sys_set_sock_broadcast(int64_t sockfd)
{
	int r;
#if defined(JOY_WIN)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&on, (int)sizeof(on));
#elif defined(JOY_LINUX)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, (socklen_t)sizeof(on));
#else
	r = -1;
#endif
	return r;
}

int joy_sys_set_sock_reuseaddr(int64_t sockfd)
{
	int r;
#if defined(JOY_WIN)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, (int)sizeof(on));
#elif defined(JOY_LINUX)
	int on = 1;
	r = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, (socklen_t)sizeof(on));
#else
	r = -1;
#endif
	return r;
}

int64_t joy_sys_tcp()
{
	int64_t sockfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

int64_t joy_sys_udp()
{
	int64_t sockfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#else
	sockfd = 0;
#endif
	return sockfd;
}

bool joy_sys_bind(int64_t sockfd, const char* ip, int port)
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

bool joy_sys_connect(int64_t sockfd, const char* ip, int port)
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

bool joy_sys_listen(int64_t sockfd)
{
	bool resval;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	resval = listen(sockfd, SOMAXCONN) == 0;
#else
	resval = false;
#endif
	return resval;
}

int64_t joy_sys_accept(int64_t sockfd, char* ip, int* port)
{
	int64_t acceptfd;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in client;
#if defined(JOY_WIN)
	int len = (int)sizeof(client);
#else
	socklen_t len = sizeof(client);
#endif
	acceptfd = accept(sockfd, (struct sockaddr*)&client, &len);
	strncpy(ip, inet_ntoa(client.sin_addr), JOY_MAX_IP);
	*port = ntohs(client.sin_port);
#else
	acceptfd = 0;
#endif
	return acceptfd;
}

int joy_sys_send(int64_t sockfd, const char* buf, int len)
{
	int n;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	n = send(sockfd, buf, len, 0);
#else
	n = -1;
#endif
	return n;
}

int joy_sys_recv(int64_t sockfd, char* buf)
{
	int n;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	/* 调用方的 buf 是 char[JOY_MAX_BUFFER], 留 1 字节给结尾的 '\0' */
	n = recv(sockfd, buf, JOY_MAX_BUFFER - 1, 0);
	if (n > 0) buf[n] = 0;
#else
	n = -1;
#endif
	return n;
}

int joy_sys_recvfrom(int64_t sockfd, char* buf, char* ip, int* port)
{
	int n;
#if defined(JOY_WIN)
	int len;
#else
	socklen_t len;
#endif
	if (ip) ip[0] = '\0';
	if (port) *port = 0;
#if defined(JOY_WIN) || defined(JOY_LINUX)
	struct sockaddr_in peer;
	len = sizeof(peer);
	n = recvfrom(sockfd, buf, JOY_MAX_BUFFER, 0,
		(struct sockaddr*)&peer, &len);
	/* 只有成功收到数据时 peer 才被填充, 失败时不能读未初始化内容 */
	if (n >= 0 && ip && port) {
		strcpy(ip, inet_ntoa(peer.sin_addr));
		*port = ntohs(peer.sin_port);
	}
#else
	n = -1;
#endif
	return n;
}

int joy_sys_sendto(int64_t sockfd, const char* buf, int len,
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

int64_t joy_sys_current_time()
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

void joy_sys_sleep(int ms)
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


void joy_sys_install_crash_handlers()
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



/* 枚举网卡在 Android 上需要 API >= 24；更低版本退回到"主机名解析"兜底。
 * （Android NDK 的 getifaddrs 从 API 24 起可用。） */
#if defined(__ANDROID__)
#  if defined(__ANDROID_API__) && __ANDROID_API__ >= 24
#    define JOY_HAVE_IFADDRS 1
#  endif
#elif defined(__EMSCRIPTEN__)
// 浏览器内没有真实网卡，getifaddrs 既未实现（链接会报 undefined symbol）也没意义，
// 直接走下面的"本机名解析"兜底即可。
#elif !defined(JOY_WIN)
#  define JOY_HAVE_IFADDRS 1
#endif

#if defined(JOY_HAVE_IFADDRS)
#  include <ifaddrs.h>
#  include <net/if.h>
#endif

/* 只收 IPv4 —— 调用方（如服务器 IP 下拉列表）一般只要 IPv4，
 * 混进 fe80:: 之类的链路本地 IPv6 会干扰选择。
 *
 * 原来的实现有两个问题：
 *   1) 只有 Windows 分支，其它平台直接 return 0（安卓上列表是空的）；
 *   2) AF_UNSPEC 会把 IPv6 一起收进来。
 */
int joy_sys_get_local_ips(char ips[][JOY_MAX_IP], int max_count)
{
        if (!ips || max_count <= 0) return 0;

        // 浏览器内没有真实网卡/本地 IP，原始 socket 与 bind 也不可用；
        // Web 联机走 WebSocket/WebRTC 到外部服务器，故此处直接返回 0。
        // （getifaddrs 在 Emscripten 也未实现，会在链接期报错，这里一并规避。）
#if defined(__EMSCRIPTEN__)
        return 0;
#endif

        int count = 0;

#if defined(__ANDROID__) && !defined(JOY_HAVE_IFADDRS)
        /* Android 在 NDK 平台 < 24 时没有 getifaddrs，且 gethostname 兜底只能拿到
         * 127.0.0.1。这里用 /proc/net/dev 枚举接口 + ioctl(SIOCGIFADDR) 取 IPv4，
         * 无需权限、不限 API 等级，从而能拿到 wlan0 的 192.168.x.x 等局域网地址。 */
#ifndef SIOCGIFADDR
#  define SIOCGIFADDR 0x8915
#endif
        FILE* f = fopen("/proc/net/dev", "r");
        if (f) {
                char line[256];
                int sock = socket(AF_INET, SOCK_DGRAM, 0);
                while (count < max_count && fgets(line, sizeof(line), f)) {
                        char* p = line;
                        while (*p == ' ' || *p == '\t') p++;
                        if (*p == '\0' || *p == '|' || *p == '\n') continue; /* 表头 */
                        char* colon = strchr(p, ':');
                        if (!colon) continue;
                        *colon = '\0';
                        char name[IFNAMSIZ];
                        strncpy(name, p, IFNAMSIZ - 1);
                        name[IFNAMSIZ - 1] = '\0';
                        if (strncmp(name, "lo", 2) == 0) continue; /* 跳过回环 */
                        if (sock >= 0) {
                                struct ifreq ifr;
                                memset(&ifr, 0, sizeof(ifr));
                                strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);
                                if (ioctl(sock, SIOCGIFADDR, &ifr) == 0) {
                                        struct sockaddr_in* sin = (struct sockaddr_in*)&ifr.ifr_addr;
                                        if (inet_ntop(AF_INET, &sin->sin_addr, ips[count], JOY_MAX_IP))
                                                count++;
                                }
                        }
                }
                if (sock >= 0) close(sock);
                fclose(f);
        }
        if (count > 0) return count;
#endif

#if defined(JOY_WIN)
        /* Windows 无 getifaddrs：用 IP Helper 枚举网卡单播 IPv4 地址。
         * 下面的 gethostname+getaddrinfo 兜底只会把本机名解析成回环 127.0.0.1，
         * 拿不到真实局域网 IP，所以 Windows 必须走这条路径。 */
        ULONG abufLen = 0;
        DWORD adr = GetAdaptersAddresses(AF_INET,
                        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                        NULL, NULL, &abufLen);
        if (adr == ERROR_BUFFER_OVERFLOW && abufLen > 0) {
                PIP_ADAPTER_ADDRESSES addrs = (PIP_ADAPTER_ADDRESSES)malloc(abufLen);
                if (addrs) {
                        adr = GetAdaptersAddresses(AF_INET,
                                        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                        NULL, addrs, &abufLen);
                        if (adr == NO_ERROR) {
                                for (PIP_ADAPTER_ADDRESSES a = addrs; a && count < max_count; a = a->Next) {
                                        for (PIP_ADAPTER_UNICAST_ADDRESS u = a->FirstUnicastAddress;
                                             u && count < max_count; u = u->Next) {
                                                if (u->Address.lpSockaddr->sa_family != AF_INET) continue;
                                                void* addr = &((struct sockaddr_in*)u->Address.lpSockaddr)->sin_addr;
                                                if (inet_ntop(AF_INET, addr, ips[count], JOY_MAX_IP))
                                                        count++;
                                        }
                                }
                        }
                        free(addrs);
                }
        }
        if (count > 0) return count;
#endif

#if defined(JOY_HAVE_IFADDRS)
        /* POSIX / Android：直接枚举网卡。比"解析本机名"可靠得多 ——
         * 安卓上 gethostname 往往拿不到主机名，解析不出局域网 IP。 */
        struct ifaddrs* ifa = NULL;
        if (getifaddrs(&ifa) == 0) {
                struct ifaddrs* p;
                for (p = ifa; p && count < max_count; p = p->ifa_next) {
                        if (!p->ifa_addr || p->ifa_addr->sa_family != AF_INET) continue;
                        void* addr = &((struct sockaddr_in*)p->ifa_addr)->sin_addr;
                        if (inet_ntop(AF_INET, addr, ips[count], JOY_MAX_IP))
                                count++;
                }
                freeifaddrs(ifa);
        }
        if (count > 0) return count;
#endif

        /* Windows，或上面没取到时的兜底：用本机名解析 */
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) != 0) return 0;
        struct addrinfo hints, *res, *rp;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;              /* 只要 IPv4 */
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(hostname, NULL, &hints, &res) != 0) return 0;
        for (rp = res; rp && count < max_count; rp = rp->ai_next) {
                if (rp->ai_family != AF_INET) continue;
                void* addr = &((struct sockaddr_in*)rp->ai_addr)->sin_addr;
                if (inet_ntop(AF_INET, addr, ips[count], JOY_MAX_IP))
                        count++;
        }
        freeaddrinfo(res);
        return count;
}

/* 恢复「独立 TU」语义：本分片的私有宏不外泄到同域后续分片 */
#undef JOY_HAVE_BACKTRACE
#undef JOY_HAVE_IFADDRS
#undef SIOCGIFADDR

/* ==================== 结束 src/jsys.c ==================== */
