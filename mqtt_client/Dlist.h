/*
 * Dlist.h
 *
 *  Created on: 2017-5-18
 *      Author: firefly
 */

#ifndef DLIST_H_
#define DLIST_H_
#include <sys/time.h>
#define	SENDUARTENABLE 1
#define SENDARTDISABLE 0
#define UARTDATEMAXSIZE	300
#define COMMTEMPSTRUCTSIZE	80

typedef unsigned char Item;//定义Date缓存的数据类型
typedef struct Node *PNode;//定义节点指针
typedef PNode Position;
typedef struct Node {
	Item data[UARTDATEMAXSIZE];
	struct timeval receivetime;
	PNode previous;
	PNode next;
}Node;//双向链表的节点定义
typedef struct {
	PNode head;
	PNode tail;
	int size;//可根据节点数*单个节点的数据量得到所需的内存，设置警报模式
}DList;//双向链表定义

extern DList *InitList(Item *i);//创建一个双向链表
extern Position MakeNode(Item *i);//创建一个节点
extern PNode DelFirst(DList *plist);//删除第一个节点
extern Position GetHead(DList *plist);//查找表头指针
extern Position GetTail(DList *plist);//查找表尾指针
extern PNode InsAfterTail(DList *plist, PNode s);//在表尾增加一个节点
extern PNode DelHeadNode(DList *plist);//删除头节点

typedef struct CommTempStruct {
	char time[COMMTEMPSTRUCTSIZE];
	char longitude[COMMTEMPSTRUCTSIZE];
	char laittude[COMMTEMPSTRUCTSIZE];
	char speed[COMMTEMPSTRUCTSIZE];
	char heading[COMMTEMPSTRUCTSIZE];
	char accel[COMMTEMPSTRUCTSIZE];
	char scheduleTime[COMMTEMPSTRUCTSIZE];
}SendServerStruct, SendUartStruct;

typedef SendServerStruct * pSendServerStruct;
typedef	SendUartStruct * pSendUartStruct;


#endif /* DLIST_H_ */
