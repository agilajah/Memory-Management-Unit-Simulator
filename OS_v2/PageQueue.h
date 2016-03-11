#ifndef PAGE_QUEUE_H
#define PAGE_QUEUE_H

#include "Boolean.h"
#include "PageTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
//#include <Vector.h>


// LRU cache implementation

// Node untuk Queue dengan implementasi double linked list
typedef struct QNode {
	struct QNode *prev, *next;
	int no_page;
} QNode;

// Queue (FIFO, kumpulan dari node-node)
typedef struct Queue {
	unsigned counter;
	unsigned totalFrame;
	QNode *front, *rear;
} Queue;

/*
// Hash, kumpulan pointer yang menunjuk ke node-node queue
typedef struct Hash {
	int capacity; //kapasitas yang dapat ditampung (banyak frame yang tersedia)
	QNode* *array;	//array of queue nodes
} Hash; */

//fungsi untuk menginisialsiasi QNode
QNode * initQNode (int no_page);
//fungsi untuk membuat Queue kosong
//elemen yang dimiliki queue maksimal adalah sebanyak totalFrame
Queue * createQueue(int totalFrame);

/*
//fungsi untuk membuat hash dengan kapasitas yang diberikan
Hash * createHash(int capacity) ; */

//fungsi untuk mengecek apakah masih tersedia slot di memory
boolean isFrameFull(Queue * queue);

//fungsi untuk mengecek apakah queue kosong
boolean isFrameEmpty(Queue * queue) ;

//prosedur menghapus frame dari queue
void delFromFrame (Queue * queue) ;

//prosedur menambah frame ke queue
void addToFrame (Queue * queue, int no_page);

/*void insertPage (Queue * queue, Hash * hash, unsgined no_page); */

//Variable Global
//Variable Global
int banyakPage;
int banyakFrame;
page_table_pointer dataPage;
int statusOS;
int aksesDisk;
int frameVictim; //untuk mengingat di frame mana victim dipilih, akan dipakai untuk proses selanjutnya
Queue * que;
//Vector vectorFrame;

#endif