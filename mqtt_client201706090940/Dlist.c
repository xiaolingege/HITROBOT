/*
 * Dlist.c
 *
 *  Created on: 2017-5-18
 *      Author: firefly
 */
#include "Dlist.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

DList *InitList(Item *i){
	DList *plist = (DList *)malloc(sizeof(DList));
	PNode head = MakeNode(i);
	if(plist!=NULL){
		if(head!=NULL){
			plist->head = head;
			plist->tail = head;
			plist->size = 1;
		}
		else
			return NULL;
	}
	return plist;
}

Position MakeNode(Item * i){
	PNode p = NULL;
	unsigned short count = 0;
	p = (PNode)malloc(sizeof(Node));
//	printf("save data is %s\n",i);
	if(p!=NULL){

		for(count = 0; count < UARTDATEMAXSIZE; count ++){
			p->data[count] = i[count];
			if(p->data[count] == '\0'){
				break;
			}

		}
		gettimeofday(&p->receivetime, 0);
		p->previous = NULL;
		p->next = NULL;
	}
	return p;
}

PNode DelFirst(DList *plist){
	Position head = GetHead(plist);
	Position p = head->next;
	 if(p!=NULL){
		if(p == GetTail(plist)){
			plist->tail = p->previous;
			head->next = p->next;
			head->next->previous = head;
			plist->size--;
		}
	}
	return p;
}

Position GetHead(DList *plist){
	return plist->head;
}

Position GetTail(DList *plist){
	return plist->tail;
}

PNode InsAfterTail(DList *plist, PNode s){
	Position p = GetTail(plist) ;
	s->next = p->next;
	s->previous = p;
	if(p->next!=NULL){
		p->next->previous = s;
	}
	p->next = s;
	plist->tail = s;
	plist->size++;
	return s;
}

PNode DelHeadNode(DList *plist){
	Position p =NULL;
	p = GetHead(plist);
	p->next->previous = p->previous;
	plist->head = p->next;
	plist->size --;
	return p;
}
