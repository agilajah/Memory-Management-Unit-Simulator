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

/*
	Sinopsis
		int alokasiSharedMemory(key_t kunci, int ukuran);

	Nilai Kembali
		Jika alokasi berhasil, dikembalikan id shared memory, jika gagal -1.
*/
int alokasiSharedMemory(key_t kunci, int ukuran){
    int shmid;
    char *data;
    int mode;

    /* connect to (and possibly create) the segment: */
    if ((shmid = shmget(kunci, ukuran, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        return -1;
    }

    //printf("shmid = %d | PID = %d\n", shmid, getpid());

    /* attach to the segment to get a pointer to it: */
    data = shmat(shmid, (void *)0, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
        return -1;
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

    /* attach to the segment to get a pointer to it: */
    page_table_pointer data = (page_table_pointer) shmat(kunci, NULL, 0);
    if (shmdt(data) == -1){
        perror("shmdt");
        return -1;
    }

    if(shmctl(kunci, IPC_RMID, NULL) == -1){
        perror("shmctl");
        return -1;
    }

    return 0;
}

/*
	Menangani sinyal SIGUSR1 yang dikirim oleh MMU
*/
void  penangananSignal(int sig){

	// tangani sinyal disini...
    
    signal(SIGUSR1, penangananSignal);
}

int main(int argc, char *argv[]){

	if(argc < 2){
		printf("Error: argumen harus diisi\n");
		return 0;
	}

	int NumberOfPages = atoi(argv[1]);

	int idSharedMemory = alokasiSharedMemory(getpid(), NumberOfPages * sizeof(page_table_entry));
	if(idSharedMemory == -1){
		printf("Alokasi Shared Memory Gagal\n");
		printf("Program Berhenti.\n");
		return 0;
	}else{
		printf("Alokasi Shared Memory berhasil | idSharedMemory = %d\n", idSharedMemory);
	}

	printf("\n");
	printf("+->->->->->->->->->->-+\n");
	printf("$ PROSES BERLANGSUNG! $\n");
	printf("+-<-<-<-<-<-<-<-<-<-<-+\n");
	printf("\n");
	/*
		Pada bagian ini, Simulator OS (OS.c) bekerja dengan menunggu sinyal SIGUSR1 dari program MMU (MMU.c)

		kode sbb:

			signal(SIGUSR1, penangananSignal);
		    while (1)
		        pause();
	*/

	if(dealokasiSharedMemory(idSharedMemory) == -1){
		printf("Dealokasi Shared Memory Gagal\n");
	}else{
		printf("Dealokasi Shared Memory Berhasil\n");
	}

	printf("OS Selesai\n");

	return 0;
}
