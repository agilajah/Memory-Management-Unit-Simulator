//-----------------------------------------------------------------------------
typedef struct {
    int Valid;
    int Frame;
    int Dirty;
    int Requested;
    int Counter;
    } page_table_entry; 

typedef page_table_entry* page_table_pointer;
//-----------------------------------------------------------------------------
