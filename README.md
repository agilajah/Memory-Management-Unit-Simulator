# Memory-Management-Unit-Simulator
Tugas Besar OS

#Tipe Data Share
<p>
  Pada berkas "PageTable.h", tipe data sebagai berikut:
  <pre>
  typedef struct {
    int Valid;
    int Frame;
    int Dirty;
    int Requested;
    } page_table_entry;

  typedef page_table_entry* page_table_pointer;
  </pre>
  <i>Valid</i> : menyatakan apakah page tersebut ada di memori fisik. <br>
  <i>Frame</i> : menyatakan indeks frame page tersebut di memori fisik.<br>
  <i>Dirty</i> : menyatakan apakah page tersebut telah ditulis. <br>
  <i>Requested</i> : bernilai bukan‐nol hanya jika page tersebut tidak pada memori fisik dan dipesan oleh MMU. Pada kasus tersebut, nilai ini diisi dengan ID proses (PID) dari MMU.<br>
</p>
<br>
#Simulator Unit Manajemen Memori (MMU)
<p>
  Proses yang terdiri dari Page disimulasikan oleh MMU. Masukan proses diberikan dalam bentuk
  <pre>
    [mode][page]
  </pre>
  [mode] : jenis aksi dari proses yang akan diproses, baca(R) atau tulis(R) <br>
  [page] : nomor page yang akan diakses<br><br>
  <b> Alur Manajemen Memori </b><br>
  <b>> Pre</b><br>
  Pertama kali dijalankan, MMU menerima masukan lokasi memori virtual berupa kunci(Shared Memory Key) yang dialokasikan oleh OS Simulator (terdapat pada argumen terakhir MMU.c) yang akan digunakan oleh MMU untuk menciptakan sebuah segmen memori virtual pada RAM agar dapat digunakan (proses attach), lalu menciptakan sebuah pointer ke segmen tersebut.<br>
  Setelah proses tersebut selesai, MMU siap menerima pekerjaan.<br><br>
  <b>> Proses</b><br>
  Step MMU mengolah pekerjaan adalah sbb:<br>
  1. Masukan sebuah proses dengan format seperti diatas.<br>
  2. MMU mengecek page yang diminta apakah ada pada segmen memori virtual diatas.<br>
  3. Jika page yang diminta ada, maka selesai, lanjut mengolah proses berikutnya.<br>
  4. Jika page yang diminta tidak ada, maka MMU melakukan request ke OS Simulator, agar OS Simulator memuat/memasukkan page tersebut ke memori virtual. (Rincian proses pemuatan page, baca : spek tubes)<br>
  5. Setelah diload, maka selesai, lanjut mengolah proses berikutnya.<br><br>
  <b>> Post</b><br>
  Setelah pekerjaan MMU selesai, MMU harus melepas segmen yang diciptakan pada RAM (free shared memory).<br>
</p>
<br>
#Simulator Sistem Operasi
<p>
  <b>> Pre</b><br>
  Sistem Operasi dimulai dengan dua argumen awal, yaitu banyak Page dan banyak Frame. Sistem operasi akan melakukan alokasi memori virtual pada disk sebesar banyak Page (arg) dengan jenis data page_table_entry. Sistem Operasi akan mencetak Shared Memory Key ke layar ketika berhasil melakukan alokasi. Kunci ini akan digunakan oleh MMU untuk mengakses memori virtual yang telah dialokasi.
  Setelah proses alokasi dan mencetak kunci selesai, Sistem Operasi menunggu sinyal dari MMU untuk mengerjakan tugas yang dibutuhkan oleh MMU.<br><br>
  <b>> Proses</b><br>
  Step Sistem Operasi mengolah pekerjaan adalah sbb:<br>
  1. Sistem operasi menerima sinyal SIGUSR1 dari MMU.<br>
  2. Sistem Operasi mengecek apakah MMU meminta untuk meload sebuah page ke memori.<br>
  3. Jika MMU meminta request page, Sistem Operasi akan memuat page tersebut ke dalam memori virtual, lalu mengirim sinyal kembali ke MMU menandakan page telah selesai dimuat.<br>
  4. Jika MMU tidak meminta request page, maka Sistem Operasi berhenti (Selesai).<br><br>
  <b>> Post</b><br>
  Sebelum Sistem Operasi berhenti, memori virtual yang dialokasikan pada disk harus didealokasi oleh Sistem Operasi.<br>
  
</p>
<br>
