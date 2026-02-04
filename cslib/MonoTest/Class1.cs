using Aspose.Cells;
using Livnet;
using MonoTest;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text.Unicode;


public static class Log2
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal extern static void Info3(string message);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void ProcessIntArray(int[] array, int length);
}


namespace Livnet
{
    [StructLayout(LayoutKind.Sequential)]
    public struct MyData
    {
        public int Id { get; set; }
        public float Value { get; set; }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        private string name;

        public string Name { get { return name; } set { name = value; } }
    }



    public static class Log
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Info(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Info2(out string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static IntPtr GetData( );

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PrintData2(IntPtr mydata);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PrintData3(byte[] bytes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PrintData(MyData mydata);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Callback(Action<int, string, IntPtr> action);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetPointer(IntPtr ptr);
    }

    public static class MyClass
    {
        public static void ProcessData(MyData data)
        {
            Console.WriteLine($"Processed: {data.Id}, {data.Value}, {data.Name}");
            //Console.WriteLine($"Processed: {data.Id}, {data.Value}");
        }

        public static string ProcessData(int id, string value)
        {
            Console.WriteLine($"Processed: {id}, {value}");
            return "ProcessData liwei";
        }

        public static void ConvertToPdf()
        {
            //AsposeExcelHelper.ActiveAspose();
            string filePath = "D:\\web\\水科院文件\\委托报告\\混凝土工程类\\混凝土制品\\25000019\\20250530140244_25000019.xls";
            byte[] fileBytes = File.ReadAllBytes(filePath);
            MemoryStream steam = new MemoryStream(fileBytes);
            AsposeExcelHelper.ConvertToPdf(steam, "D:\\output.pdf");
            steam.Dispose();
        }
    }
}

namespace Test
{
    public class Class1
    {
        public static void Fun()
        {
            try
            {
                int i = 0;
                int result = 100 / i;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            Console.WriteLine("Hello World");
            List<string> names = new List<string>() {
                "liwei",
                "水利"
            };

            Queue<string> tq = new Queue<string>();
            tq.Enqueue("ok");


            string str = "from c#";
            Log.Info(str);
            Log.Info2(out str);
            Log.Info(str);

            Console.WriteLine("================================");
            Console.WriteLine("================================");
            Log.PrintData3(System.Text.UTF8Encoding.UTF8.GetBytes("PrintData3: const char* "));

            Log2.Info3("what's this");
            List<int> intArray = new List<int>() { 
                10,
                20,
                30,
                40,
                50
            };
            Log2.ProcessIntArray(intArray.ToArray(), intArray.Count);

            Log.PrintData(new MyData()
            {
                Id = 19,
                Value = 20.2f,
            });

            Console.WriteLine("================================");

            IntPtr t2 = Log.GetData();
            Log.PrintData2(t2);

            foreach (string name in names)
            {
                Console.WriteLine(name);
            }

            Log.Callback((id, name, pointer) => {
                Console.WriteLine("Callback:" + id + "," + name);
                Console.WriteLine("C Pointer:" + pointer);

                int value = Marshal.ReadInt32(pointer);
                Console.WriteLine($"C#: 原始值: {value}");

                int newValue = value * 2;
                Marshal.WriteInt32(pointer, newValue);

                Log.SetPointer(pointer);
            });

            Console.WriteLine("============================");

            IntPtr env = Native.SDL.CreateEnvironment(true);
            Native.SDL.SetEnvironmentVariable(env, "name", "liwei", true);
            string value = Native.SDL.GetEnvironmentVariable(env, "name");
            Console.WriteLine("GetEnvironmentVariable:" + value);
            Native.SDL.DestroyEnvironment(env);

            Console.WriteLine("=================打印pdf===================");
            //MyClass.ConvertToPdf();
            Console.WriteLine("=================打印pdf===================");
        }
    }
}
