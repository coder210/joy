using Livlib.Native;
using System;
using System.Collections.Generic;

public class Person
{
    public string Name { get; set; }
    public int Age { get; set; }
}

public class Program
{
    private IntPtr window;
    private IntPtr renderer;
    private IntPtr context;
    private IntPtr spriteBatch;
    private IntPtr shapeBatch;
    private IntPtr shapeBatch2;
    private Texture2d texture1;
    private Texture2d texture2;
    private int conv;
    private Dictionary<int, IntPtr> connections;

    private SDL.FRect player;
    private long sockfd;

    public void Start(IntPtr ctx)
    {
        context = ctx;
        SDL.CreateWindowAndRenderer("你好", 800, 600, SDL.WindowsFlag.RESIZABLE, out window, out renderer);
        spriteBatch = SpriteBatch.Create(1009);
        shapeBatch = ShapeBatch.Create();
        shapeBatch2 = ShapeBatch.Create();
        

        //texture1 = SpriteBatch.LoadBmp(renderer, "resources/textures/livnet.bmp");
        //texture2 = SpriteBatch.LoadPng(renderer, "resources/textures/livnet.png");

        //string ip = Core.GetEnv(context, "ip");
        //int port = Convert.ToInt32(Core.GetEnv(context, "port"));

        //sockfd = Sys.Udp();
        //Sys.Bind(sockfd, ip, port);
        //Sys.SetSockRcvtimeo(sockfd, 10);
        //this.conv = 1;
        //this.connections = new Dictionary<int, IntPtr>();
        //Console.WriteLine("sockfd:" + sockfd + " ip:" + ip + " port:" + port);

        SDL.SetRenderLogicalPresentation(renderer, 1024, 760, SDL.RendererLogicalPresentation.LOGICAL_PRESENTATION_STRETCH);

        //player.X = 0;
        //player.Y = 0;

        Person p = new Person();
        p.Name = "liwei";
        p.Age = 10;
        Core.Log(ctx, "Init");
    }

    public void Event(SDL.Event @event)
    {
        var eventType = @event.Type;
        Core.Log(context, string.Format("c# event type:{0}", (int)eventType));
        Core.Log(context, "running:" + Core.Running(context).ToString());
        if (eventType == SDL.EventType.EVENT_QUIT)
        {
            Core.Quit(context);
        }
        else if (eventType == SDL.EventType.EVENT_KEY_DOWN)
        {
            SDL.Scancode scancode = @event.Key.Scancode;
            if (scancode == SDL.Scancode.SCANCODE_ESCAPE)
            {
                Core.Quit(context);
                Core.Log(context, "running:" + Core.Running(context).ToString());
            }
            else if (scancode == SDL.Scancode.SCANCODE_Q)
            {
                Core.Quit(context);
                Core.Log(context, "running:" + Core.Running(context).ToString());
            }
            else if (scancode == SDL.Scancode.SCANCODE_UP)
            {
                player.Y -= 10;
            }
            else if (scancode == SDL.Scancode.SCANCODE_DOWN)
            {
                player.Y += 10;
            }
            else if (scancode == SDL.Scancode.SCANCODE_LEFT)
            {
                player.X -= 10;
            }
            else if (scancode == SDL.Scancode.SCANCODE_RIGHT)
            {
                player.X += 10;
            }
            //Console.WriteLine("press key:" + scancode);
        }
    }

    private void OnRecv(int conv, byte[]data)
    {

    }

