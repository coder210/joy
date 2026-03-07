using Livlib.Native;
using System;
using System.Text;


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

    private SDL.FRect player;
    private long sockfd;

    // 设计分辨率（逻辑尺寸）
    const int LOGICAL_WIDTH = 512;
    const int LOGICAL_HEIGHT = 380;

    public void Start(IntPtr ctx)
    {
        context = ctx;
        SDL.CreateWindowAndRenderer("test2", 1024, 760, SDL.WindowsFlag.RESIZABLE, out window, out renderer);
        spriteBatch = SpriteBatch.Create(1009);
        shapeBatch = ShapeBatch.Create();
        shapeBatch2 = ShapeBatch.Create();

        texture1 = SpriteBatch.LoadBmp(renderer, "resources/textures/livnet.bmp");
        texture2 = SpriteBatch.LoadPng(renderer, "resources/textures/livnet.png");

        string ip = Core.GetEnv(context, "ip");
        int port = Convert.ToInt32(Core.GetEnv(context, "port"));

        sockfd = Sys.Udp();
        Console.WriteLine("2sockfd:" + sockfd + " ip:" + ip + " port:" + port);
        Sys.SetSockRcvtimeo(sockfd, 10);
        Sys.SendTo(sockfd, Encoding.UTF8.GetBytes("liwei"), ip, port);
        player.X = 0;
        player.Y = 0;

      
        SDL.SetRenderLogicalPresentation(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT,
 SDL.RendererLogicalPresentation.LOGICAL_PRESENTATION_STRETCH);

        SDL.GetRenderLogicalPresentation(renderer, out int w, out int h, out SDL.RendererLogicalPresentation mode);

        Console.WriteLine("{0}, {1}, {2}", w, h, mode);
    }

    public void Event(SDL.Event @event)
    {
        var eventType = @event.Type;
        //Console.WriteLine("c# event type:{0}", (int)eventType);
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
            }
            else if (scancode == SDL.Scancode.SCANCODE_Q)
            {
                Core.Quit(context);
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

    public void Update()
    {
        int len = Sys.Recvfrom(sockfd, out byte[] buf, out string ip, out int port);
        if (len > 0)
        {
            Console.WriteLine("len:" + len + "buf:" + Encoding.UTF8.GetString(buf) + "ip:" + ip + "port:" + port);
        }

        SDL.SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL.RenderClear(renderer);

        //SDL.FRect detrect = new SDL.FRect() { X = player.X, Y = player.Y, W = 64, H = 64 };
        //SDL.FRect srcrect = new SDL.FRect() { X = 0, Y = 0, W = 64, H = 64 };
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

        ShapeBatch.Rectangle(shapeBatch, new SDL.FRect() { X = 10, Y = 10, W = 40, H = 40 }, 2, new SDL.FColor() { R = 0, G = 255, B = 0, A = 1.0f });
        ShapeBatch.Circle(shapeBatch, new SDL.FPoint() { X = 40, Y = 40 }, 100, 40, 2, new SDL.FColor() { R = 3, G = 3, B = 3, A = 1.0f });

        // 绘制对角线
        SDL.SetRenderDrawColor(renderer, 60, 220, 60, 255);
        SDL.RenderLine(renderer, 0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT);
        SDL.RenderLine(renderer, LOGICAL_WIDTH, 0, 0, LOGICAL_HEIGHT);

        // 绘制网格
        SDL.SetRenderDrawColor(renderer, 100, 100, 150, 100);
        for (int x = 0; x < LOGICAL_WIDTH; x += 50)
        {
            SDL.RenderLine(renderer, x, 0, x, LOGICAL_HEIGHT);
        }
        for (int y = 0; y < LOGICAL_HEIGHT; y += 50)
        {
            SDL.RenderLine(renderer, 0, y, LOGICAL_WIDTH, y);
        }


        SpriteBatch.Render(spriteBatch);
        ShapeBatch.Render(shapeBatch, renderer);
        ShapeBatch.Render(shapeBatch2, renderer);
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
        Console.WriteLine("destroy");
    }
}
