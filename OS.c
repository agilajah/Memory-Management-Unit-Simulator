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


/** LINUX Standard signals

	Linux supports the standard signals listed below.  Several signal
	numbers are architecture-dependent, as indicated in the "Value"
	column.  (Where three values are given, the first one is usually
	valid for alpha and sparc, the middle one for x86, arm, and most
	other architectures, and the last one for mips.  (Values for parisc
	are not shown; see the Linux kernel source for signal numbering on
	that architecture.)  A - denotes that a signal is absent on the
	corresponding architecture.)

	First the signals described in the original POSIX.1-1990 standard.

	Signal     Value     Action   Comment
	──────────────────────────────────────────────────────────────────────
	SIGHUP        1       Term    Hangup detected on controlling terminal
	                              or death of controlling process
	SIGINT        2       Term    Interrupt from keyboard
	SIGQUIT       3       Core    Quit from keyboard
	SIGILL        4       Core    Illegal Instruction
	SIGABRT       6       Core    Abort signal from abort(3)
	SIGFPE        8       Core    Floating point exception
	SIGKILL       9       Term    Kill signal
	SIGSEGV      11       Core    Invalid memory reference
	SIGPIPE      13       Term    Broken pipe: write to pipe with no readers
	SIGALRM      14       Term    Timer signal from alarm(2)
	SIGTERM      15       Term    Termination signal
	SIGUSR1   30,10,16    Term    User-defined signal 1
	SIGUSR2   31,12,17    Term    User-defined signal 2
	SIGCHLD   20,17,18    Ign     Child stopped or terminated
	SIGCONT   19,18,25    Cont    Continue if stopped
	SIGSTOP   17,19,23    Stop    Stop process
	SIGTSTP   18,20,24    Stop    Stop typed at terminal
	SIGTTIN   21,21,26    Stop    Terminal input for background process
	SIGTTOU   22,22,27    Stop    Terminal output for background process

	The signals SIGKILL and SIGSTOP cannot be caught, blocked, or
	ignored.

	Next the signals not in the POSIX.1-1990 standard but described in
	SUSv2 and POSIX.1-2001.

	Signal       Value     Action   Comment
	────────────────────────────────────────────────────────────────────
	SIGBUS      10,7,10     Core    Bus error (bad memory access)
	SIGPOLL                 Term    Pollable event (Sys V).
	                                Synonym for SIGIO
	SIGPROF     27,27,29    Term    Profiling timer expired
	SIGSYS      12,31,12    Core    Bad argument to routine (SVr4)
	SIGTRAP        5        Core    Trace/breakpoint trap
	SIGURG      16,23,21    Ign     Urgent condition on socket (4.2BSD)
	SIGVTALRM   26,26,28    Term    Virtual alarm clock (4.2BSD)
	SIGXCPU     24,24,30    Core    CPU time limit exceeded (4.2BSD)
	SIGXFSZ     25,25,31    Core    File size limit exceeded (4.2BSD)

	Up to and including Linux 2.2, the default behavior for SIGSYS,
	SIGXCPU, SIGXFSZ, and (on architectures other than SPARC and MIPS)
	SIGBUS was to terminate the process (without a core dump).  (On some
	other UNIX systems the default action for SIGXCPU and SIGXFSZ is to
	terminate the process without a core dump.)  Linux 2.4 conforms to
	the POSIX.1-2001 requirements for these signals, terminating the
	process with a core dump.

	Next various other signals.

	Signal       Value     Action   Comment
	────────────────────────────────────────────────────────────────────
	SIGIOT         6        Core    IOT trap. A synonym for SIGABRT
	SIGEMT       7,-,7      Term
	SIGSTKFLT    -,16,-     Term    Stack fault on coprocessor (unused)
	SIGIO       23,29,22    Term    I/O now possible (4.2BSD)
	SIGCLD       -,-,18     Ign     A synonym for SIGCHLD
	SIGPWR      29,30,19    Term    Power failure (System V)
	SIGINFO      29,-,-             A synonym for SIGPWR
	SIGLOST      -,-,-      Term    File lock lost (unused)
	SIGWINCH    28,28,20    Ign     Window resize signal (4.3BSD, Sun)
	SIGUNUSED    -,31,-     Core    Synonymous with SIGSYS

	(Signal 29 is SIGINFO / SIGPWR on an alpha but SIGLOST on a sparc.)

	SIGEMT is not specified in POSIX.1-2001, but nevertheless appears on
	most other UNIX systems, where its default action is typically to
	terminate the process with a core dump.

	SIGPWR (which is not specified in POSIX.1-2001) is typically ignored
	by default on those other UNIX systems where it appears.

	SIGIO (which is not specified in POSIX.1-2001) is ignored by default
	on several other UNIX systems.

	Where defined, SIGUNUSED is synonymous with SIGSYS on most
	architectures.

**/
