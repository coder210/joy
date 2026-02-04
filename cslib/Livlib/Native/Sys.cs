using System.Runtime.CompilerServices;

namespace Livlib.Native
{
    public struct Sys
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool InitNetEnv();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ReleaseNetEnv();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static long Anyaddr();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool CloseSocket(long sockfd);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int SetSockSndtimeo(long sockfd, int ms);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int SetSockRcvtimeo(long sockfd, int ms);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static long Tcp();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static long Udp();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool Bind(long sockfd, string ip, int port);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Connect(long sockfd, string ip, int port);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Listen(long sockfd);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Accept(long sockfd, string ip, int port);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Send(long sockfd, byte[] data, int len);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int Recv(long sockfd, byte[] data);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int Recvfrom(long sockfd, out byte[] data, out string ip, out int port);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int SendTo(long sockfd, byte[] data, string ip, int port);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static long CurrentTime();
    }
}
