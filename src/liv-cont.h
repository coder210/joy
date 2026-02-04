/************************************************
Copyright: 2021-2028, lanchong.xyz/Ltd.
File name: liv-cont.h
Description: 数据结构
Author: ydlc
Version: 1.0
Date: 2021.11.14
History:
*************************************************/
#ifndef LIVNET_CONT_H
#define LIVNET_CONT_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "liv-config.h"


#define RED		0
#define BLACK	        1

#define rb_parent(r)   ((r)->parent)
#define rb_color(r) ((r)->color)
#define rb_is_red(r)   ((r)->color==RED)
#define rb_is_black(r)  ((r)->color==BLACK)
#define rb_set_black(r)  do { (r)->color = BLACK; } while (0)
#define rb_set_red(r)  do { (r)->color = RED; } while (0)
#define rb_set_parent(r,p)  do { (r)->parent = (p); } while (0)
#define rb_set_color(r,c)  do { (r)->color = (c); } while (0)


typedef int (*queue_node_free_fn)(void* data);
typedef int (*list_node_free_fn)(void* data);
typedef int (*map_node_free_fn)(void* key, void* value);


typedef struct queue_node {
        void* data;
        struct queue_node* next;
}queue_node_t;

typedef struct list_node {
        void* data;
        struct list_node* next;
}list_node_t;

typedef struct map_node {
        unsigned char color;
        void* key;
        void* data;
        struct map_node* left;
        struct map_node* right;
        struct map_node* parent;
}map_node_t;


typedef struct linked_queue {
        queue_node_t* front;
        queue_node_t* rear;
        queue_node_free_fn free_fn;
}linked_queue_t;


typedef struct linked_list {
        list_node_t* head;
        list_node_t* rear;
        list_node_free_fn free_fn;
}linked_list_t;


typedef struct vector {
        void* data;
        size_t size;
        size_t capacity;
        size_t elem_size;
}vector_t;


typedef struct map {
        map_node_t* root;
        map_node_free_fn free_fn;
}map_t;


static int
default_node_free(void* data)
{
        free(data);
        return 0;
}


static void
linked_queue_init(linked_queue_t* q, queue_node_free_fn fn)
{
        q->front = q->rear = NULL;
        q->free_fn = fn;
}

static bool
linked_queue_isempty(linked_queue_t* q)
{
        return (q->front == NULL);
}

static int
linked_queue_length(linked_queue_t* q)
{
        int count;
        queue_node_t* iter;
        iter = q->front;
        count = 0;
        while (iter) {
                count++;
                iter = iter->next;
        }
        return count;
}

static void
linked_queue_enqueue(linked_queue_t* q, void* value)
{
        queue_node_t* new_node;
        new_node = (queue_node_t*)malloc(sizeof(queue_node_t));
        assert(new_node);
        new_node->data = value;
        new_node->next = NULL;
        if (linked_queue_isempty(q)) {
                q->front = q->rear = new_node;
        }
        else {
                q->rear->next = new_node;
                q->rear = new_node;
        }
}

static void*
linked_queue_dequeue(linked_queue_t* q)
{
        queue_node_t* front = q->front;
        if (!front)
                return front;
        void* front_data = front->data;
        q->front = q->front->next;
        if (!q->front)
                q->rear = NULL;
        free(front);
        return front_data;
}

static void*
linked_queue_front(linked_queue_t* q)
{
        if (!linked_queue_isempty(q))
                return q->front->data;
        return NULL;
}

static void
linked_queue_release(linked_queue_t* q)
{
        while (!linked_queue_isempty(q)) {
                void* value = linked_queue_dequeue(q);
                q->free_fn&& q->free_fn(value);
        }
}


static list_node_t*
list_node_next(list_node_t* node)
{
        assert(node);
        return node->next;
}

static void*
list_node_getdata(list_node_t* node)
{
        assert(node);
        return node->data;
}

static bool
linked_list_init(linked_list_t* list, list_node_free_fn fn)
{
        assert(list);
        list->head = NULL;
        list->rear = NULL;
        list->free_fn = fn;
        return true;
}

static void
linked_list_release(linked_list_t* list)
{
        list_node_t* iter = list->head;
        while (iter) {
                list_node_t* tmp = iter;
                iter = iter->next;
                list->free_fn&& list->free_fn(tmp->data);
                free(tmp);
                tmp = NULL;
        }
        list->head = list->rear = NULL;
}

static list_node_t*
linked_list_find(linked_list_t* list, const void* data)
{
        list_node_t* iter = list->head;
        while (iter) {
                if (iter->data == data)
                        return iter;
                iter = iter->next;
        }
        return NULL;
}

static list_node_t*
linked_list_first(linked_list_t* list)
{
        assert(list);
        return list->head;
}

