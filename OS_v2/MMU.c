//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

 
//----Used for delayed tasks
void ContinueHandler(int Signal) {
//----Nothing to do
}

//-----------------------------------------------------------------------------
void PrintPageTable(page_table_entry PageTable[],int NumberOfPages) {
    int Index;

    for (Index =  0;Index < NumberOfPages;Index++) {
        printf("%2d: Valid=%1d Frame=%2d Dirty=%1d Requested=%1d\n",Index, PageTable[Index].Valid,PageTable[Index].Frame,PageTable[Index].Dirty, PageTable[Index].Requested);
    }
}


//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {

    int SharedMemoryKey;
    int NumberOfPages;
    int OSPID;
    int SegmentID;
    page_table_pointer PageTable;
    int RSIndex;
    int Mode;
    int Page;

    /*
        SharedMemoryKey = arg terakhir
        NumberOfPages = arg pertama
    */
    if (argc < 2 || (OSPID = SharedMemoryKey = atoi(argv[argc-1])) == 0  || (NumberOfPages = atoi(argv[1])) == 0) {
        printf("Usage:");
        exit(EXIT_FAILURE);
    }

    printf("SharedMemoryKey = %d | NumberOfPages = %d\n", SharedMemoryKey, NumberOfPages);
    //----Create the page table
    if ((SegmentID = shmget(SharedMemoryKey, NumberOfPages * sizeof(page_table_entry), 0)) == -1
     || (PageTable = (page_table_pointer) shmat(SegmentID,NULL,0)) == NULL){
        printf("SegmentID : %d\n", SegmentID);
        perror("ERROR: Could not get page table");
        exit(EXIT_FAILURE);
    }

    //----Handler for page fault
    if (signal(SIGCONT,ContinueHandler) == SIG_ERR) {
        printf("ERROR: Could not initialize continue handler\n");
        exit(EXIT_FAILURE);
    }

    printf("Initialized page table:\n");
    PrintPageTable(PageTable,NumberOfPages);
    printf("\n");
    //----Deal with the page requests
    for (RSIndex = 2;RSIndex < argc-1;RSIndex++) {
        Mode = argv[RSIndex][0];
        Page = atoi(&argv[RSIndex][1]);
        //----Check that it's within the process
        if (Page >= NumberOfPages) {
            printf("ERROR: That page number in %c%d is outside the process\n", Mode,Page);
        } else {
            printf("Request for page %d in %c mode\n",Page,Mode);
            //----Check if in memory

            //# 1. Mengecek apakah page ada dalam memori fisik
            if (!PageTable[Page].Valid) {

                //# 2. Jika tidak, MMU akan menuliskan PID ke field ​Requested untuk page tersebut. 
                printf("It's not in RAM - page fault\n");

                //# Page
                PageTable[Page].Requested = getpid();
                //----Sleep a bit to allow OS to get ready for another signal
                sleep(1);

                //# 3. Mengirimkan sinyal ​SIGUSR1 ke proses OS.
                //# mengirim sinyal SIGUSR1 ke program OS dgn PID = OSPID
                if (kill(OSPID,SIGUSR1) == -1) {
                    perror("Kill to OS");
                    exit(EXIT_FAILURE);
                }

                //# 4. Blocking, sampai mendapatkan sinyal SIGCONT dari proses OS yang menyatakan bahwa page telah dimuat ke memori fisik
                /*
                    > Deskripsi pause()
                    > pause() causes the calling process (or thread) to sleep until a signal is
                      delivered that either terminates the process or causes the invocation of
                      a signal-catching function.
                    > Sumber: man pause
                */
                pause();

                if (!PageTable[Page].Valid) {
                    printf("Bugger, something wrong\n");
                }
            } else {
                printf("It's in RAM\n");
            }

            //# 5. Jika proses ini merupakan proses penulisan, melakukan update pada ​Dirty ​menjadi true
            //----If write mode, set the dirty bit
            if (Mode == 'W') {
                printf("Set the dirty bit for page %d\n",Page);
                PageTable[Page].Dirty = 1;
            }
            PrintPageTable(PageTable,NumberOfPages);
            printf("\n");
        }
    }

    //----Free the shared memory
    if (shmdt(PageTable) == -1) {
        perror("ERROR: Error detaching segment");
        exit(EXIT_FAILURE);
    }

    //----Alert OS
    printf("Tell OS that I'm finished\n");
    //----Sleep a bit to allow OS to get ready for another signal
    sleep(1);

    if (kill(OSPID,SIGUSR1) == -1) {
        perror("Kill to OS");
        exit(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
//-----------------------------------------------------------------------------
