// LRU cache implementation

#include "PageQueue.h"
//#include "Vector.h"


//fungsi untuk menginisialsiasi QNode
QNode * initQNode (int no_page) {
	QNode* temp = (QNode *) malloc(sizeof(QNode));
	temp->no_page = no_page;
	//inisialisasi prev dan next dengan NULL
	temp->prev = temp->next = NULL;

	return temp;
}

//fungsi untuk membuat Queue kosong
//elemen yang dimiliki wueue maksimal adalah sebanyak totalFrame

Queue * createQueue(int totalFrame) {
	Queue * queue = (Queue *) malloc (sizeof(Queue));

	//queue kosong
	queue->counter=0;
	queue->front = queue->rear = NULL;

	//banyaknya frame yang dapat ditampung
	queue->totalFrame = totalFrame;

	return queue;
}

//fungsi untuk membuat hash dengan kapasitas yang diberikan
/*
Hash * createHash(int capacity) {
	//Alokasi memory untuk hash
	Hash * hash = (Hash *) malloc (sizeof(Hash));
	hash->capacity = capacity;

	//membuat array of pointers yang menunjuk ke node-node queue

	hash->array=(QNode **) malloc (hash->capacity) * sizeof(QNode*));

	//inisialisasi semuanya kosong
	int i;
	for(i =0; i <hash->capacity; i++) {
		hash->array[i] = NULL;
	}

	return hash;
} */

//fungsi untuk mengecek apakah masih tersedia slot di memory

boolean isFrameFull(Queue * queue) {
	if (queue->counter == queue->totalFrame) {
		return true;
	} else
		return false;
}

//fungsi untuk mengecek apakah queue kosong

boolean isFrameEmpty(Queue * queue) {
	if (queue->rear == NULL) {
		return true;
	} else
		return false;
}

//prosedur menghapus frame dari queue

void delFromFrame (Queue * queue) {
	if (isFrameEmpty(queue)) {
		return;
	}

	//if one element
	if (queue->front == queue->rear) {
		queue->front = NULL;
	}

	//ubah rear menjadi previous rear dan buang rear yang sebelumnya
	QNode * temp = queue->rear;
	int simpan = queue->rear->no_page; //menyimpan indeks victim
	queue->rear = queue->rear->prev;

	if (queue->rear) {
		queue->rear->next = NULL;
	}

		printf("Choose a victim page %d\n", simpan);
		printf("Victim is dirty, write out\n");
		
		
		printf("Put in victim's frame %d\n", dataPage[simpan].Frame);	//selalu dipilih frame 0, karena queue
		frameVictim = dataPage[simpan].Frame;
		dataPage[simpan].Valid = 0; //victim di invalid, menyatakan bahwa sudah tidak ada di memory fisik
		dataPage[simpan].Frame = -1; //tidak ada diframe manapun

		//	   	+ Jika page ‘victim’ tersebut memiliki dirty bernilai true simulasikan penulisan pada disk, lakukan 
		//		  sleep(1) dan tambahkan nilai pengaksesan pada disk.
		//        .. code ..
		if (dataPage[simpan].Dirty == true) {
			dataPage[simpan].Dirty = 0; 
			aksesDisk++;
			sleep(1); //menuliskan ke disk	
		}


	

	//kurangi jumlah frame
	queue->counter--;
}


void addToFrame (Queue * queue, int no_page) {
	//kalau penuh, hapus yang paling depan
	//# 3. Jika terdapat frame yang tidak ditempati, alokasikan.	
	//# 4. Jika tidak ada seluruh frame ditempati, pilih page ‘victim’ yang akan di swap.
	

	if (!dataPage[no_page].Valid) { //page tidak ada di frame, maka hapus victim 
		boolean penuh=false;
		if (isFrameFull(queue)) {
			delFromFrame(queue);
			penuh = true;
		}  
		//kalau dia blm ada di frame, siapkan node baru
		//buat node baru, lalu tambah di depan queue
		QNode * temp = initQNode(no_page);
		temp->next = queue->front;

		//kalau queue kosong, ubah queue bagian depan dan belakang
		if(isFrameEmpty(queue)) {
			queue->rear = queue->front = temp;
		} else { //ubah depan
			queue->front->prev = temp;
			queue->front = temp;
		}

		//add page ke hash
		//hash->array[no_page] = temp;
		//tambah jumlah element
		dataPage[no_page].Valid = 1;
		if (penuh) {
			dataPage[no_page].Frame = frameVictim;
		} else {
			printf("Put it in free frame %d\n", queue->counter);
			dataPage[no_page].Frame = queue->counter;
		}	
	
		aksesDisk++;
		sleep(1); //simulasi akses disk

		queue->counter++;
	} else { //jika page memiliki frame, maka hanya akses (read)
		printf("It is in ram dude\n");
		aksesDisk++;
		sleep(1);
	}	
} 

/*void insertPage (Queue * queue, Hash * hash, unsgined no_page) {
	QNode * requestedPage = hash->array[no_page];

	//page tidak ada di cache/memory/frame, maka insert
	
	if (requestedPage == NULL) {
		addToFrame(queue, hash, no_page);
	} else { //pagenya ada, tapi bukan di depan
		if (requestedPage != queue->front)
			requestedPage->prev->next = requestedPage->next;
			if (requestedPage->next) {
				requestedPage->next->prev = requestedPage->prev;
			}

			//kalau pagenya ada di belakang, maka nodenya dipindah ke depan
			
			if (requestedPage == queue->rear) {
				queue->rear = requestedPage->page;
				queue->rear->next = NULL;
			}

			//pindahkan page yang diminta ke sebelum depan (paling depan)
			requestedPage->next = queue->front;
			requestedPage->prev = NULL;

			//ubah previous dari queue paling depan yang sekarang
			requestedPage->next->prev = requestedPage;

			//ubah depan ke reqpage
			queue->front = requestedPage;

	}
} */