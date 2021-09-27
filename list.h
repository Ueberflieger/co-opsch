#ifndef __LIST_H__
#define __LIST_H__

#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct list_s list_t;

typedef struct list_s
{
    list_t *next;
    list_t *prev;
} list_t;

static inline void list_add_tail(list_t *list, list_t *new)
{
    list->prev->next = new;
    new->prev = list->prev;
    list->prev = new;
    new->next = list;
}

static inline void list_remove(list_t *list)
{
    list->prev->next = list->next;
    list->next->prev = list->prev;
}

static inline void list_init(list_t *list)
{
    list->next = list;
    list->prev = list;
}

static inline list_t * list_head(list_t *list)
{
    return list->next;
}

static inline list_t * list_tail(list_t *list)
{
    return list->prev;
}

static inline int list_empty(list_t *list)
{
    return (list->next == list);
}

#endif
