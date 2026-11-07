using System;

namespace JoyGame
{
    /// <summary>
    /// C 宿主反射调用的入口：OnInit / OnFrame / OnShutdown。
    /// 名字与参数个数必须和仓库根 main.c 里的 csx_find_method 一致。
    /// </summary>
    public static class Host
    {
        private static GameApp _app;
        private static SmokeScript _smoke;

        /// <summary>
        /// 宿主只传尺寸与冒烟帧数：窗口与渲染器由 GameApp 自己用生成链路创建，
        /// 不再由 C 宿主建好再传裸指针过来（裸指针不在生成链路的句柄表里）。
        /// </summary>
        public static void OnInit(int width, int height, int smokeFrames)
        {
            _app = new GameApp(width, height, smokeFrames);
            _smoke = smokeFrames > 0 ? new SmokeScript() : null;
            if (smokeFrames > 0) ApiBindingSmoke.Run();
            _app.Start();
            Joy.Host.log("[host] init " + width + "x" + height +
                         (smokeFrames > 0 ? " smoke=" + smokeFrames : ""));
        }

        public static void OnFrame(float dt)
        {
            if (_app == null) return;
            _smoke?.BeforeTick(_app);
            _app.Tick(dt);
            _smoke?.AfterTick(_app);
        }

        public static void OnShutdown()
        {
            Joy.Host.log("[host] shutdown frames=" + (_app != null ? _app.Frame : 0));
        }
    }
}
