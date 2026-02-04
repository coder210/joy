#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "external/klib/khash.h"
#include "external/klib/kbtree.h"
#include "external/klib/klist.h"
#include "external/klib/kvec.h"

////typedef const char* str_t;
////
//////kbitr_t itr;
////
//////
//////// 声明并初始化一个键为int，值为int的哈希表
//////KHASH_INIT(32, int, int, 1, kh_int_hash_func, kh_int_hash_equal)
//////
//////static void test_hash()
//////{
//////        khash_t(32)* h = kh_init(32); // 创建哈希表
//////        int ret;
//////
//////        // 插入键值对 key=1, value=100
//////        khint_t k = kh_put(32, h, 1, &ret);
//////        kh_val(h, k) = 100;
//////
//////        // 查找 key=1
//////        k = kh_get(32, h, 1);
//////        if (k != kh_end(h)) {
//////                printf("value: %d\n", kh_val(h, k)); // 输出: value: 100
//////        }
//////
//////        kh_destroy(32, h); // 销毁哈希表
//////        return 0;
//////}
//////
////typedef struct kv {
////        int key;
////        char value[512];
////}kv_t, *kv_p;
////
////
////// 定义比较函数（必须）
////static inline int kv_cmp(int a, int b) {
////        return (a > b) - (a < b);
////}
////
////// 声明一个键为整数、值为字符串的 B 树
////KBTREE_INIT(kint, int, kv_cmp)
////
////int main() {
////        kbtree_t(kint)* tree = kb_init(kint, KB_DEFAULT_SIZE); // 初始化 B 树
////
////        kv_t kv1, kv2, kv3;
////        kv1.key = 2;
////        strcpy(kv1.value, "test");
////        kv2.key = 3;
////        strcpy(kv2.value, "333");
////        kb_put(kint, tree, 2);
////        kb_put(kint, tree, 3);
////
////        int* t = kb_get_kint(tree, 4);
////        int k = *t;
////
////
////
////        // 查找键
////        //kv_p *value = kb_get(kint, tree, &kv2);
////        ////kv_p *value = kb_get(int, tree, &kv2);
////        //if (value) {
////        //        printf("2: %d\n", (*value)->key); // 输出: 2: two
////        //}
////
////        // 遍历所有键值对（按键的顺序）
////       /* khiter_t  itr;
////        kb_itr_first(int, tree, &itr);
////        do {
////                printf("%d: %s\n", kb_iter_key(int, &itr), kb_iter_value(char*, &itr));
////        } while (kb_iter_next(int, tree, &itr));*/
////        // 输出:
////        // 1: one
////        // 2: two
////        // 3: three
////
////        //kb_destroy(kv_t, tree); // 销毁 B 树
////        return 0;
////}
//
//
//typedef struct {
//        char* key;
//        int count;
//} elem_t;
//
//#define elem_cmp(a, b) (strcmp((a).key, (b).key))
//KBTREE_INIT(str, elem_t, elem_cmp)
//
//int main(int argc, char* argv[])
//{
//        kbtree_t(str)* b;
//        elem_t* p, t;
//
//        //kb_iter_t  itr;
//        //kbitr_t itr;
//        int i;
//        b = kb_init(str, KB_DEFAULT_SIZE);
//        for (i = 1; i < argc; ++i) {
//                // no need to allocate; just use pointer
//                t.key = argv[i], t.count = 1;
//                p = kb_getp(str, b, &t); // kb_get() also works
//                // IMPORTANT: put() only works if key is absent
//                if (!p) kb_putp(str, b, &t);
//                else ++p->count;
//        }
//
//        // ordered tree traversal
//        //kb_itr_first(str, b, &itr); // get an iterator pointing to the first
//        //for (; kb_itr_valid(&itr); kb_itr_next(str, b, &itr)) { // move on
//        //        p = &kb_itr_key(elem_t, &itr);
//        //        printf("%d\t%s\n", p->count, p->key);
//        //}
//        //kb_destroy(str, b);
//        return 0;
//}
//
//#define __int_free(x)
//KLIST_INIT(32, int, __int_free)
//
//int main()
//{
//	klist_t(32)* kl;
//	kliter_t(32)* p;
//
//	kl = kl_init(32);
//
//
//	*kl_pushp(32, kl) = 1;
//	*kl_pushp(32, kl) = 10;
//	kl_shift(32, kl, 0);
//	//kl_shift(32, kl, 0);
//	*kl_pushp(32, kl) = 10;
//
//	for (p = kl_begin(kl); p != kl_end(kl); p = kl_next(p))
//		printf("%d\n", kl_val(p));
//	kl_destroy(32, kl);
//	return 0;
//}



//static int main()
//{
//	/*int M = 10, N = 20000000, i, j;
//	clock_t t;
//	t = clock();
//	for (i = 0; i < M; ++i) {
//		int* array = (int*)malloc(N * sizeof(int));
//		for (j = 0; j < N; ++j) array[j] = j;
//		free(array);
//	}
//	printf("C array, preallocated: %.3f sec\n",
//		(float)(clock() - t) / CLOCKS_PER_SEC);
//	t = clock();
//	for (i = 0; i < M; ++i) {
//		int* array = 0, max = 0;
//		for (j = 0; j < N; ++j) {
//			if (j == max) {
//				max = !max ? 1 : max << 1;
//				array = (int*)realloc(array, sizeof(int) * max);
//			}
//			array[j] = j;
//		}
//		free(array);
//	}
//	printf("C array, dynamic: %.3f sec\n",
//		(float)(clock() - t) / CLOCKS_PER_SEC);
//	t = clock();
//	for (i = 0; i < M; ++i) {
//		kvec_t(int) array;
//		kv_init(array);
//		kv_resize(int, array, N);
//		for (j = 0; j < N; ++j) kv_A(array, i) = j;
//		kv_destroy(array);
//	}
//	printf("C vector, dynamic(kv_a): %.3f sec\n",
//		(float)(clock() - t) / CLOCKS_PER_SEC);
//	t = clock();
//	for (i = 0; i < M; ++i) {
//		kvec_t(int) array;
//		kv_init(array);
//		for (j = 0; j < N; ++j)
//			kv_push(int, array, j);
//		kv_destroy(array);
//	}
//	printf("C vector, dynamic(kv_push): %.3f sec\n",
//		(float)(clock() - t) / CLOCKS_PER_SEC);
//	t = clock();
//	printf("C++ vector, dynamic: %.3f sec\n",
//		(float)(clock() - t) / CLOCKS_PER_SEC);*/
//
//	kvec_t(int) array;
//	((array).n = (array).m = 0, (array).a = 0);
//	kv_push(int, array, 100);
//	kv_push(int, array, 200);
//	int len = kv_size(array);
//
//	kv_A(array, 100) = 20;
//
//	int value1 = kv_A(array, 0);
//	int value2 = kv_A(array, 2);
//	int value3 = kv_A(array, 12);
//
//	return 0;
//}