static list_node_t*
linked_list_end(linked_list_t* list)
{
        assert(list);
        return list->rear;
}

static int
linked_list_length(linked_list_t* list)
{
        int count = 0;
        list_node_t* iter = list->head;
        while (iter) {
                count++;
                iter = iter->next;
        }
        return count;
}

static bool
linked_list_add(linked_list_t* list, void* data)
{
        list_node_t* new_node = (list_node_t*)malloc(sizeof(list_node_t));
        assert(new_node);
        new_node->data = data;
        new_node->next = NULL;
        if (list->head)
                list->rear->next = list->rear = new_node;
        else
                list->head = list->rear = new_node;
        return true;
}

static bool
linked_list_remove(linked_list_t* list, const void* data)
{
        if (!list->head)
                return false;
        if (list->head->data == data) {
                list_node_t* tmp = list->head;
                list->head = list->head->next;
                free(tmp);
                return true;
        }

        list_node_t* prev = list->head;
        list_node_t* iter = list->head->next;
        while (iter) {
                if (iter->data == data) {
                        list_node_t* tmp = iter;
                        prev->next = iter->next;
                        free(tmp);
                        iter = NULL;
                        return true;
                }
                iter = iter->next;
        }
        return false;
}

static bool
vector_init(vector_t* vec, size_t elem_size, size_t capacity)
{
        assert(vec);
        if (elem_size == 0 || capacity == 0)
                return false;
        vec->data = malloc(elem_size * capacity);
        if (!vec->data)
                return false;
        vec->size = 0;
        vec->capacity = capacity;
        vec->elem_size = elem_size;
        return true;
}

static void
vector_release(vector_t* vec)
{
        assert(vec);
        free(vec->data);
        vec->data = NULL;
        vec->size = vec->elem_size = 0;
        vec->capacity = 0;
}

static bool
vector_resize(vector_t* vec, size_t new_capacity)
{
        void* new_data = realloc(vec->data, new_capacity * vec->elem_size);
        assert(!new_data);
        vec->data = new_data;
        vec->capacity = new_capacity;
        return true;
}

static bool
vector_pushback(vector_t* vec, const void* elem)
{
        if (vec->size >= vec->capacity) {
                if (!vector_resize(vec, vec->capacity * 2))
                        return false;
        }
        void* dest = (char*)vec->data + vec->size * vec->elem_size;
        memcpy(dest, elem, vec->elem_size);
        vec->size++;
        return true;
}

static bool
vector_popback(vector_t* vec)
{
        if (vec->size == 0)
                return false;
        vec->size--;
        return true;
}

static void*
vector_at(vector_t* vec, size_t index)
{
        if (index >= vec->size)
                return NULL;
        return (char*)vec->data + index * vec->elem_size;
}

static bool
vector_insert(vector_t* vec, size_t index, const void* elem)
{
        if (index > vec->size)
                return false;
        if (vec->size >= vec->capacity) {
                if (vector_resize(vec, vec->capacity * 2) != 0) {
                        return false;
                }
        }
        char* dest = (char*)vec->data + index * vec->elem_size;
        size_t move_size = (vec->size - index) * vec->elem_size;
        if (move_size > 0)
                memmove(dest + vec->elem_size, dest, move_size);
        memcpy(dest, elem, vec->elem_size);
        vec->size++;
        return true;
}

static bool
vector_erase(vector_t* vec, size_t index)
{
        if (index >= vec->size)
                return false;
        char* dest = (char*)vec->data + index * vec->elem_size;
        size_t move_size = (vec->size - index - 1) * vec->elem_size;
        if (move_size > 0)
                memmove(dest, dest + vec->elem_size, move_size);
        vec->size--;
        return true;
}


static void
_rbtree_destroy(map_node_t* node, map_node_free_fn fn)
{
        if (node == NULL)
                return;

        if (node->left != NULL)
                _rbtree_destroy(node->left, fn);
        if (node->right != NULL)
                _rbtree_destroy(node->right, fn);

        fn&& fn(node->key, node->data);
        free(node);
}

static map_node_t*
_search(map_node_t* x, void* key) {
        if (x == NULL || x->key == key)
                return x;

        if (key < x->key)
                return _search(x->left, key);
        else
                return _search(x->right, key);
}

static map_node_t*
_iterative_search(map_node_t* x, void* key)
{
        while ((x != NULL) && (x->key != key))
        {
                if (key < x->key)
                        x = x->left;
                else
                        x = x->right;
        }

        return x;
}

static map_node_t*
_create_rbtree_node(void* key, void* data,
        map_node_t* parent, map_node_t* left, map_node_t* right)
{
        map_node_t* p;

        if ((p = (map_node_t*)malloc(sizeof(map_node_t))) == NULL)
                return NULL;
        p->key = key;
        p->data = data;
        p->left = left;
        p->right = right;
        p->parent = parent;
        p->color = BLACK;

        return p;
}


