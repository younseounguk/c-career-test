#ifndef _LIST_FUNC_H
#define _LIST_FUNC_H

#include "list.h"

#define list_entry(ptr, type, member) ({                      \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})

#define list_for_each(pos, n, head) \
	for (pos = (head)->next, n = (pos)->next; \
			pos != (head); pos = n, n = (pos)->next)

extern void initList(list_t *list);
extern void addList(list_t *new, list_t *head);
extern void _addList(list_t *new, list_t *prev, list_t *next);
extern void delList(list_t *entry);
extern void _delList(list_t *prev, list_t *next);
extern int isEmptyList(list_t *head);

inline void initList(list_t *list) {
	if (NULL == list)
		return;

	list->prev = list;
	list->next = list;
}

inline void _addList(list_t *new, list_t *prev, list_t *next) {
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

inline void addList(list_t *new, list_t *head) {
	_addList(new, head->prev, head);
}

inline void _delList(list_t *prev, list_t *next) {
	next->prev = prev;
	prev->next = next;
}

inline void delList(list_t *entry) {
	_delList(entry->prev, entry->next);
	initList(entry);
}

inline int isEmptyList(list_t *head) {
	return head->next == head ? 1 : 0;
}

#endif