    public void Update()
    {
        /* recv socket data */
        //int conv, currentTime;
        //IntPtr kcpcb;

        //currentTime = (int)Utils.CurrentTime();

        //int len = Sys.Recvfrom(sockfd, out byte[] data, out string ip, out int port);
        //if (len >= 4)
        //{
        //    conv = Utils.Bit2Int(data);
        //    if (connections.ContainsKey(conv))
        //    {
        //        kcpcb = connections[conv];
        //        Kcp.Input(kcpcb, data);
        //    }
        //    else
        //    {
        //        conv = this.conv++;
        //        kcpcb = Kcp.Create(conv, IntPtr.Zero);
        //        connections[conv] = kcpcb;
        //        byte[] buff = Utils.Int2Bit(conv);
        //        Sys.SendTo(sockfd, buff, ip, port);
        //    }
        //    //Console.WriteLine("len:" + len + "buf:" + buf + "ip:" + ip + "port:" + port);
        //    //Console.WriteLine("len:" + len + "buf:" + Encoding.UTF8.GetString(buf) + "ip:" + ip + "port:" + port);
        //}

        ///* update connection */
        //foreach (var conn in this.connections)
        //{
        //    Kcp.Update(conn.Value, currentTime);
        //    int n = Kcp.Recv(conn.Value, out byte[] buf);
        //    if (n > 0)
        //    {
        //        OnRecv(conn.Key, buf);
        //    }
        //}

        // draw
        SDL.SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL.RenderClear(renderer);

        SDL.FRect detrect = new SDL.FRect() { X = player.X, Y = player.Y, W = 64, H = 64 };
        SDL.FRect srcrect = new SDL.FRect() { X = 0, Y = 0, W = 64, H = 64 };
        //SpriteBatch.Draw(spriteBatch, texture1, srcrect, detrect, new SDL.FColor() { R = 1.0f, G = 1.0f, B = 1.0f, A = 1.0f });


        //detrect = new SDL.FRect() { X = 150, Y = 20, W = 64, H = 64 };
        //srcrect = new SDL.FRect() { X = 10, Y = 10, W = 64, H = 64 };
        //SpriteBatch.Draw(spriteBatch, texture2, srcrect, detrect, new SDL.FColor() { R = 1.0f, G = 1.0f, B = 1.0f, A = 1.0f });

        //detrect = new SDL.FRect() { X = 220, Y = 20, W = 64, H = 64 };
        //srcrect = new SDL.FRect() { X = 0, Y = 0, W = 54, H = 54 };
        //SpriteBatch.Draw(spriteBatch, texture2, srcrect, detrect, new SDL.FColor() { R = 1.0f, G = 1.0f, B = 1.0f, A = 1.0f });

        //detrect = new SDL.FRect() { X = 300, Y = 20, W = 64, H = 64 };
        //srcrect = new SDL.FRect() { X = 0, Y = 0, W = 54, H = 54 };
        //SpriteBatch.Draw(spriteBatch, texture2, srcrect, detrect, new SDL.FColor() { R = 1.0f, G = 1.0f, B = 1.0f, A = 1.0f });

        //detrect = new SDL.FRect() { X = 100, Y = 210, W = 64, H = 64 };
        //srcrect = new SDL.FRect() { X = 0, Y = 20, W = 64, H = 64 };
        //SpriteBatch.Draw(spriteBatch, texture2, srcrect, detrect, new SDL.FColor() { R = 1.0f, G = 1.0f, B = 1.0f, A = 1.0f });

        //Random random = new Random();
        //for (int i = 0; i < 100; i++)
        //{
        //    int k = random.Next();
        //    int k2 = random.Next();
        //    ShapeBatch.Line(shapeBatch, new SDL.FPoint() { X = k, Y = k2 }, new SDL.FPoint() { X = 100, Y = 100 }, 2, new SDL.FColor() { R = 255, G = 255, B = 255, A = 1.0f });
        //}

        //for (int i = 0; i < 100; i++)
        //{
        //    int k = random.Next();
        //    int k2 = random.Next();
        //    ShapeBatch.Line(shapeBatch2, new SDL.FPoint() { X = k, Y = k2 }, new SDL.FPoint() { X = 100, Y = 100 }, 2, new SDL.FColor() { R = 255, G = 0, B = 0, A = 1.0f });
        //}

        //ShapeBatch.Rectangle(shapeBatch, new SDL.FRect() { X = 10, Y = 10, W = 400, H = 400 }, 2, new SDL.FColor() { R = 255, G = 0, B = 0, A = 1.0f });
        //ShapeBatch.Circle(shapeBatch, new SDL.FPoint() { X = 400, Y = 400 }, 100, 40, 2, new SDL.FColor() { R = 255, G = 0, B = 0, A = 1.0f });

        //ShapeBatch.Rectangle(shapeBatch, new SDL.FRect() { X = 10, Y = 10, W = 40, H = 40 }, 2, new SDL.FColor() { R = 0, G = 255, B = 0, A = 1.0f });
        //ShapeBatch.Circle(shapeBatch, new SDL.FPoint() { X = 40, Y = 40 }, 100, 40, 2, new SDL.FColor() { R = 3, G = 3, B = 3, A = 1.0f });

        //SpriteBatch.Render(spriteBatch);
        //ShapeBatch.Render(shapeBatch, renderer);
        //ShapeBatch.Render(shapeBatch2, renderer);
        SDL.RenderPresent(renderer);
    }

    public void Destroy()
    {
        Sys.CloseSocket(sockfd);
        SpriteBatch.Destroy(spriteBatch);
        ShapeBatch.Destroy(shapeBatch);
        ShapeBatch.Destroy(shapeBatch2);
        SDL.DestroyRenderer(renderer);
        SDL.DestroyWindow(window);
        Core.Log(context, "destroy");
    }
}

