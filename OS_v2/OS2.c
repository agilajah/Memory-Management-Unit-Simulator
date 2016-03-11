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
#include "PageQueue.h"
/*
	R0 R2 R1 W3 R0 R2 R1 W4 R0 R2 R1 W3 R0 R2 R1 W4 R0 R2 R1 W3 R0 R2 R1 W4 
*/




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
void tanganiData(int nomor_request){
	//nomor request adalah index untuk mengakses dataPage
	boolean done=false; 
	int i=0;
	// Contoh...
	printf("Process %d has requested page %d\n",dataPage[nomor_request].Requested, nomor_request);

	//# 3. Jika terdapat frame yang tidak ditempati, alokasikan.	
	//# 4. Jika tidak ada seluruh frame ditempati, pilih page ‘victim’ yang akan di swap.
	//printf("ini nomor request : %d\n", nomor_request );
	addToFrame(que, nomor_request);



	//	   	+ Jika page ‘victim’ tersebut memiliki dirty bernilai true simulasikan penulisan pada disk, lakukan 
	//		  sleep(1) dan tambahkan nilai pengaksesan pada disk.
	//        .. code ..

	//		+ Update page table untuk menyatakan page tersebut sudah tidak berada pada memori fisik.
	//        .. code ..

	//# 5. Simulasikan pemuatan page dengan sleep(1) dan tambahkan nilai pengaksesan pada disk.


	//# 6. Update page table untuk menyatakan page tersebut telah di load ke memori fisik dalam frame tersebut,
	//     tentu dengan dirty bernilai false, dan kembalikan nilai Requested ke 0.
	//dataPage[nomor_request].Dirty = 0;
	dataPage[nomor_request].Requested = 0;

	//# 7. Cetak page table setelah diupdate.
	// .. code ..
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

		//# 3, 4, 5, 6, 7
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

	//printf("> Status OS : %s\n\n", (statusOS != -1 ? "Online" : "Offline"));
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


	statusOS = 0;
	aksesDisk = 0;
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


	//melakukan inisialisasi queue dan hash(frame)
		que = createQueue(banyakFrame);
		//hash = createHash(banyakPage);
	//inisialisasi vector untuk merepresentasikan frame yang masih tersedia, elemen vector berjumlah sebanyak frame
		//vector_init(&vectorFrame);

	/*
	printf("> Alokasi Shared Memory :\n");
	printf("  Page  : %d\n", banyakPage);
	printf("  Frame : %d\n", banyakFrame);
	printf("\n");
	printf("> Status :\n");
	*/
	int idSharedMemory = alokasiSharedMemory(getpid(), banyakPage * sizeof(page_table_entry));
	if(idSharedMemory == -1){
		printf("Alokasi Shared Memory Gagal\n");
		printf("Program Berhenti.\n");
		return 0;
	}else{
		/*
		printf("  Alokasi berhasil!\n");
		printf("+----------------------+---------------+\n");
		printf("| Shared Memory Key    | %13d |\n", getpid());
		printf("+----------------------+---------------+\n");
		printf("| Shared Memory ID     | %13d |\n", idSharedMemory);
		printf("+----------------------+---------------+\n");
		printf("| Process ID (PID)     | %13d |\n", getpid());
		printf("+----------------------+---------------+\n");
		*/
	}

	/*
	printf("\n");
	printf("+->->->->->->->->->->-+\n");
	printf("$ PROSES BERLANGSUNG! $\n");
	printf("+-<-<-<-<-<-<-<-<-<-<-+\n");
	printf("\n");
	*/
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


	if(dealokasiSharedMemory(idSharedMemory) == -1){
		//printf("Dealokasi Shared Memory Gagal\n");
	}else{
		//printf("Dealokasi Shared Memory Berhasil\n");
	}

	//printf("OS Selesai\n");

	return 0;
}

