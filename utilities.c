#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "utilities.h"

/* A stack of void pointers */
Stack stack_new(size_t initialsize)
{
    Stack s = (Stack) malloc(sizeof(*s));
    if (initialsize <= 0)
        initialsize = 256;
    s->allocated = initialsize;
    s->used = 0;
    s->buffer = (void**) malloc(sizeof(void*) * s->allocated);
    return s;
}

void stack_push(Stack s, void * p)
{
    if (s->used == s->allocated) {
        s->allocated *= 2;
        s->buffer = (void **) realloc(s->buffer, sizeof(void*) * s->allocated);
    }
    s->buffer[s->used] = p;
    s->used += 1;
}

void* stack_pop(Stack s)
{
    if (s->used == 0)
        return NULL;
    s->used -= 1;
    return s->buffer[s->used];
}

void stack_free(Stack s)
{
    free(s->buffer);
}

/* String is a growable string */
String string_new_empty()
{
    String s = (String) malloc(sizeof(*s));
    s->len = 0;
    s->size = 128;
    s->content = (char*) malloc(s->size);
    s->content[0] = '\0';
    return s;
}

String string_new_array(char *a)
{
    String s = (String) malloc(sizeof(*s));
    s->len = strlen(a);
    s->size = s->len + 1;
    s->content = strdup(a);
    return s;
}

void string_free(String s)
{
    free(s->content);
    free(s);
}

void string_grow(String s, size_t newsize)
{
    s->size = newsize;
    s->content = (char*) realloc(s->content, newsize);
}

void string_cat_array(String dst, const char *a)
{
    if (dst->len != 0) {
        string_printf(dst, " %s", a);
    } else {
        string_printf(dst, "%s", a);
    }
}

void string_printf(String dst, const char *fmt, ... )
{
    size_t len;
    va_list args;
    va_start(args, fmt);
    len = vsnprintf(0, 0, fmt, args);
    va_end(args);

    if (dst->len + len >= dst->size) {
        string_grow(dst, dst->size + len);
    }

    va_start(args, fmt);
    vsnprintf(dst->content + dst->len, len + 1, fmt, args);
    dst->len = dst->len + len;
    va_end(args);
}