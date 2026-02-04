using System;
using System.IO;
using System.Runtime.CompilerServices;


public static class Log2
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal extern static void Info3(string message);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void ProcessIntArray(int[] array, int length);
}


//namespace Livnet
//{
//    [StructLayout(LayoutKind.Sequential)]
//    public struct MyData
//    {
//        public int Id { get; set; }
//        public float Value { get; set; }

//        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
//        private string name;

//        public string Name { get { return name; } set { name = value; } }
//    }


//    public static class Log
//    {
//        [MethodImpl(MethodImplOptions.InternalCall)]
//        internal extern static void Info(string message);

//        [MethodImpl(MethodImplOptions.InternalCall)]
//        internal extern static void Info2(out string message);

//        [MethodImpl(MethodImplOptions.InternalCall)]
//        internal extern static IntPtr GetData( );

//        [MethodImpl(MethodImplOptions.InternalCall)]
//        internal extern static void PrintData2(IntPtr mydata);

//        [MethodImpl(MethodImplOptions.InternalCall)]
//        internal extern static void PrintData(MyData mydata);

//        [MethodImpl(MethodImplOptions.InternalCall)]
//        internal extern static void Callback(Action<int, string, IntPtr> action);

//        [MethodImpl(MethodImplOptions.InternalCall)]
//        internal extern static void SetPointer(IntPtr ptr);
//    }

//    public static class MyClass
//    {
//        public static void ProcessData(MyData data)
//        {
//            Console.WriteLine($"Processed: {data.Id}, {data.Value}, {data.Name}");
//            //Console.WriteLine($"Processed: {data.Id}, {data.Value}");
//            Console.WriteLine("");
//            Console.WriteLine("2222");
//            Console.WriteLine("ok");
//            Console.WriteLine("你好");

//            // ASPOSEXLSTEST_FILES
//            // SPIREXLSTEST_FILES


//        }

//        public static string ProcessData(int id, string value)
//        {
//            Console.WriteLine($"Processed: {id}, {value}");
//            return "ProcessData liwei";
//        }
//    }

//}







namespace Test
{


    public class Class1
    {
        /// <summary>
        /// test
        /// </summary>
        private static void Test()
        {
            //try
            //{
            //    int i = 0;
            //    int result = 100 / i;
            //}
            //catch (Exception ex)
            //{
            //    Console.WriteLine(ex.ToString());
            //}
            //Console.WriteLine("Hello World");
            //List<string> names = new List<string>() {
            //    "liwei",
            //    "水利"
            //};

            ////Queue<string> tq = new Queue<string>();
            ////tq.Enqueue("ok");


            //string str = "from c#";
            //Log.Info(str);
            //Log.Info2(out str);
            //Log.Info(str);

            //Log2.Info3("what's this");
            //List<int> intArray = new List<int>() { 
            //    10,
            //    20,
            //    30,
            //    40,
            //    50
            //};
            //Log2.ProcessIntArray(intArray.ToArray(), intArray.Count);

            //Log.PrintData(new MyData()
            //{
            //    Id = 19,
            //    Value = 20.2f,
            //});

            //Console.WriteLine("================================");

            //IntPtr t2 = Log.GetData();
            //Log.PrintData2(t2);

            //foreach (string name in names)
            //{
            //    Console.WriteLine(name);
            //}

            //Log.Callback((id, name, pointer) => {
            //    Console.WriteLine("Callback:" + id + "," + name);
            //    Console.WriteLine("C Pointer:" + pointer);

            //    int value = Marshal.ReadInt32(pointer);
            //    Console.WriteLine($"C#: 原始值: {value}");

            //    int newValue = value * 2;
            //    Marshal.WriteInt32(pointer, newValue);

            //    Log.SetPointer(pointer);
            //});
        }

        /// <summary>
        /// test2
        /// </summary>
        private static void Test2() 
        {
            //Console.WriteLine("CSharp========================");
            //{
            //    Vec2 a = new Vec2();
            //    a.X = 10.1f;
            //    a.Y = 20.2f;
            //    Vec2 b = new Vec2();
            //    b.X = 10.1f;
            //    b.Y = 10.1f;
            //    Vec2 r = Vec2.Add(a, b);
            //    Console.WriteLine(r.X);
            //    Console.WriteLine(r.Y);
            //}

            //{
            //    Vec2F a = new Vec2F();
            //    a.X = FP.FromFloat(10.1f);
            //    a.Y = FP.FromFloat(20.2f);
            //    Vec2F b = new Vec2F();
            //    b.X = FP.FromFloat(10.1f);
            //    b.Y = FP.FromFloat(10.1f);
            //    Vec2F r = Vec2F.Add(a, b);
            //    Console.WriteLine(FP.ToFloat(r.X));
            //    Console.WriteLine(FP.ToFloat(r.Y));

            //    Vec2F r2 = Vec2F.Sub(a, b);
            //    Console.WriteLine(FP.ToFloat(r2.X));
            //    Console.WriteLine(FP.ToFloat(r2.Y));
            //}
        }



