//#include <stdio.h>
//#include <stdlib.h>
//#include "./external/mujs/mujs.h"
//
//static void hello(js_State* J)
//{
//	const char* name = js_tostring(J, 1);
//	printf("Hello, %s!\n", name);
//	js_pushundefined(J);
//}
//
//int main(int argc, char** argv)
//{
//	js_State* J = js_newstate(NULL, NULL, JS_STRICT);
//
//	js_newcfunction(J, hello, "hello", 1);
//	js_setglobal(J, "hello");
//
//	js_dostring(J, "var a=10;\
// function test(){hello('test')};\
//if (a==10){\
//        hello('true');\
//}else{\
//hello('false');\
//}\
// var b={};\
// b.k=20; hello(a+''+b.k); test();");
//
//	js_freestate(J);
//}