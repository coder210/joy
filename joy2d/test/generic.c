//#include <stdio.h>
//#include <stdbool.h>
//#include <stdlib.h>
//
//// 1. 基本宏函数 - 求平方
//#define SQUARE(x) ((x) * (x))
//
//// 2. 多行宏函数 - 交换两个值
//#define SWAP(type, a, b) do { \
//    type temp = a; \
//    a = b; \
//    b = temp; \
//} while(0)
//
//// 3. 带条件的宏函数 - 求最大值
//#define MAX(a, b) ((a) > (b) ? (a) : (b))
//
//// 4. 字符串化操作 - 将参数转换为字符串
//#define STRINGIFY(x) #x
//
//// 5. 连接操作 - 连接两个标记
//#define CONCAT(a, b) a##b
//
//
//
//#define QUEUE_INIT(name, type) \
//typedef struct queue_node_##name { \
//    type data; \
//    struct queue_node_##name* next; \
//}queue_node_##name##_t; \
//\
//typedef int (*queue_node_##name##_free_fn)(type data); \
//\
//typedef struct linked_queue_##name { \
//    queue_node_##name##_t* front; \
//    queue_node_##name##_t* rear; \
//    queue_node_##name##_free_fn free_fn; \
//}linked_queue_##name##_t; \
//\
//static void \
//linked_queue_##name##_init(linked_queue_##name##_t* q, queue_node_##name##_free_fn fn) \
//{ \
//    q->front = q->rear = NULL; \
//    q->free_fn = fn; \
//} \
//\
//static bool \
//linked_queue_##name##_isempty(linked_queue_##name##_t* q) \
//{\
//    return (q->front == NULL);\
//}\
//\
//static int\
//linked_queue_##name##_length(linked_queue_##name##_t* q) \
//{ \
//    int count; \
//    queue_node_##name##_t* iter; \
//    iter = q->front; \
//    count = 0; \
//    while (iter) { \
//        count++; \
//        iter = iter->next; \
//    } \
//    return count; \
//} \
//\
//static void \
//linked_queue_##name##_enqueue(linked_queue_##name##_t* q, type value) \
//{ \
//    queue_node_##name##_t* new_node; \
//    new_node = (queue_node_##name##_t*)malloc(sizeof(queue_node_##name##_t)); \
//    assert(new_node); \
//    new_node->data = value; \
//    new_node->next = NULL; \
//    if (linked_queue_##name##_isempty(q)) { \
//        q->front = q->rear = new_node; \
//    } \
//    else { \
//        q->rear->next = new_node; \
//        q->rear = new_node; \
//    } \
//} \
//\
//static type \
//linked_queue_##name##_dequeue(linked_queue_##name##_t* q) \
//{ \
//    queue_node_##name##_t* front = q->front; \
//    if (!front) \
//        return front; \
//    type front_data = front->data; \
//    q->front = q->front->next; \
//    if (!q->front) \
//        q->rear = NULL; \
//    free(front); \
//    return front_data; \
//}\
//\
//static type \
//linked_queue_##name##_front(linked_queue_##name##_t* q) \
//{ \
//    if (!linked_queue_##name##_isempty(q)) \
//        return q->front->data; \
//    return NULL; \
//} \
//\
//static void \
//linked_queue_##name##_release(linked_queue_##name##_t* q) \
//{ \
//    while (!linked_queue_##name##_isempty(q)) { \
//        type value = linked_queue_##name##_dequeue(q); \
//        if (q->free_fn) q->free_fn(value); \
//    } \
//} \
//\
//static int queue_node_##name##_free(type data) \
//{ \
//    free(data);\
//    return 0;\
//}\
//
//#define linked_queue_is_emtpty(name, q) do { \
//	linked_queue_##name##_isempty(q); \
//}while(0)
//
//
//
//QUEUE_INIT(test, int)
//
//
//
//int main(int argc, char** argv)
//{
//	// 使用SQUARE宏
//	int num = 5;
//	printf("%d的平方是: %d\n", num, SQUARE(num));
//
//	// 使用SWAP宏
//	int x = 10, y = 20;
//	printf("交换前: x=%d, y=%d\n", x, y);
//	SWAP(int, x, y);
//	printf("交换后: x=%d, y=%d\n", x, y);
//
//	// 使用MAX宏
//	printf("%d和%d中的最大值是: %d\n", x, y, MAX(x, y));
//
//	// 使用STRINGIFY宏
//	printf("变量名: %s\n", STRINGIFY(num));
//
//	// 使用CONCAT宏
//	int value1 = 100;
//	int value2 = 200;
//	printf("连接后的值: %d\n", CONCAT(value, 1)); // 输出value1的值
//
//	linked_queue_test_t t;
//	linked_queue_is_emtpty(test, &t);
//
//
//
//
//
//
//	return 0;
//}