        private static void ActiveAspose()
        {
            //string licenseData = "DQo8TGljZW5zZT4NCjxEYXRhPg0KPExpY2Vuc2VkVG8+VGhlIFdvcmxkIEJhbms8L0xpY2Vuc2VkVG8+DQo8RW1haWxUbz5ra3VtYXIzQHdvcmxkYmFua2dyb3VwLm9yZzwvRW1haWxUbz4NCjxMaWNlbnNlVHlwZT5EZXZlbG9wZXIgU21hbGwgQnVzaW5lc3M8L0xpY2Vuc2VUeXBlPg0KPExpY2Vuc2VOb3RlPjEgRGV2ZWxvcGVyIEFuZCAxIERlcGxveW1lbnQgTG9jYXRpb248L0xpY2Vuc2VOb3RlPg0KPE9yZGVySUQ+MjEwMzE2MTg1OTU3PC9PcmRlcklEPg0KPFVzZXJJRD43NDQ5MTY8L1VzZXJJRD4NCjxPRU0+VGhpcyBpcyBub3QgYSByZWRpc3RyaWJ1dGFibGUgbGljZW5zZTwvT0VNPg0KPFByb2R1Y3RzPg0KPFByb2R1Y3Q+QXNwb3NlLlRvdGFsIGZvciAuTkVUPC9Qcm9kdWN0Pg0KPC9Qcm9kdWN0cz4NCjxFZGl0aW9uVHlwZT5Qcm9mZXNzaW9uYWw8L0VkaXRpb25UeXBlPg0KPFNlcmlhbE51bWJlcj4wM2ZiMTk5YS01YzhhLTQ4ZGItOTkyZS1kMDg0ZmYwNjZkMGM8L1NlcmlhbE51bWJlcj4NCjxTdWJzY3JpcHRpb25FeHBpcnk+MjAyMjA1MTY8L1N1YnNjcmlwdGlvbkV4cGlyeT4NCjxMaWNlbnNlVmVyc2lvbj4zLjA8L0xpY2Vuc2VWZXJzaW9uPg0KPExpY2Vuc2VJbnN0cnVjdGlvbnM+aHR0cHM6Ly9wdXJjaGFzZS5hc3Bvc2UuY29tL3BvbGljaWVzL3VzZS1saWNlbnNlPC9MaWNlbnNlSW5zdHJ1Y3Rpb25zPg0KPC9EYXRhPg0KPFNpZ25hdHVyZT5XbkJYNnJOdHpCclNMV3pBdFlqOEtkdDFLSUI5MlFrL2xEbFNmMlM1TFRIWGdkcS9QQ2NqWHVORmp0NEJuRmZwNFZLc3VsSjhWeFExakIwbmM0R1lWcWZLek14SFFkaXFuZU03NTJaMjlPbmdyVW40Yk0rc1l6WWVSTE9UOEpxbE9RN05rRFU0bUk2Z1VyQ3dxcjdnUVYxbDJJWkJxNXMzTEFHMFRjQ1ZncEE9PC9TaWduYXR1cmU+DQo8L0xpY2Vuc2U+DQo=";
            //MemoryStream stream = new MemoryStream(Convert.FromBase64String(licenseData));
            //stream.Seek(0, SeekOrigin.Begin);
            //License license = new License();
            //license.SetLicense(stream);
        }

        /// <summary>
        /// 打印报告
        /// </summary>
        public static void BuildReport()
        {
            //ActiveAspose();

            //string filePath = "D:/web/水科院文件/委托报告/混凝土工程类/混凝土制品/25000019/20250530140244_25000019.xls";
            //byte[] fileBytes = File.ReadAllBytes(filePath);
            //MemoryStream steam = new MemoryStream(fileBytes);

            ////Spire.Xls.Workbook workbook = new Spire.Xls.Workbook();
            ////workbook.LoadFromStream(steam);
            //////workbook.ConverterSetting.PdfSecurity.Encrypt("111111", "222222", PdfPermissionsFlags.Print, PdfEncryptionKeySize.Key128Bit);
            //string newFile = "D:\\" + DateTime.Now.ToString("yyyyMMddHHmmss") + ".pdf";
            ////workbook.SaveToFile(newFile, Spire.Xls.FileFormat.PDF);

            //// 加载Excel文件
            //Workbook workbook = new Workbook(filePath);

            //// 设置PDF保存选项
            //PdfSaveOptions saveOptions = new PdfSaveOptions();
            //saveOptions.OnePagePerSheet = true; // 设置每张工作表为一页

            //// 保存为PDF
            //workbook.Save(newFile, saveOptions);



            //// 转换为HTML
            //workbook.Save("D:\\output.html", SaveFormat.Html);

            //workbook.Dispose();

            //steam.Dispose();
        }


        public static void Callback(IntPtr ctx, int type, int source, string data)
        {
            Console.WriteLine("callback");
            Console.WriteLine(ctx);
            Console.WriteLine(type);
            Console.WriteLine(source);
            Console.WriteLine(data);

           

        }

        public static void Fun()
        {
            Test2();

            string filePath = "D:/web/水科院文件/委托报告/混凝土工程类/混凝土制品/25000019/20250530140244_25000019.xls";
            //string filename = Path.GetFileNameWithoutExtension(filePath);
            //string pdfPath = filename + "_report_build.pdf";
            string pdfPath = Path.Combine(Path.GetDirectoryName(filePath), Path.GetFileNameWithoutExtension(filePath) + "_report_build.pdf");
            Console.WriteLine(filePath);
            Console.WriteLine(pdfPath);

            Console.WriteLine(Path.GetDirectoryName(filePath));
            Console.WriteLine("============================================");


            {
                string dir = Path.GetDirectoryName(filePath);
                string filename = Path.GetFileNameWithoutExtension(filePath);
                string pdfPath2 = dir + "/" + filename + "_report_build.pdf";
                Console.WriteLine(pdfPath2);
            }


            


            //BuildReport();
        }
    }
}
