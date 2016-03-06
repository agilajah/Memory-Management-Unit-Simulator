# Memory-Management-Unit-Simulator
Tugas Besar OS

#Penjelasan Berkas
<p>
  <b>Tipe data yang di share pada Virtual Memori</b><br>
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
<p>
  <b>Simulator Unit Manajemen Memori (MMU)</b><br>
  Proses yang terdiri dari Page disimulasikan oleh MMU. Masukan proses diberikan dalam bentuk
  <pre>
    [mode][page]
  </pre>
  [mode] : jenis aksi dari proses yang akan diproses, baca(R) atau tulis(R) <br>
  [page] : nomor page yang akan diakses<br><br>
  <b> Alur Manajemen Memori </b><br>
  <b>Pre</b><br>
  Pertama kali dijalankan, MMU menerima masukan berupa alamat Shared Memory (terdapat pada argumen terakhir MMU.c) yang digunakan oleh MMU untuk menciptakan sebuah pointer ke memori virtual yang telah dialokasikan oleh Simulator Sistem Operasi.<br>
  Setelah proses tersebut selesai, MMU siap menerima pekerjaan.<br><br>
  <b>Proses</b><br>
  Step MMU mengolah pekerjaan adalah sbb:<br>
  1. Masukan sebuah proses dengan format seperti diatas.<br>
  2. MMU mengecek page yang diminta apakah ada pada memori virtual diatas.<br>
  3. Jika page yang diminta ada, maka selesai, lanjut mengolah proses berikutnya.<br>
  4. Jika page yang diminta tidak ada, maka MMU melakukan request ke OS Simulator, agar OS Simulator memuat/memasukkan page tersebut ke memori virtual. (Rincian proses pemuatan page, baca : spek tubes)<br>
  5. Setelah diload, maka selesai, lanjut mengolah proses berikutnya.<br>
</p>
<br>
<p>
  <b>Simulator Sistem Operasi</b><br>
</p>
<br>
