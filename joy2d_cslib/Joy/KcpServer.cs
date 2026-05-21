using Livlib.Native;
using System;
using System.Collections.Generic;

namespace Livlib
{
    public class KcpServer
    {
        private long sockfd;
        private int conv;
        private Dictionary<int, IntPtr> connections;

        public KcpServer(long sockfd)
        {
            this.sockfd = sockfd;
            this.conv = 1;
            this.connections = new Dictionary<int, IntPtr>();
        }


        public void Input(byte[] data, string ip, int port)
        {
            int conv = Utils.Bit2Int(data);
            IntPtr kcpcb = connections[conv];
            if (kcpcb != null)
            {
                //local ip = kcp.get_ip(kcpcb);
                //local port = kcp.get_port(kcpcb);
                //Native.Kcp.GetConv();
                Native.Kcp.Input(kcpcb, data);

            }
            else
            {
                conv = this.conv++;
                Native.Kcp.Create(conv, IntPtr.Zero);
                connections[conv] = kcpcb;
                byte[] buff = Utils.Int2Bit(conv);
                Sys.SendTo(sockfd, buff, ip, port);
            }
        }

        public void Remove(int conv)
        {
            var kcpcb = this.connections[conv];
            if (kcpcb != IntPtr.Zero)
            {
                Kcp.Release(kcpcb);
                this.connections.Remove(conv);
            } 
        }

        public void Send(int conv, byte[] buf)
        {
            var kcpcb = this.connections[conv];
            if (kcpcb != IntPtr.Zero)
            {
                Kcp.Send(kcpcb, buf);
            }
            else
            {
                SDL.Log("kcpcb is nil conv:%d", conv.ToString());
            }
        }

        public void Update(int currentTime, Action<byte[]> onRecv)
        {
            foreach (var item in this.connections)
            {
                Kcp.Update(item.Value, currentTime);
                int n = Kcp.Recv(item.Value, out byte[] buf);
                if (n > 0)
                {
                    onRecv(buf);
                }
            }
        }

    }
}
