using Aspose.Cells;

namespace MonoTest
{
    public class AsposeExcelHelper
    {
        /// <summary>
        /// 激活Aspose,可用,但现在所用的dll是破解版的,所以暂且放置
        /// 程序中只需要调用一次即可
        /// </summary>
        public static void ActiveAspose()
        {
            string licenseData = "DQo8TGljZW5zZT4NCjxEYXRhPg0KPExpY2Vuc2VkVG8+VGhlIFdvcmxkIEJhbms8L0xpY2Vuc2VkVG8+DQo8RW1haWxUbz5ra3VtYXIzQHdvcmxkYmFua2dyb3VwLm9yZzwvRW1haWxUbz4NCjxMaWNlbnNlVHlwZT5EZXZlbG9wZXIgU21hbGwgQnVzaW5lc3M8L0xpY2Vuc2VUeXBlPg0KPExpY2Vuc2VOb3RlPjEgRGV2ZWxvcGVyIEFuZCAxIERlcGxveW1lbnQgTG9jYXRpb248L0xpY2Vuc2VOb3RlPg0KPE9yZGVySUQ+MjEwMzE2MTg1OTU3PC9PcmRlcklEPg0KPFVzZXJJRD43NDQ5MTY8L1VzZXJJRD4NCjxPRU0+VGhpcyBpcyBub3QgYSByZWRpc3RyaWJ1dGFibGUgbGljZW5zZTwvT0VNPg0KPFByb2R1Y3RzPg0KPFByb2R1Y3Q+QXNwb3NlLlRvdGFsIGZvciAuTkVUPC9Qcm9kdWN0Pg0KPC9Qcm9kdWN0cz4NCjxFZGl0aW9uVHlwZT5Qcm9mZXNzaW9uYWw8L0VkaXRpb25UeXBlPg0KPFNlcmlhbE51bWJlcj4wM2ZiMTk5YS01YzhhLTQ4ZGItOTkyZS1kMDg0ZmYwNjZkMGM8L1NlcmlhbE51bWJlcj4NCjxTdWJzY3JpcHRpb25FeHBpcnk+MjAyMjA1MTY8L1N1YnNjcmlwdGlvbkV4cGlyeT4NCjxMaWNlbnNlVmVyc2lvbj4zLjA8L0xpY2Vuc2VWZXJzaW9uPg0KPExpY2Vuc2VJbnN0cnVjdGlvbnM+aHR0cHM6Ly9wdXJjaGFzZS5hc3Bvc2UuY29tL3BvbGljaWVzL3VzZS1saWNlbnNlPC9MaWNlbnNlSW5zdHJ1Y3Rpb25zPg0KPC9EYXRhPg0KPFNpZ25hdHVyZT5XbkJYNnJOdHpCclNMV3pBdFlqOEtkdDFLSUI5MlFrL2xEbFNmMlM1TFRIWGdkcS9QQ2NqWHVORmp0NEJuRmZwNFZLc3VsSjhWeFExakIwbmM0R1lWcWZLek14SFFkaXFuZU03NTJaMjlPbmdyVW40Yk0rc1l6WWVSTE9UOEpxbE9RN05rRFU0bUk2Z1VyQ3dxcjdnUVYxbDJJWkJxNXMzTEFHMFRjQ1ZncEE9PC9TaWduYXR1cmU+DQo8L0xpY2Vuc2U+DQo=";
            MemoryStream stream = new MemoryStream(Convert.FromBase64String(licenseData));
            stream.Seek(0, SeekOrigin.Begin);
            License license = new License();
            license.SetLicense(stream);
        }

        /// <summary>
        /// 转成pdf
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="pdfFilePath"></param>
        public static void ConvertToPdf(Stream stream, string pdfFilePath)
        {
            Workbook workbook = new Workbook(stream);
            PdfSaveOptions saveOptions = new PdfSaveOptions();
            saveOptions.OnePagePerSheet = true; // 设置每张工作表为一页
            saveOptions.CalculateFormula = true;
            workbook.Save(pdfFilePath, saveOptions);
        }

        /// <summary>
        /// 只转某个sheet成pdf
        /// </summary>
        /// <param name="stream">流</param>
        /// <param name="sheetIndex">sheet的索引</param>
        /// <param name="pdfFilePath"></param>
        public static void ConvertToPdf(Stream stream, int sheetIndex, string pdfFilePath)
        {
            Workbook workbook = new Workbook(stream);

            foreach (Worksheet sheet in workbook.Worksheets)
            {
                if (sheet.IsVisible)
                {
                    if (sheet.Index != sheetIndex)
                    {
                        sheet.IsVisible = false;
                    }
                }
            }

            PdfSaveOptions saveOptions = new PdfSaveOptions();
            saveOptions.OnePagePerSheet = true;
            saveOptions.CalculateFormula = true;
            workbook.Save(pdfFilePath, saveOptions);
        }

        /// <summary>
        /// 只转某个sheet成pdf
        /// </summary>
        /// <param name="stream">流</param>
        /// <param name="name">sheet的名字</param>
        /// <param name="pdfFilePath"></param>
        public static void ConvertToPdf(Stream stream, string name, string pdfFilePath)
        {
            Workbook workbook = new Workbook(stream);
            foreach (Worksheet sheet in workbook.Worksheets)
            {
                string sheetName = sheet.Name;
                if (sheet.IsVisible)
                {
                    if (!sheetName.Contains(name))
                    {
                        sheet.IsVisible = false;
                    }
                }
            }
            PdfSaveOptions saveOptions = new PdfSaveOptions();
            saveOptions.OnePagePerSheet = true;
            saveOptions.CalculateFormula = true;
            workbook.Save(pdfFilePath, saveOptions);
        }

        /// <summary>
        /// 只转某个sheet成pdf
        /// </summary>
        /// <param name="stream">流</param>
        /// <param name="names">sheet的名字列表</param>
        /// <param name="pdfFilePath"></param>
        public static void ConvertToPdf(Stream stream, string[] names, string pdfFilePath)
        {
            Workbook workbook = new Workbook(stream);
            foreach (Worksheet sheet in workbook.Worksheets)
            {
                string sheetName = sheet.Name;
                if (sheet.IsVisible)
                {
                    bool visible = false;
                    foreach (string name in names)
                    {
                        if (sheetName.Contains(name))
                        {
                            visible = true;
                            break;
                        }
                    }

                    sheet.IsVisible = visible;
                }
            }
            PdfSaveOptions saveOptions = new PdfSaveOptions();
            saveOptions.OnePagePerSheet = true;
            saveOptions.CalculateFormula = true;
            workbook.Save(pdfFilePath, saveOptions);
        }

    }
}
