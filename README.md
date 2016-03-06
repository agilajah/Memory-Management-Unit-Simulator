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
