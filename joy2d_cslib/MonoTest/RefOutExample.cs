using System;
using System.Runtime.InteropServices;

public class RefOutExample
{
    // ref 参数示例
    public static void RefIncrement(ref int value)
    {
        value++;
        Console.WriteLine($"C#: RefIncrement: 值已增加为 {value}");
    }

    // out 参数示例
    public static bool OutParse(string input, out int result)
    {
        Console.WriteLine($"C#: 尝试解析: {input}");
        if (int.TryParse(input, out result))
        {
            return true;
        }
        result = -1;
        return false;
    }

    // 复杂结构体示例
    public struct Point
    {
        public int X;
        public int Y;
    }

    // ref 结构体参数
    public static void RefMovePoint(ref Point point, int dx, int dy)
    {
        point.X += dx;
        point.Y += dy;
        Console.WriteLine($"C#: 点移动到 ({point.X}, {point.Y})");
    }

    // out 结构体参数
    public static void OutCreatePoint(out Point point, int x, int y)
    {
        point = new Point { X = x, Y = y };
        Console.WriteLine($"C#: 创建点 ({point.X}, {point.Y})");
    }
}