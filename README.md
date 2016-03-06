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
  boolean <i>Valid</i> : menyatakan apakah page tersebut ada di memori fisik. <br>
  integer <i>Frame</i> : menyatakan indeks frame page tersebut di memori fisik.<br>
  boolean <i>Dirty</i> : menyatakan apakah page tersebut telah ditulis. <br>
  integer <i>Requested<i> : bernilai bukan‐nol hanya jika page tersebut tidak pada memori fisik dan dipesan oleh MMU. Pada kasus tersebut, nilai ini diisi dengan ID proses (PID) dari MMU.<br>
</p>
<br>
<p>
  <b>Simulator Unit Manajemen Memori (MMU)</b><br>
</p>
<br>
<p>
  <b>Simulator Sistem Operasi</b><br>
</p>
<br>
