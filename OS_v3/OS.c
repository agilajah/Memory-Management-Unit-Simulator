/* Tugas Besar OS 1
   Author : Febi Agil Ifdillah (13514010)
   			Harry Octavianus Purba
   			Rio Chandra Rajaguguk
   			

   Compile :
   			$ gcc -o OS OS.c Vector.c

   Run (example):
   			$ OS  5 3
   				The shared memory key (PID) is 16231
				Initialized page table

   			$ MMU 5  W2 R3 W3 R4 16231

   					.....

   Result : 

  			The MMU has finished
 			0: Valid=0 Frame=-1 Dirty=0 Requested=0
 			1: Valid=0 Frame=-1 Dirty=0 Requested=0
 			2: Valid=1 Frame= 0 Dirty=1 Requested=0
 			3: Valid=1 Frame= 1 Dirty=1 Requested=0
 			4: Valid=1 Frame= 2 Dirty=0 Requested=0
			3 disk accesses required
			
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "PageTable.h"
#include "Vector.h" //menggunakan dynamic array dengan nama vector
#include "Boolean.h"



//Variable Global
int banyakPage;
int banyakFrame;
page_table_pointer dataPage;
int statusOS = 0;
int aksesDisk = 0;
int frameVictim;
Vector vectorFrame;

//-----------------------------------------------------------------------------
void PrintPageTable(page_table_entry PageTable[],int NumberOfPages) {
    int Index;

    for (Index =  0;Index < NumberOfPages;Index++) {
        printf("%2d: Valid=%1d Frame=%2d Dirty=%1d Requested=%1d\n",Index, PageTable[Index].Valid,
        	PageTable[Index].Frame,PageTable[Index].Dirty, PageTable[Index].Requested);
    }
}

/*
	Sinopsis
		int alokasiSharedMemory(key_t kunci, int ukuran);

	Nilai Kembali
		Jika alokasi berhasil, dikembalikan id shared memory, jika gagal -1.
*/
int alokasiSharedMemory(key_t kunci, int ukuran){
	// ID Shared Memory yang akan dialokasi
    int shmid;

    // Melakukan alokasi Shared Memori jika memungkinkan dan mengehubungkannya
    if ((shmid = shmget(kunci, ukuran, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        return -1;
    }

    // Menempatkan (attaching) Shared Memori yang telah dialokasi dan 
    // dihubungkan diatas pada memori (RAM) dan alamat memori disimpan 
    // pada variabel dataPage.
    dataPage = (page_table_pointer) shmat(shmid, NULL, 0);
    if (dataPage == (page_table_pointer)(-1)) {
        perror("shmat");
        return -1;
    }

    printf("The shared memory key (PID) is %d\n", kunci);

    // Inisialisasi nilai awal page
    printf("Initialized page table\n\n");
    for(int i = 0; i < banyakPage; i++){
    	dataPage[i].Valid = 0;
    	dataPage[i].Frame = -1;
    	dataPage[i].Dirty = 0;
    	dataPage[i].Requested = 0;
    	dataPage[i].Counter = 0;
    }

   

    return shmid;
}


/*
	Sinopsis
		int dealokasiSharedMemory(key_t kunci, int ukuran);

	Nilai Kembali
		Jika alokasi berhasil, dikembalikan 0, jika gagal -1.
*/
int dealokasiSharedMemory(int kunci){

    // Melakukan detaching (bahasa indonesianya : pelepasan?) Shared Memori 
    // dari memori (RAM).
    // Setelah dilepas, Shared Memori masih bisa di tempatkan (attach) kembali
    if (shmdt(dataPage) == -1){
        perror("shmdt");
        return -1;
    }

    // Menghapus/dealokasi (deallocate/remove) Shared Memori yang dialokasi tadi.
    if(shmctl(kunci, IPC_RMID, NULL) == -1){
        perror("shmctl");
        return -1;
    }

    return 0;
}

/*
	Sinopsis
		int cariRequest();

	Nilai Kembali
		Jika terdapat request page dari MMU, maka dikembalikan nomor request,
		-1 jika tidak ada.
		Nilai kembali -1, artinya OS selesai bekerja
*/
int cariRequest(){
	int i;
	for(i = 0; i < banyakPage; i++)
		//printf("dataPage[%d] = %d\n", i, dataPage[i].Requested);
		if(dataPage[i].Requested != 0)
			return i;

	return -1;
}

/*
	**PENTING**

	FUNGSI UTAMA
	Penanganan Memori dilakukan disini...
*/

boolean isFrameFull() {
	
	int i;
	int hitung=0;
	for (i=0; i<banyakFrame; i++) {
		int temp = vector_get(&vectorFrame, i);
		if (temp !=-1) {
			hitung++;
		}
	}
	if (hitung==banyakFrame) {
		return true;
	} else
		return false;
}

boolean isFrameEmpty() {

	int i;
	int hitung=0;
	for (i=0; i<banyakFrame; i++) {
		int temp = vector_get(&vectorFrame, i);
		if (temp !=-1) {
			hitung++;
		}
	}
	if (hitung==0) {
		return true;
	} else
		return false;
}

void delFromFrame() {
		/*   Prosedur ini dipanggil karena frame penuh. Maka, akan dicari victim yang akan diswap dengan page yang baru.

		   	+ Jika page ‘victim’ tersebut memiliki dirty bernilai true simulasikan penulisan pada disk, lakukan 
			  sleep(1) dan tambahkan nilai pengaksesan pada disk.

			+ Update page table untuk menyatakan page tersebut sudah tidak berada pada memori fisik. 
		*/

		if (isFrameEmpty()) {
			return; 
		}

		int i, simpan, max;
		max = 0;
		for (i=0; i<banyakPage; i++) {
			if ((dataPage[i].Counter>max) && (dataPage[i].Frame != -1)){
				simpan = i;
				max = dataPage[i].Counter;
				break;
			}
		}

		printf("Choose a victim page %d\n", simpan);
		printf("Victim is dirty, write out\n");
		
		printf("Put in victim's frame %d\n", dataPage[simpan].Frame);
		frameVictim = dataPage[simpan].Frame;
		vector_set(&vectorFrame, frameVictim, 0);
		dataPage[simpan].Valid = 0; //victim di invalid, menyatakan bahwa sudah tidak ada di memory fisik
		dataPage[simpan].Frame = -1; 
		dataPage[simpan].Counter = 0;


		if (dataPage[simpan].Dirty == true) {
			dataPage[simpan].Dirty = 0; 
			aksesDisk++;
			sleep(1); //menuliskan ke disk	
		}

}

void addToFrame (int no_page) {
	/* 	Page yang akan diproses hanya page yang tidak valid
		Kalau frame penuh, hapus yang paling lama di frame atau jarang dipakai  (Least Recently Used)
		Jika terdapat frame yang tidak ditempati, alokasikan.	
		Jika frame penuh, maka akan dipilih victim untuk diswap dengan page yang baru 
	*/
	

	if (!dataPage[no_page].Valid) { //page tidak ada di frame,  maka pilih dan hapus victim 
		boolean penuh=false;
		if (isFrameFull()) {
			delFromFrame();
			penuh = true;
		} 

		
		dataPage[no_page].Valid = 1;
		if (penuh) {
			dataPage[no_page].Frame = frameVictim;
			vector_set(&vectorFrame, frameVictim, no_page);
		} else {
			//Jika belum ada di frame, cari frame yang belum ditempati
			//Jika terdapat frame yang tidak ditempati, alokasikan.
			int i;
			for (i=0; i<banyakFrame; i++) {
				int temp;
				temp = vector_get(&vectorFrame, i);
				if (temp == -1) {
					dataPage[no_page].Frame = i;
					vector_set(&vectorFrame, i, no_page);
					printf("Put it in free frame %d\n", i);
					break;
				}
			}
				
		}	
		aksesDisk++;
		sleep(1); //simulasi akses disk
	} 
} 

void tanganiData(int nomor_request){
	/* Prosedur akan melakukan penambahan waktu tinggal di frame, untuk setiap page yang berada di frame
	   setelah melakukan pengecekan dan diproses(ditambahkan atau tidak ke frame), maka request diset ke 0.

   	   Update page table untuk menyatakan page tersebut telah di load ke memori fisik dalam frame tersebut,
	   tentu dengan dirty bernilai false, dan kembalikan nilai Requested ke 0.
	*/

	int i;
	printf("Process %d has requested page %d\n",dataPage[nomor_request].Requested, nomor_request);
	//melakukan penambahan waktu bagi variable counter untuk masing-masing table
	for (i=0; i<banyakPage; i++) {
		int temp = dataPage[i].Counter;
		dataPage[i].Counter = temp +1;
	}
	addToFrame(nomor_request); 

	dataPage[nomor_request].Requested = 0;

}

/*
	Menangani sinyal SIGUSR1 yang dikirim oleh MMU
*/
void penangananSIGUSR1(int sig){

	// tangani sinyal disini...
	int no_request;

	//# 1. Scan pada page table untuk menemukan Requested field yang diisi oleh MMU. 
	no_request = cariRequest();
	statusOS = no_request;

	//# 2. Jika ditemukan, artinya page tersebut dibutuhkan oleh MMU. 
	if(no_request != -1){
		int PIDMMU = dataPage[no_request].Requested;

		// untuk point # 3, 4, 5, 6, 7
		tanganiData(no_request);

		//# 8. Mengirimkan sinyal SIGCONT ke MMU untuk menyatakan page telah dimuat.
		// kirim sinyal SIGCONT ke MMU
		printf("Unblock MMU\n\n");
		if (kill(PIDMMU, SIGCONT) == -1){
            perror("Gagal mengirim sinyal SIGCONT ke MMU");
            statusOS = -1;
        }
     	
		// Menerima signal realtime selama simulator OS masih berjalan (online)
		signal(SIGUSR1, penangananSIGUSR1);   
	}

}

/*
	Menangani sinyal interrup Ctrl-C yang dilakukan user pada simulator OS
*/
void penangananCtrlC(int sig){
	char  c;

	signal(sig, SIG_IGN);
	printf("\nAnda menekan Ctrl-C? Ingin Berhenti? [y/t] : ");
	c = getchar();
	if (c == 'y' || c == 'Y')
		statusOS = -1;
	else
		signal(SIGINT, penangananCtrlC);
}


int main(int argc, char *argv[]){
	
	if(argc < 3){
		printf("> Kesalahan: argumen Page(arg1) dan Frame(arg2) tidak boleh kosong\n");
		printf("> Gunakan perintah:\n\n");
		printf("    %s <banyak page> <banyak frame>\n\n", argv[0]);
		printf("    contoh: ");
		printf("%s 5 2\n\n", argv[0]);
		return 0;
	}

	
	banyakPage = atoi(argv[1]);
	banyakFrame = atoi(argv[2]);
	
	
	vector_init(&vectorFrame);
	int i;
	for (i=0; i<banyakFrame; i++) {
		vector_set(&vectorFrame, i, -1);
	}

	int idSharedMemory = alokasiSharedMemory(getpid(), banyakPage * sizeof(page_table_entry));
	if(idSharedMemory == -1){
		printf("Alokasi Shared Memory Gagal\n");
		printf("Program Berhenti.\n");
		return 0;
	}

	/*
		Pada bagian ini, Simulator OS (OS.c) bekerja dengan menunggu sinyal SIGUSR1 dari program MMU (MMU.c)
	*/

	// menangani interup ctrl-c dari keyboard
	signal(SIGINT, penangananCtrlC);

	//# 1, 2, 3, 4, 5, 6, 7, 8
	signal(SIGUSR1, penangananSIGUSR1);

	//# 9. Jika tidak ditemukan field Requested dengan nilai bukan nol, berhenti.
	while(statusOS != -1)
		pause();


	// OS Selesai
	printf("The MMU has finished\n");
	PrintPageTable(dataPage, banyakPage);
	printf("%d disk accesses required\n\n", aksesDisk);

	return 0;
}

