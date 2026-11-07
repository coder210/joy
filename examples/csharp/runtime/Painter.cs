using System;
using Joy.Api;

namespace JoyGame
{
    /// <summary>
    /// C# 侧的绘制门面，全部走生成链路（Joy.Api.Bindings）。
    ///
    /// 生成链路里 R2dSetColorf 是「设当前色」，ShapeDrawRectangle / ShapeDrawLineF
    /// 都用这个当前色，所以每个绘制函数先设色再画。
    /// 文字宽度测量（jtext_measurex）是 L3 的输出参数，尚未开放，居中用估算。
    /// </summary>
    internal static class Painter
    {
        public static void Fill(GameApp app, float x, float y, float w, float h,
                                int r, int g, int b, int a = 255)
        {
            Bindings.R2dSetColorf(app.R2d, r / 255f, g / 255f, b / 255f, a / 255f);
            Bindings.ShapeDrawRectangle(app.R2d, "fill", x, y, w, h);
        }

        public static void Stroke(GameApp app, float x, float y, float w, float h,
                                  int r, int g, int b, int a = 255)
        {
            Bindings.R2dSetColorf(app.R2d, r / 255f, g / 255f, b / 255f, a / 255f);
            Bindings.ShapeDrawRectangle(app.R2d, "line", x, y, w, h);
        }

        public static void Line(GameApp app, float x1, float y1, float x2, float y2,
                                int r, int g, int b, int a = 255)
        {
            Bindings.R2dSetColorf(app.R2d, r / 255f, g / 255f, b / 255f, a / 255f);
            Bindings.ShapeDrawLineF(app.R2d, x1, y1, x2, y2);
        }

        public static void Text(GameApp app, string text, float x, float y,
                                int r = 232, int g = 236, int b = 245)
        {
            if (app.Assets.FontSmall == null) return;
            Bindings.JtextPrintx(app.R2d, app.Assets.FontSmall, text, text.Length,
                                 (uint)r, (uint)g, (uint)b, 255u, x, y);
        }

        public static void TextCenter(GameApp app, string text, float cx, float cy, bool big = false,
                                      int r = 232, int g = 236, int b = 245)
        {
            Font font = big ? app.Assets.FontBig : app.Assets.FontSmall;
            if (font == null) return;

            int size = big ? 22 : 12;
            float w = text.Length * size * 0.58f;
            Bindings.JtextPrintx(app.R2d, font, text, text.Length,
                                 (uint)r, (uint)g, (uint)b, 255u,
                                 cx - w * 0.5f, cy - size * 0.5f);
        }

        public static int TextWidth(GameApp app, string text)
        {
            if (app.Assets.FontSmall == null) return 0;
            return (int)(text.Length * 12 * 0.58f);
        }

        /// <summary>
        /// 以 (cx, cy) 为中心画精灵。
        /// 生成链路里 image_draw1 是 jrect_t* 指针参数（L3，未开放），
        /// 所以统一画成同尺寸色块，与 lua / js 两份的行为一致。
        /// </summary>
        public static void Sprite(GameApp app, IntPtr image, float cx, float cy, float size,
                                  int fallbackR, int fallbackG, int fallbackB)
        {
            Fill(app, cx - size * 0.5f, cy - size * 0.5f, size, size,
                 fallbackR, fallbackG, fallbackB);
        }
    }
}