static void
_rbtree_left_rotate(map_t* map, map_node_t* x)
{
        map_node_t* y = x->right;

        x->right = y->left;
        if (y->left != NULL)
                y->left->parent = x;

        y->parent = x->parent;

        if (x->parent == NULL) {
                map->root = y;
        }
        else {
                if (x->parent->left == x)
                        x->parent->left = y;
                else
                        x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
}


static void
_rbtree_right_rotate(map_t* map, map_node_t* y)
{
        map_node_t* x = y->left;
        y->left = x->right;
        if (x->right != NULL)
                x->right->parent = y;
        x->parent = y->parent;

        if (y->parent == NULL) {
                map->root = x;
        }
        else {
                if (y == y->parent->right)
                        y->parent->right = x;
                else
                        y->parent->left = x;
        }

        x->right = y;
        y->parent = x;
}


static void
_rbtree_insert_fixup(map_t* map, map_node_t* node)
{
        map_node_t* parent, * gparent;
        while ((parent = rb_parent(node)) && rb_is_red(parent)) {
                gparent = rb_parent(parent);
                if (parent == gparent->left) {
                        map_node_t* uncle = gparent->right;
                        if (uncle && rb_is_red(uncle))
                        {
                                rb_set_black(uncle);
                                rb_set_black(parent);
                                rb_set_red(gparent);
                                node = gparent;
                                continue;
                        }

                        if (parent->right == node) {
                                map_node_t* tmp;
                                _rbtree_left_rotate(map, parent);
                                tmp = parent;
                                parent = node;
                                node = tmp;
                        }

                        rb_set_black(parent);
                        rb_set_red(gparent);
                        _rbtree_right_rotate(map, gparent);
                }
                else {
                        map_node_t* uncle = gparent->left;
                        if (uncle && rb_is_red(uncle))
                        {
                                rb_set_black(uncle);
                                rb_set_black(parent);
                                rb_set_red(gparent);
                                node = gparent;
                                continue;
                        }

                        if (parent->left == node)
                        {
                                map_node_t* tmp;
                                _rbtree_right_rotate(map, parent);
                                tmp = parent;
                                parent = node;
                                node = tmp;
                        }

                        rb_set_black(parent);
                        rb_set_red(gparent);
                        _rbtree_left_rotate(map, gparent);
                }
        }

        rb_set_black(map->root);
}


static void
_rbtree_insert(map_t* map, map_node_t* node)
{
        map_node_t* y = NULL;
        map_node_t* x = map->root;

        while (x != NULL) {
                y = x;
                if (node->key < x->key)
                        x = x->left;
                else
                        x = x->right;
        }
        rb_parent(node) = y;

        if (y != NULL) {
                if (node->key < y->key)
                        y->left = node;
                else
                        y->right = node;
        }
        else {
                map->root = node;
        }

        node->color = RED;
        _rbtree_insert_fixup(map, node);
}


static void
_rbtree_delete_fixup(map_t* map, map_node_t* node, map_node_t* parent)
{
        map_node_t* other;
        while ((!node || rb_is_black(node)) && node != map->root)
        {
                if (parent->left == node)
                {
                        other = parent->right;
                        if (rb_is_red(other))
                        {
                                rb_set_black(other);
                                rb_set_red(parent);
                                _rbtree_left_rotate(map, parent);
                                other = parent->right;
                        }
                        if ((!other->left || rb_is_black(other->left)) &&
                                (!other->right || rb_is_black(other->right)))
                        {
                                rb_set_red(other);
                                node = parent;
                                parent = rb_parent(node);
                        }
                        else
                        {
                                if (!other->right || rb_is_black(other->right))
                                {
                                        rb_set_black(other->left);
                                        rb_set_red(other);
                                        _rbtree_right_rotate(map, other);
                                        other = parent->right;
                                }
                                rb_set_color(other, rb_color(parent));
                                rb_set_black(parent);
                                rb_set_black(other->right);
                                _rbtree_left_rotate(map, parent);
                                node = map->root;
                                break;
                        }
                }
                else
                {
                        other = parent->left;
                        if (rb_is_red(other))
                        {
                                rb_set_black(other);
                                rb_set_red(parent);
                                _rbtree_right_rotate(map, parent);
                                other = parent->left;
                        }
                        if ((!other->left || rb_is_black(other->left)) &&
                                (!other->right || rb_is_black(other->right)))
                        {
                                rb_set_red(other);
                                node = parent;
                                parent = rb_parent(node);
                        }
                        else
                        {
                                if (!other->left || rb_is_black(other->left))
                                {
                                        rb_set_black(other->right);
                                        rb_set_red(other);
                                        _rbtree_left_rotate(map, other);
                                        other = parent->left;
                                }
                                rb_set_color(other, rb_color(parent));
                                rb_set_black(parent);
                                rb_set_black(other->left);
                                _rbtree_right_rotate(map, parent);
                                node = map->root;
                                break;
                        }
                }
        }
        if (node)
                rb_set_black(node);
}



static void
_rbtree_delete(map_t* map, map_node_t* node)
{
        map_node_t* child, * parent;
        int color;

        if ((node->left != NULL) && (node->right != NULL))
        {
                map_node_t* replace = node;
                replace = replace->right;
                while (replace->left != NULL)
                        replace = replace->left;
                if (rb_parent(node))
                {
                        if (rb_parent(node)->left == node)
                                rb_parent(node)->left = replace;
                        else
                                rb_parent(node)->right = replace;
                }
                else
                        map->root = replace;

                child = replace->right;
                parent = rb_parent(replace);
                color = rb_color(replace);
                if (parent == node)
                {
                        parent = replace;
                }
                else
                {
                        if (child)
                                rb_set_parent(child, parent);
                        parent->left = child;

                        replace->right = node->right;
                        rb_set_parent(node->right, replace);
                }

                replace->parent = node->parent;
                replace->color = node->color;
                replace->left = node->left;
                node->left->parent = replace;

                if (color == BLACK)
                        _rbtree_delete_fixup(map, child, parent);
                free(node);

                return;
        }

        if (node->left != NULL)
                child = node->left;
        else
                child = node->right;

        parent = node->parent;
        color = node->color;

        if (child)
                child->parent = parent;

        if (parent)
        {
                if (parent->left == node)
                        parent->left = child;
                else
                        parent->right = child;
        }
        else
                map->root = child;

        if (color == BLACK)
                _rbtree_delete_fixup(map, child, parent);
        free(node);
}



static bool 
map_init(map_t* map, map_node_free_fn fn)
{
        assert(map);
        map->root = NULL;
        map->free_fn = fn;
        return true;
}

static void 
map_release(map_t* map)
{
        assert(map);
        _rbtree_destroy(map->root, map->free_fn);
}

static int
map_insert(map_t* map, void* key, void* data)
{
        map_node_t* node;
        if (_search(map->root, key) != NULL)
                return -1;
        node = _create_rbtree_node(key, data, NULL, NULL, NULL);
        if (node == NULL)
                return -1;
        _rbtree_insert(map, node);
        return 0;
}

static int
map_set(map_t* map, void* key, void* data)
{
        map_node_t* node = _search(map->root, key);
        if (node != NULL) {
                node->data = data;
                return 0;
        }
        return -1;
}

static void*
map_remove(map_t* map, void* key)
{
        map_node_t* z = _search(map->root, key);
        if (!z)
                return NULL;
        void* data = z->data;
        _rbtree_delete(map, z);
        return data;
}

static void*
map_search(map_t* map, void* key)
{
        map_node_t* node = _search(map->root, key);
        if (node)
                return node->data;
        return NULL;
}

static void*
map_isearch(map_t* map, void* key)
{
        assert(map);
        map_node_t* node = _iterative_search(map->root, key);
        if (node)
                return node->data;
        return NULL;
}

static map_node_t* 
map_first(const map_t* map)
{
        map_node_t* n = map->root;
        if (!n)
                return NULL;
        while (n->left)
                n = n->left;
        return n;
}

static map_node_t* 
map_last(const map_t* map)
{
        map_node_t* n = map->root;
        if (!n)
                return NULL;
        while (n->right)
                n = n->right;
        return n;
}

static map_node_t* 
map_trev_node(map_node_t* node)
{
        map_node_t* parent;
        if (rb_parent(node) == node)
                return NULL;

        if (node->left) {
                node = node->left;
                while (node->right)
                        node = node->right;
                return node;
        }

        while ((parent = rb_parent(node)) && node == parent->left)
                node = parent;

        return parent;
}

static map_node_t* 
map_next_node(map_node_t* node)
{
        map_node_t* parent;

        if (rb_parent(node) == node)
                return NULL;

        if (node->right) {
                node = node->right;
                while (node->left)
                        node = node->left;
                return node;
        }

        while ((parent = rb_parent(node)) && node == parent->right)
                node = parent;

        return parent;
}

static void*
map_node_getkey(const map_node_t* node)
{
        if (node)
                return node->key;
        return NULL;
}

static void*
map_node_getvalue(const map_node_t* node)
{
        if (node)
                return node->data;
        return NULL;
}

static void
map_node_setvalue(map_node_t* node, void* data)
{
        node->data = data;
}



#endif // !LIVNET_CONT_H
