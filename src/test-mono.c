#include <stdio.h>
#include <string.h>

//
//typedef struct {
//	int id; 
//	float value; 
//	MonoString *name;
//} MyData;
//
//// 定义与 C# Point 结构体匹配的结构
//typedef struct {
//	int X;
//	int Y;
//} Point;
//
//
//static void process_int_array(int* array, int length)
//{
//	printf("Received int array:\n");
//	for (int i = 0; i < length; i++)
//	{
//		printf("%d ", array[i]);
//	}
//	printf("\n");
//}
//
//static void log_info(MonoString * string)
//{
//	char* str = mono_string_to_utf8(string);
//	printf("c++ %s\n", str);
//	mono_free(string);
//}
//
//static void log_info2(MonoString** string)
//{
//	char* str = mono_string_to_utf8(*string);
//	printf("log_info2 %s\n", str);
//	*string = mono_string_new(mono_domain_get(), "log info2 liwei");
//	mono_free(string);
//}
//
//static void log_info3(MonoString* string)
//{
//	char* str = mono_string_to_utf8(string);
//	printf("no namespace c++ %s\n", str);
//	mono_free(string);
//}
//
//static void print_data(MyData * mydata)
//{
//	printf("print_data %d, %f\n", mydata->id, mydata->value);
//}
//
//static void print_data2(MyData* mydata)
//{
//	printf("print_data %d, %f\n", mydata->id, mydata->value);
//}
//
//
//static void print_data3(const char* str)
//{
//	printf("print_data %s\n", str);
//}
//
//static MyData* get_data()
//{
//	MyData* data = (MyData*)malloc(sizeof(MyData*));
//	data->id = 10;
//	data->value = 10.02f;
//	//data->name = mono_string_new(mono_domain_get(), "log info2 liwei");
//	return data;
//}
//
//
////typedef void (*fn_t)();
//static void callback(MonoObject *del)
//{
//	MyData* mydata = (MyData*)malloc(sizeof(MyData));
//	mydata->id = 10000;
//	mydata->value = 10.02f;
//
//	intptr_t ptrValue = (intptr_t)mydata;
//	
//	int id = 102;
//	void* args[3];
//	args[0] = &id;
//	args[1] = mono_string_new(mono_domain_get(), "liwei");
//	args[2] = &ptrValue;
//	mono_runtime_delegate_invoke(del, args, NULL);
//}
//
//static void set_pointer(void* mydata)
//{
//	int i = 10;
//	MyData* mydata2 = (MyData*)mydata;
//	printf("C:%d,%f\n", mydata2->id, mydata2->value);
//}
//
//static char*
//utils_read_file(const char* filename, size_t* sz)
//{
//	FILE* fp;
//	char* buf;
//	long total_size;
//
//	fp = fopen(filename, "rb");
//	if (!fp) {
//		*sz = 0;
//		return NULL;
//	}
//	fseek(fp, 0, SEEK_END);
//	total_size = ftell(fp);
//	rewind(fp);
//	buf = (char*)malloc((total_size + 1) * sizeof(char));
//	if (buf)
//		*sz = fread(buf, 1, total_size, fp);
//	else
//		*sz = 0;
//	fclose(fp);
//	return buf;
//}
//
//void ProcessReturnValue(MonoObject* result) {
//	if (!result) {
//		printf("Void return\n");
//		return;
//	}
//	
//	MonoClass* returnClass = MONO_OBJECT_GET_CLASS(result);
//
//	if (returnClass == MONO_GET_INT32_CLASS()) {
//		int value = *(int*)MONO_OBJECT_UNBOX(result);
//		printf("Int: %d\n", value);
//	}
//	else if (returnClass == MONO_GET_STRING_CLASS()) {
//		char* str = mono_string_to_utf8((MonoString*)result);
//		printf("String: %s\n", str);
//		//mono_free(str);
//	}
//	else if (returnClass == MONO_GET_BOOLEAN_CLASS()) {
//		//bool value = *(bool*)MONO_OBJECT_UNBOX(result);
//		//printf("Bool: %s\n", value ? "true" : "false");
//	}
//	else if (strcmp(MONO_CLASS_GET_NAME(returnClass), "Point") == 0) {
//		typedef struct { int X; int Y; } Point;
//		Point* point = (Point*)MONO_OBJECT_UNBOX(result);
//		printf("Point: (%d, %d)\n", point->X, point->Y);
//	}
//	else {
//		printf("Unhandled return type: %s\n", MONO_CLASS_GET_NAME(returnClass));
//	}
//}
//
//int test_mono(MonoDomain* domain)
//{
//	// 第一个参数是标准库目录，第二个参数已经废弃
//	//domain = mono_jit_init("TestMono");
//
//	//init_mono_delegate_system(domain);
//
//	/*
//	'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
//	
//	*/
//
//	// 验证核心库已加载
//	//MonoAssembly* corelib = mono_assembly_open("System.Private.CoreLib", NULL);
//	//if (!corelib) {
//	//	printf("核心库加载失败！\n");
//	//	return -1;
//	//}
//	//printf("核心库版本: %s\n",
//		//mono_assembly_name_get_version(mono_assembly_get_name(corelib)));
//
//
//	//ASSERT_LOG_RETURN(domain != NULL, 1, "error: domain");
//	// 第二个参数填入要加载的c#的il
//	//MonoAssembly* assembly = mono_domain_assembly_open(domain, "E:\\c++\\mono_test\\x64\\Debug\\Test.dll");
//
//
//	//ASSERT_LOG_RETURN(assembly != NULL, 1, "error: assembly");
//
//	//MonoImage* image = mono_assembly_get_image(assembly);
//	//ASSERT_LOG_RETURN(image != NULL, 1, "error: image");
//	size_t data_size;
//	char* assembly_data = utils_read_file("D:/music/livnet/cslib/MonoTest/bin/Debug/net8.0/MonoTest.dll", &data_size);
//	//const size_t data_size = sizeof(assembly_data);
//
//	MonoImageOpenStatus status;
//	MonoImage* image = MONO_IMAGE_OPEN_FROM_DATA(assembly_data, data_size, 1, &status);
//	if (status != MONO_IMAGE_OK || !image) {
//		//fprintf(stderr, "创建映像失败: %d\n", status);
//		//return NULL;
//
//	}
//
//
//	
//
//	// 2. 设置程序集名称（可选但推荐）
//	//const char * t = mono_image_get_name(image);
//	////mono_image_set_name(image, "Test");
// //        // 3. 从映像创建程序集
//	//MonoAssembly* assembly = mono_assembly_load_from_full(
//	//	image,          // 内存映像
//	//	"Test",           // 程序集名称
//	//	&status,        // 状态码
//	//	0               // 仅用于反射
//	//);
//
//
//	MonoClass* main_class = MONO_CLASS_FROM_NAME(image, "Test", "Class1");
//	//ASSERT_LOG_RETURN(image != NULL, 1, "error: main_class");
//
//	// 注册，此处第一个参数最后的名字就是C#中要用的方法名，前面的命名空间和类名可以自定义
//	MONO_REGISTER("Livnet.Log::Info", log_info);
//	MONO_REGISTER("Livnet.Log::Info2", log_info2);
//	MONO_REGISTER("Log2::Info3", log_info3);
//	MONO_REGISTER("Log2::ProcessIntArray", process_int_array);
//	MONO_REGISTER("Livnet.Log::GetData", get_data);
//	MONO_REGISTER("Livnet.Log::PrintData2", print_data2);
//	MONO_REGISTER("Livnet.Log::PrintData3", print_data3);
//	MONO_REGISTER("Livnet.Log::PrintData", print_data);
//	MONO_REGISTER("Livnet.Log::Callback", callback);
//	MONO_REGISTER("Livnet.Log::SetPointer", set_pointer);
//
//	// 获取详细错误信息
//
//	MonoMethod* mono_method = MONO_CLASS_GET_METHOD_FROM_NAME(main_class, "Fun", 0);
//	//ASSERT_LOG_RETURN(mono_method != NULL, 1, "error: main_class");
//	MONO_RUNTIME_INVOKE(mono_method, NULL, NULL, NULL);
//	{
//		MonoClass* my_class = MONO_CLASS_FROM_NAME(image, "Livnet", "MyClass");
//		MonoMethod* process_data_method = MONO_CLASS_GET_METHOD_FROM_NAME(my_class, "ProcessData", 2);
//
//		// 5. 准备参数
//		int id = 123;  // 整数参数
//		const char* value = "Hello from C";  // 字符串参数
//
//		// 创建 Mono 字符串对象
//		MonoString* monoString = mono_string_new(domain, value);
//
//		// 参数数组（注意顺序：id 在前，value 在后）
//		void* args[2];
//		args[0] = &id;         // int 参数需要传递指针
//		args[1] = monoString;  // 字符串直接传递对象指针
//
//		// 6. 调用方法
//		MonoObject* result = MONO_RUNTIME_INVOKE(
//			process_data_method,  // 方法指针
//			NULL,    // 实例对象（静态方法为 NULL）
//			args,    // 参数数组
//			NULL     // 异常对象指针（可设为 NULL）
//		);
//		ProcessReturnValue(result);
//	}
//
//	// 水利项目代码单元功能测试
//	MyData data;
//	data.id = 100;
//	data.name = mono_string_new(domain, "LIMS");
//	/*strcpy(data.name, "LIMS");
//	data.name[sizeof(data.name) - 1] = 0;*/
//	data.value = 10.0f;
//
//
//	{
//
//		MonoClass* my_class = MONO_CLASS_FROM_NAME(image, "Livnet", "MyClass");
//		MonoMethod* process_data_method = MONO_CLASS_GET_METHOD_FROM_NAME(my_class, "ProcessData", 1);
//		//MonoClass* myDataClass = MONO_CLASS_FROM_NAME(image, "Livnet", "MyData");
//		//MonoObject* boxed = mono_value_box(domain, myDataClass, &data);
//		void* args[1];
//		args[0] = &data;
//
//		MonoObject* exception;
//		MONO_RUNTIME_INVOKE(process_data_method, NULL, args, &exception);
//		if (exception) {
//			MonoString* excStr = MONO_OBJECT_TO_STRING(exception, NULL);
//			printf("Exception: %s\n", mono_string_to_utf8(excStr));
//		}
//
//	}
//
//	{
//		// 获取 RefOutExample 类
//		MonoClass* exampleClass = MONO_CLASS_FROM_NAME(image, "", "RefOutExample");
//		if (!exampleClass) {
//			fprintf(stderr, "未找到 RefOutExample 类\n");
//			return 1;
//		}
//
//		// 示例1: 处理 ref int 参数
//		printf("=== 示例1: ref int 参数 ===\n");
//		{
//			// 查找方法
//                        MonoMethod* ref_method;
//			ref_method = MONO_CLASS_GET_METHOD_FROM_NAME(exampleClass, "RefIncrement", 1);
//
//			if (!ref_method) {
//				fprintf(stderr, "未找到 RefIncrement 方法\n");
//				return 1;
//			}
//
//			// 准备参数
//			int value = 10;
//			printf("C: 调用前值: %d\n", value);
//
//			// 创建参数数组 (ref 参数需要传递指针)
//			void* args[1];
//			args[0] = &value;
//
//			// 调用方法
//			MONO_RUNTIME_INVOKE(ref_method, NULL, args, NULL);
//			printf("C: 调用后值: %d\n", value);
//		}
//
//		// 示例2: 处理 out int 参数
//		printf("\n=== 示例2: out int 参数 ===\n");
//		{
//			// 查找方法
//			MonoMethod* out_method;
//			out_method = MONO_CLASS_GET_METHOD_FROM_NAME(exampleClass, "OutParse", 2);
//			if (!out_method) {
//				fprintf(stderr, "未找到 OutParse 方法\n");
//				return 1;
//			}
//
//			// 准备参数
//			const char* input = "42";
//			int result = 0; // 不需要初始化，但需要内存空间
//
//			// 创建参数数组
//			void* args[2];
//			args[0] = mono_string_new(domain, input); // 字符串参数
//			args[1] = &result; // out 参数传递指针
//
//			// 调用方法并获取返回值
//			MonoObject* returnValue = MONO_RUNTIME_INVOKE(out_method, NULL, args, NULL);
//			int success = (*(int*)MONO_OBJECT_UNBOX(returnValue));
//			printf("C: 解析成功: %s\n", success ? "是" : "否");
//			printf("C: 解析结果: %d\n", result);
//		}
//
//		// 示例3: 处理 ref 结构体参数
//		printf("\n=== 示例3: ref 结构体参数 ===\n");
//		{
//			// 查找方法
//			MonoMethod* ref_struct_method;
//			ref_struct_method = MONO_CLASS_GET_METHOD_FROM_NAME(exampleClass, "RefMovePoint", 3);
//			if (!ref_struct_method) {
//				fprintf(stderr, "未找到 RefMovePoint 方法\n");
//				return 1;
//			}
//
//			// 准备参数
//			Point point = { .X = 10, .Y = 20 };
//			int dx = 5, dy = -3;
//
//			printf("C: 调用前点位置: (%d, %d)\n", point.X, point.Y);
//
//			// 创建参数数组
//			void* args[3];
//			args[0] = &point; // ref 参数传递指针
//			args[1] = &dx;
//			args[2] = &dy;
//
//			// 调用方法
//			MONO_RUNTIME_INVOKE(ref_struct_method, NULL, args, NULL);
//			printf("C: 调用后点位置: (%d, %d)\n", point.X, point.Y);
//		}
//
//		// 示例4: 处理 out 结构体参数
//		printf("\n=== 示例4: out 结构体参数 ===\n");
//		{
//			// 查找方法
//			MonoMethod* out_struct_method;
//			out_struct_method = MONO_CLASS_GET_METHOD_FROM_NAME(exampleClass, "OutCreatePoint", 3);
//			if (!out_struct_method) {
//				fprintf(stderr, "未找到 OutCreatePoint 方法\n");
//				return 1;
//			}
//
//			// 准备参数
//			Point point; // 不需要初始化
//			int x = 100, y = 200;
//
//			// 创建参数数组
//			void* args[3];
//			args[0] = &point; // out 参数传递指针
//			args[1] = &x;
//			args[2] = &y;
//
//			// 调用方法
//			MONO_RUNTIME_INVOKE(out_struct_method, NULL, args, NULL);
//			printf("C: 创建的点: (%d, %d)\n", point.X, point.Y);
//		}
//	}
//
//	MONO_IMAGE_CLOSE(image);
//	//MONO_DOMAIN_UNLOAD(domain);
//	//MONO_JIT_CLEANUP(domain);
//	//domain = NULL;
//
//	return 0;
//}