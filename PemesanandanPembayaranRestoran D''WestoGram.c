#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>//untuk delay
#include <stdbool.h>

#define TABLE_SIZE 101
#define ShowIDPW 1//ubah untuk menampilkan id dan password dilayar inisiasi (0= disable, 1 = enable)

typedef struct Data
{
    char menu[30];//nama makanan
    long long int harga;//harga makanan
    int kode;//kode makanan (untuk pemesanan)
    int stock;//ketersediaan menu

    struct Data *next, *prev;//untuk double linked list
} Data;

typedef struct Pesanan
{
    char menu[30];//nama makanan
    long long int harga;//harga makanan
    int jumlah;//jumlah pesanan

    struct Pesanan *next, *prev;//untuk double linked list
} Pesanan;

typedef struct Gtree//untuk daftar pekerja
{
    char nama[30];

    struct Gtree *fchild;
    struct Gtree *nextsib;
} Gtree;

struct Password
{
    char username[30];
    unsigned long int pass;
};

struct Password *hash_table[TABLE_SIZE];

void delay(int detik);//delay dalam 1/10 detik

char mainMenu( int mode, int no, char status[]);//menampilkan menu utama (pilihan)

void menus(int mode);//menampilkan pilihan untuk berbagai pilihan

int meja();//menentukan nomor meja

void addmenu(Data **Head, Data **Tail, char makanan[30], long long int harga, int stock);//menyimpan menu kedalam file dan linked list

int addpesanan(Pesanan **Head, Pesanan **Tail, Data **Dhead, int *history);//menambahkan pesanan baru

void inputmenu(Data **Head, Data **Tail);//memasukan menu makanan baru

void pop(Pesanan **Head, Pesanan **Tail, Pesanan **Curr);//menghapus pesanan

void listmenu(Data **Head);//menampilkan daftar makanan yang tersedia

void listpesanan(Pesanan **Head, char menu[]);//menampilkan pesanan yang sudah dipilih

void swap(Pesanan **a, Pesanan **b);//swap node

void sort(Pesanan **Head, Pesanan **Tail, Pesanan **Curr, int mode);//fungsi untuk sortir linked list pesanan

int modes(int mode, char opsi);//menentukan mode yang digunakan

void pushstaf(Gtree **root, Gtree **Curr, char *nama, int n);//Memasukan data pekerja kedalam tree

void dfs(Gtree *root);//menampilkan daftar pekerja bila sudah di inisiasi

int main()
{
    int mode=0, batas, stop=0;//variabel untuk batas
    char opsi, opsi2, opsi3;//variabel untuk pilihan
    int history = -1;
    int init=0, n;
    long long int harga;
    int kode;//untuk snatch harga dan kode dari file
    int stock;//untuk snatch harga dan kode dari file
    int nomor_meja;//variabel untuk menentukan nomor meja

    char nama[30];//variabel untuk snatch nama dari file
    char status[10]="Waiting";//variabel untuk menentukan status pemesanan

    Data *dhead, *dtail, *Dcurr;//variabel penyimpan linked list Data (menu)
    Pesanan *phead, *ptail, *Pcurr;//variabel penyimpan linked list Pesan (pesanan)
    dhead=dtail=Dcurr=NULL;
    phead=ptail=Pcurr=NULL;

    Gtree *staff, *curr;//daftar pekerja
    staff=curr=NULL;

    FILE *fp;
    fp=fopen("Menu.txt", "r");//membaca dari file
    while ((batas = fgetc(fp)) != EOF)//membaca sampai end of file (habis)
    {
        fscanf(fp, "%[^\n]%*c", nama);
        fscanf(fp, "%*c%lld%*c", &harga);
        fscanf(fp, "%*c%d%*c", &kode);
        fscanf(fp, "%*c%d%*c%*c", &stock);
        addmenu(&dhead,&dtail,nama,harga, stock);//menambahkan kedalam linked list dari file
    }
    fclose(fp);

    menus(0);//opening dari program
    nomor_meja=meja();//menentukan nomor meja di awal
    do
    {
        opsi = mainMenu(mode, nomor_meja, status);//main menu atau menu utama
        mode=modes(mode,opsi);
        if(opsi=='+'||opsi=='-')//mengganti mode
            continue;
        else if(opsi=='1')//(customer) memesan makanan
        {
            if(dhead==NULL)//error cek sederhana
            {
                printf("Maaf, daftar menu belum tersedia untuk saat ini");
                delay(10);
                continue;
            }
            else if(strcmp(status,"Accepted")==0)//error cek
            {
                printf("Pesanan yang sudah dikirim tidak dapat diubah");
                delay(10);
                continue;
            }
            do
            {
                listmenu(&dhead);
                stop=addpesanan(&phead,&ptail, &dhead, &history);
            }
            while(stop!=1);
            stop=0;
        }
        else if(opsi=='2')//mengecek pesanan yang sudah dipesan
        {
            if(phead==NULL)//error cek sederhana
            {
                printf("Anda belum memesan!");
                delay(8);
                continue;
            }

            do//looping untuk memilih opsi atau pilihan selanjutnya
            {
                listpesanan(&phead,"");//menampilkan pesanan
                menus(1);//menampilkan pilihan

                opsi2=getche();//input untuk opsi pilihan
                printf("\n");
                delay(2);

                if(opsi2=='0')//keluar dari menu
                    break;
                else if(opsi2=='1')//Mengubah Pesanan
                {
                    history=-1;
                    if(strcmp(status,"Accepted")==0)//error cek sederhana
                        printf("Tidak dapat mengubah pesanan yang sudah dikirim");
                    else
                    {
                        Pcurr=phead;
                        do
                        {
                            listpesanan(&phead,Pcurr->menu);
                            menus(2);

                            opsi3=getche();
                            printf("\n");
                            delay(2);

                            if(opsi3=='4')//previous
                            {
                                if(Pcurr->prev==NULL)
                                    Pcurr=ptail;
                                else
                                    Pcurr=Pcurr->prev;
                            }
                            else if(opsi3=='6')//next
                            {
                                if(Pcurr->next==NULL)
                                    Pcurr=phead;
                                else
                                    Pcurr=Pcurr->next;
                            }
                            else if(opsi3=='1')//membatalkan pesanan
                            {
                                pop(&phead, &ptail, &Pcurr);
                                printf("Berhasil membatalkan pesanan");
                                delay(8);
                            }
                            else if(opsi3=='2')//mengubah jumlah pesanan
                            {
                                printf("Masukan jumlah pesanan yang baru : ");
                                scanf("%d",&Pcurr->jumlah);
                                getchar();
                                if(Pcurr->jumlah==0)//mencegah memasukan data kosong (error cek)
                                {
                                    pop(&phead, &ptail, &Pcurr);
                                    printf("Berhasil membatalkan pesanan");
                                }
                                else
                                    printf("Berhasil mengubah jumlah pesanan");

                                delay(8);
                            }
                            else if(opsi3=='0')//keluar
                                break;
                            else//error cek sederhana
                            {
                                printf("Pilihan tidak tersedia");
                                delay(6);
                            }
                        }
                        while(opsi3!=0&&phead!=NULL);
                    }
                }
                else if(opsi2=='2')//Mengurutkan Pesanan
                {
                    history=-1;
                    Pcurr=phead;
                    listpesanan(&phead," ");
                    menus(4);
                    opsi3=getche();
                    delay(2);
                    if(opsi3=='1')//berdasarkan nama makanan
                        sort(&phead,&ptail,&Pcurr,1);
                    else if(opsi3=='2')//berdasarkan harga
                        sort(&phead,&ptail,&Pcurr,2);
                    else if(opsi3=='3')//berdasarkan jumlah
                        sort(&phead,&ptail,&Pcurr,3);
                    else if(opsi3=='4')//berdasarkan sub total
                        sort(&phead,&ptail,&Pcurr,4);
                    else
                    {
                        printf("Pilihan tidak tersedia!");
                        delay(8);
                        continue;
                    }
                    printf("\nPesanan Berhasil disortir");
                    delay(7);
                }
                else//error cek sederhana
                    printf("Opsi Tidak Tersedia!");
            }
            while(opsi2!=0&&phead!=NULL);
        }
        else if(opsi=='3')//melakukan pembatalan semua pesanan
        {
            if(strcmp(status,"Accepted")==0)//error cek
                printf("Pesanan yang sudah dikirim tidak dapat dibatalkan");
            else if(phead==NULL)//error cek sederhana
                printf("Tidak ada pesanan");
            else
            {
                history=-1;
                printf("Pesanan berhasil dibatalkan");
                while(phead!=NULL)//membersihkan linked list pesanan
                {
                    Pcurr=phead;
                    phead=phead->next;
                    free(Pcurr);
                }
            }
            delay(15);
        }
        else if(opsi=='4')//search (linear) nama makanan
        {
            strcpy(nama, "-");
            printf("Masukan nama makanan yang ingin dicari : ");
            scanf("%[^\n]", nama);
            getchar();
            Dcurr = dhead;
            while(Dcurr!=NULL)
            {
                if(strcmpi(nama, Dcurr->menu)==0)
                {
                    system("cls");
                    printf("======================================\n");
                    printf("|Nama            : %s\n", Dcurr->menu);
                    printf("|Harga           : %lld\n", Dcurr->harga);
                    printf("|Stock           : ");
                    if(Dcurr->stock==1)
                        printf("Tersedia\n");
                    else
                        printf("Kosong\n");
                    printf("|Kode pemesanan  : %d\n", Dcurr->kode);
                    printf("======================================\n");
                    printf("Tekan apapun untuk melanjutkan...");
                    getch();
                    opsi=1;
                    break;
                }
                Dcurr=Dcurr->next;
            }
            if(opsi=='4')
            {
                printf("Menu dengan nama tersebut tidak ditemukan!");
                delay(8);
            }
        }
        else if(opsi=='5')//Mengirim pesanan ke server kasir
        {
            if(phead==NULL)//error cek sederhana
                printf("Tidak ada pesanan");
            else
            {
                strcpy(status,"Accepted");
                printf("Silahkan melakukan pembayaran dikasir");
            }
            delay(10);
            //code lanjutan
        }
        else//mode owner
        {
            if(mode==0)//error check sederhana
            {
                printf("Pilihan tidak tersedia");
                opsi='1';
                delay(8);
            }
            else if(opsi=='6')//mengubah list menu
            {
                menus(3);
                scanf("%c", &opsi2);
                getchar();
                if(opsi2=='1')//menambahkan menu makanan baru
                    inputmenu(&dhead, &dtail);
                else if(opsi2=='2')//menghapus seluruh daftar menu
                {
                    fp=fopen("Menu.txt","w");//mereset file Menu.txt
                    fclose(fp);
                    while(dhead!=NULL)//menghapus linked list pesanan
                    {
                        Dcurr=dhead;
                        dhead=dhead->next;
                        free(Dcurr);
                    }
                    printf("Berhasil menghapus daftar menu");
                    delay(8);
                }
            }
            else if(opsi=='7')//mengganti nomor meja
                nomor_meja=meja();
            else if(opsi=='8')
            {
                system("cls");
                printf("=====================================\n");
                if(init==0)//inisiasi daftar pekerja (bila menu diakses untuk pertama kalinya)
                {
                    int i;
                    char pos[2], post[2];
                    fp = fopen("Staff.txt", "r");
                    while((batas = fgetc(fp)) != '#')//membaca file sampai batas yang ditentukan (#) untuk memindahkan pointer ketempat yang di inginkan
                        fscanf(fp, "%*c");

                    i=1;
                    while((batas = fgetc(fp)) != EOF)//membaca sampai end of file (habis)
                    {

                        fscanf(fp, "%*c%[^]]", post);
                        fscanf(fp, "%*c%[^\n]", nama);

                        if(strcmpi(post,"k")==0)//mengecek status pekerja (pemilik/manager/karyawan)
                        {
                            n=2;
                            if(strcmpi(pos,post)!=0)//pengecekan hierarki
                            {
                                n=1;
                                strcpy(pos,post);
                                printf("*Karyawan :\n");
                            }
                            printf("  - %s\n", nama);
                            pushstaf(&staff, &curr, nama, n);//memasukan pekerja kedalam tree
                        }
                        else if(strcmpi(post,"m")==0)
                        {
                            n=-1;
                            if(strcmpi(pos,post)!=0)
                            {
                                n=0;
                                strcpy(pos,post);
                            }
                            printf("%d. Manager :", i++);
                            printf("%s\n", nama);
                            pushstaf(&staff, &curr, nama, n);
                        }
                        //memasukan pemiliki dalam daftar pekerja
                        //diletakan dipaling bawah dikarenakan menu ini hanya diakses 1x
                        else
                        {
                            strcpy(pos,post);
                            printf("Owner   : %s\n\n", nama);
                            pushstaf(&staff, &curr, nama, 0);
                        }
                    }
                    fclose(fp);
                    init=1;
                }
                else//bila sebelumnya sudah di inisiasi
                    dfs(staff);
                printf("=====================================");
                printf("\n\nTekan apapun untuk melanjutkan...");
                getch();
            }
            else if(opsi=='0')//stop program
                break;
            else//error cek
            {
                printf("Pilihan tidak tersedia");
                delay(5);
            }
        }
    }
    while(opsi!='0');

    while(phead!=NULL)//pembersihan linked list Data
    {
        Pcurr=phead;
        phead=phead->next;
        free(Pcurr);
    }
    while(dhead!=NULL)//pembersihan linked list Pesanan
    {
        Dcurr=dhead;
        dhead=dhead->next;
        free(Dcurr);
    }

    return 0;
}

void delay(int detik)//delay dalam 1/10 detik
{
    long long int mdetik = 100 * detik;
    clock_t start = clock();

    while(clock()< start+mdetik);
}

char mainMenu(int mode, int no, char status[])//menampilkan menu utama (pilihan)
{
    char cek;
    system("cls");
    if(mode==2)
    {
        printf("===============================\n");
        printf("||          Cashier          ||\n");
        printf("===============================\n");

    }
    else
    {
        printf("===============================\n");
        printf("||           Meja            ||\n");
        printf("||          %4d             ||\n", no);
        printf("||   Order status:%8s   ||\n", status);
        printf("===============================\n");
        printf("|| 1. Pesan Makanan          ||\n");
        printf("|| 2. Daftar Pesanan         ||\n");
        printf("|| 3. Batalkan semua pesanan ||\n");
        printf("|| 4. Cari Makanan           ||\n");
        printf("|| 5. Selesai Memesan        ||\n");
        printf("===============================\n");
        if(mode==1)//Mode owner
        {
            printf("||       [Mode Owner]        ||\n");
            printf("|| 6. Ubah Daftar Menu       ||\n");
            printf("|| 7. Ganti Nomor Meja       ||\n");
            printf("|| 8. Daftar Pekerja         ||\n");
            printf("|| 0. Exit                   ||\n");
            printf("===============================\n");
        }
    }
    printf("Masukan Pilihan : ");
    cek=getche();
    printf("\n");

    return cek;
}

void menus(int mode)//menampilkan pilihan untuk berbagai pilihan
{
    if(mode==1)//menu pesanan
    {
        printf(" 1. Ubah Pesanan\n");
        printf(" 2. Urutkan Pesanan\n");
        printf(" 0. Simpan perubahan\n");
        printf("Masukan Pilihan : ");
    }
    else if(mode==2)//menu mengubah pesanan
    {
        printf("<<[(4) Prev]                            [(6) Next]>>\n\n");
        printf(" 1. Batalkan\n");
        printf(" 2. Ubah jumlah pesanan\n");
        printf(" 0. Kembali\n");
        printf("Pilihan : ");
    }
    else if(mode==3)//menu ubah daftar menu
    {
        printf("\n 1. Tambahkan menu\n");
        printf(" 2. Hapus semua Menu\n");
        printf(" 0. Kembali ke menu utama\n");
        printf(" Pilihan : ");
    }
    else if(mode==4)//menu sortir
    {
        printf("1. Urutkan berdasarkan Nama Makanan\n");
        printf("2. Urutkan berdasarkan Harga\n");
        printf("3. Urutkan berdasarkan Jumlah\n");
        printf("4. Urutkan berdasarkan Sub Total\n");
        printf("Urutkan berdasarkan : ");
    }
    else if(mode==0)
    {
        printf("(!!!)                Untuk masuk ke mode owner silahkan tekan '+' pada menu utama                 (!!!)\n");
        printf("(!!!)            Untuk masuk keluar dari mode owner silahkan tekan '-' pada menu utama            (!!!)\n");
        printf("(!!!) Untuk keluar dari aplikasi silahkan masuk kedalam mode owner dan pilih opsi '0' atau 'Exit' (!!!)\n");
        if(ShowIDPW)
        {
            printf("(!!!) Username  : admin  (!!!)\n");
            printf("(!!!) Passwrod  : 123456 (!!!)\n");
        }
        printf("                                  |||   |||    |||||    |||||||||\n");
        delay(1);
        printf("                                  |||   |||   ||| |||   |||      \n");
        delay(1);
        printf("                                  |||   |||  |||   |||  |||||||||\n");
        delay(1);
        printf("                                  |||   |||  |||||||||        |||\n");
        delay(1);
        printf("                                  |||||||||  |||   |||  |||||||||\n\n");
        delay(1);

    }
}

int meja()//menentukan nomor meja
{
    char term;
    int no;
    do
    {
        printf("Masukan Nomor Meja : ");
        if(scanf("%d%c", &no, &term) != 2 || term != '\n')//errorcek sederhana
        {
            getchar();
            printf("Nomor meja hanya dapat dimasukan angka!\n");
            delay(8);
        }
        else
        {
            system("cls");
            return no;
        }
    }
    while(1);
}

void addmenu(Data **Head, Data **Tail, char makanan[30], long long int harga, int stock)//menyimpan menu kedalam file dan linked list
{
    Data *node;
    node=(Data*) malloc(sizeof(Data));
    node->next=NULL;
    node->prev=NULL;

    strcpy(node->menu,makanan);
    node->harga=harga;
    node->stock=stock;

    if((*Head)==NULL)//membuat double linked list
    {
        (*Head)=(*Tail)=node;
        node->kode=1;
    }
    else
    {
        node->kode=((*Tail)->kode)+1;
        (*Tail)->next=node;
        node->prev=(*Tail);
        (*Tail)=node;
    }
}

int addpesanan(Pesanan **Head, Pesanan **Tail, Data **Dhead, int *history)//menambahkan pesanan baru
{
    int kode, jumlah;
    Pesanan *node;
    Data *curr;

    printf("Masukan -1 untuk membatalkan pesanan terakhir\n");
    printf("Masukan 0 untuk berhenti memesan\n");

    if(*history!=-1)//menampilkan pesanan terakhir (jika ada)
        printf("[ Pesanan terakhir = %s || Jumlah = %d ]\n", (*Tail)->menu, *history);
    printf("Kode Pesanan   : ");
    scanf("%d", &kode);
    getchar();

    if(kode==0)//menghentikan input pesanan
        return 1;
    if(kode==-1)//undo pesanan terakhir (stack)
    {
        if((*Head)==NULL)//error check
        {
            printf("Anda belum memesan apapun!");
            delay(8);
            return 0;
        }
        else if(*history==-1)//error check
        {
            printf("Tidak ada data pemesanan terakhir!");
            delay(8);
            return 0;
        }
        //bila jumlah pesanan terakhir lebih sedikit dari total pesanan yang tersimpan (tidak popstack hanya dikurangi jumlah pesanan)
        if(*history>=(*Tail)->jumlah)
        {
            if((*Head)==(*Tail))
            {
                free((*Head));
                (*Head)=(*Tail)=NULL;
                *history=-1;
            }
            else
            {
                node=(*Head);
                while(node->next!=(*Tail))
                    node=node->next;

                (*Tail)=node;
                node=node->next;
                (*Tail)->next=NULL;
                free(node);
                *history=(*Tail)->jumlah;
            }
        }
        //bila pesanan terakhir jumlahnya sama dengan data yang tersimpan maka popstack
        else
        {
            (*Tail)->jumlah-=*history;
            *history=(*Tail)->jumlah;
        }
        printf("Berhasil membatalkan pesanan terakhir");
        delay(8);
        return 0;
    }

    curr=(*Dhead);
    while(curr!=NULL)//mencari data berdasarkan kode yang dicari
    {
        if(curr->kode==kode)
            break;
        curr=curr->next;
    }
    if(curr==NULL)//error cek sederhana
    {
        printf("Tidak ada menu dengan kode tersebut !");
        delay(8);
        return 0;
    }
    else if(curr->stock==0)
    {
        printf("Menu tersebut tidak tersedia saat ini");
        delay(8);
        return 0;
    }

    printf("Jumlah Pesanan : ");
    scanf("%d", &jumlah);
    getchar();

    if(jumlah<1)//error cek sederhana
    {
        printf("Jumlah tidak boleh kurang dari 1");
        delay(8);
        return 0;
    }

    *history = jumlah;
    node=(*Head);
    while(node!=NULL)//untuk menghindari memasukan data yang sama (error cek)
    {
        if(strcmp(curr->menu,node->menu)==0)//mengubah jumlah pesanan dan melempar data kebelakang
        {
            if(node==(*Tail)||(node==(*Head)&&node==(*Tail)))
                node->jumlah+=jumlah;
            else if(node!=(*Head))
            {
                Pesanan *left, *right;
                left = node->prev;
                right= node->next;

                left->next= right;
                right->prev= left;

                (*Tail)->next=node;
                node->prev=(*Tail);
                node->next=NULL;
                (*Tail)=node;
            }
            else
            {
                Pesanan *subhead;
                subhead= node->next;
                (*Head)=subhead;
                subhead->prev=NULL;

                (*Tail)->next=node;
                node->prev=(*Tail);
                node->next=NULL;
                (*Tail)=node;
            }
            printf("Jumlah pesanan berhasil ditambahkan");

            delay(8);
            return 0;
        }
        node=node->next;
    }

    node=NULL;
    node=(Pesanan*) malloc(sizeof(Pesanan));
    node->next=node->prev=NULL;
    node->jumlah=jumlah;
    node->harga=curr->harga;
    strcpy(node->menu,curr->menu);

    if((*Head)==NULL)//membuat double linked list
        (*Head)=(*Tail)=node;
    else
    {
        (*Tail)->next=node;
        node->prev=(*Tail);
        (*Tail)=node;
    }
    printf("Pesanan berhasil dimasukan");
    delay(8);
    return 0;
}

void inputmenu(Data **Head, Data **Tail)//memasukan menu makanan baru
{
    Data *curr;
    char makanan[30];
    long long int harga;
    int stock;
    FILE *fpo;

    system("cls");
    printf("===========================\n");
    printf("||        Add Menu       ||\n");
    printf("===========================\n");
    printf("Input nama makanan: ");
    scanf("%[^\n]", makanan);
    getchar();

    printf("Input Harga : ");
    scanf("%lld", &harga);
    getchar();

    printf("Input Ketersediaan : ");
    scanf("%d", &stock);
    getchar();

    curr=(*Head);
    while(curr!=NULL)//error cek atau duplikat checker
    {
        if(strcmp(curr->menu,makanan)==0)
        {
            printf("Terdapat menu dengan nama yang sama, gagal untuk menambahkan menu\n");
            delay(10);
            return;
        }
        curr=curr->next;
    }

    addmenu(&(*Head),&(*Tail), makanan, harga, stock);//memasukan data menu baru kedalam linked list

    //memasukan data menu yang baru kedalam file
    fpo=fopen("Menu.txt","a+");
    fprintf(fpo,"#%s\n",makanan);
    fprintf(fpo,"#%lld\n",harga);
    fprintf(fpo,"#%d\n",((*Tail)->kode)+1);
    fprintf(fpo,"#%d\n\n",stock);
    fclose(fpo);

    printf("Menu berhasil ditambahkan");
    delay(8);
}

void pop(Pesanan **Head, Pesanan **Tail, Pesanan **Curr)//menghapus pesanan
{
    Pesanan *curr=(*Curr);
    if(curr==(*Head))//untuk free depan
    {
        (*Head)=(*Head)->next;
        if((*Head)!=NULL)
            (*Head)->prev=NULL;
        free(curr);
        (*Curr)=(*Head);
    }
    else if(curr==(*Tail))//untuk free belakang
    {
        (*Tail)=(*Tail)->prev;
        if((*Tail)!=NULL)
            (*Tail)->next=NULL;
        free(curr);
        (*Curr)=(*Tail);
    }
    else//untuk free tengah
    {
        (*Curr)=(*Curr)->next;
        curr->next->prev=curr->prev;
        curr->prev->next=curr->next;
        free(curr);
    }
}

void listmenu(Data **Head)//menampilkan daftar makanan yang tersedia
{
    Data *curr;

    system("cls");
    printf("=========================================================================\n");
    printf("||                             List Menu                               ||\n");
    printf("=========================================================================\n");
    printf("|%8s%8s  ", "Menu", " ");
    printf("%12s(IDR)", "Harga");
    printf("%18s  ", "Kode Pesanan");
    printf("%10s      |", "Status");
    printf("\n");
    printf("|====================  ===============  =============== ================|\n");

    curr=(*Head);
    while(curr!=NULL)
    {
        printf("| %-19s  ", curr->menu);
        printf("%15lld  ", curr->harga);
        printf("%15d", curr->kode);
        if(curr->stock==1)
            printf("    Tersedia     |\n");
        else
            printf("     Kosong      |\n");
        curr=curr->next;
    }
    printf("|====================  ===============  =============== ================|\n");
}

void listpesanan(Pesanan **Head, char menu[])//menampilkan pesanan yang sudah dipilih
{
    Pesanan *curr;
    long long int total=0;

    system("cls");
    printf("====================================================\n");
    printf("||                  List Pesanan                  ||\n");
    printf("====================================================\n");
    printf("|%-20s|", "Pesanan");
    printf("%7s   |", "Harga");
    printf("%s|", "Jumlah");
    printf(" %s |\n","Sub total");
    printf("====================================================\n");

    curr=(*Head);
    while(curr!=NULL)//output daftar pesanan
    {
        printf("|%-20s|",curr->menu);
        printf("@ Rp %5lld|",curr->harga);
        printf("%5d%1s|",curr->jumlah," ");
        printf("Rp %8lld|", (curr->jumlah)*(curr->harga));
        if(strcmp(curr->menu,menu)==0)//Digunakan dalam penunjuk pesanan saat mengedit pesanan
            printf(" <<\n");
        else
            printf("\n");
        total=total+((curr->jumlah)*(curr->harga));
        curr=curr->next;
    }
    printf("----------------------------------------------------\n");
    printf("|   Total%29s: Rp %8lld|\n"," ",total);
    printf("----------------------------------------------------\n");
}

void swap(Pesanan **a, Pesanan **b)//swap node
{
    Pesanan *node;
    node=(Pesanan*) malloc(sizeof(Pesanan));

    strcpy(node->menu,(*a)->menu);
    node->harga=(*a)->harga;
    node->jumlah=(*a)->jumlah;

    strcpy((*a)->menu,(*b)->menu);
    (*a)->harga=(*b)->harga;
    (*a)->jumlah=(*b)->jumlah;

    strcpy((*b)->menu,node->menu);
    (*b)->harga=node->harga;
    (*b)->jumlah=node->jumlah;

    free(node);
}

void sort(Pesanan **Head, Pesanan **Tail, Pesanan **Curr, int mode)//fungsi untuk sortir linked list pesanan
{
    Pesanan *curr;
    char order;
    if((*Head)==NULL)//error cek sederhana
    {
        printf("Tidak ada pesanan yang dapat disortir");
        delay(5);
        return;
    }

    curr=(*Head);
    printf("\n1. Ascending\n");
    printf("2. Descending\n");
    printf("Pilihan : ");
    order=getche();
    delay(2);

    //selection sort
    if(mode==1)//sortir berdasarkan nama
    {
        while(curr->next!=NULL)
        {
            (*Curr)=curr->next;
            while((*Curr)!=NULL)
            {
                if(strcmp(curr->menu,(*Curr)->menu)>0)//melakukan swap data pada linked list
                    swap(&curr, &(*Curr));
                (*Curr)=(*Curr)->next;
            }
            curr=curr->next;
        }
    }
    else if(mode==2)//sortir berdasarkan harga
    {
        while(curr->next!=NULL)
        {
            (*Curr)=curr->next;
            while((*Curr)!=NULL)
            {
                if(curr->harga>(*Curr)->harga)//melakukan swap data pada linked list
                    swap(&curr, &(*Curr));
                (*Curr)=(*Curr)->next;
            }
            curr=curr->next;
        }
    }
    else if(mode==3)//sortir berdasarkan jumlah
    {
        while(curr->next!=NULL)
        {
            (*Curr)=curr->next;
            while((*Curr)!=NULL)
            {
                if(curr->jumlah>(*Curr)->jumlah)//melakukan swap data pada linked list
                    swap(&curr, &(*Curr));
                (*Curr)=(*Curr)->next;
            }
            curr=curr->next;
        }
    }
    else if(mode==4)//sortir berdasarkan sub total
    {
        while(curr->next!=NULL)
        {
            (*Curr)=curr->next;
            while((*Curr)!=NULL)
            {
                if((curr->jumlah)*(curr->harga)>((*Curr)->jumlah)*((*Curr)->harga))//melakukan swap data pada linked list
                    swap(&curr, &(*Curr));
                (*Curr)=(*Curr)->next;
            }
            curr=curr->next;
        }
    }

    //melakukan reverse terhadap linked list (membalik urutan data)
    if(order=='2')
    {
        curr=(*Head);
        (*Curr)=(*Tail);
        while(curr!=(*Curr)&&(*Curr)->next!=curr)//menukar data pada linked list
        {
            swap(&curr, &(*Curr));

            curr=curr->next;
            (*Curr)=(*Curr)->prev;
        }
    }
}

void init_hash_table()
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        hash_table[i] = NULL;
    }

}

unsigned long int hash(unsigned long int pass)
{
    int num = (rand() % (9999 - 999 + 1)) + 999;
    pass *= num;
    return pass % TABLE_SIZE;
}

unsigned long int hash_table_insert(struct Password *p)
{
    unsigned long int index = hash(p->pass);
    if(hash_table[index] != NULL)
    {
        return false;
    }
    hash_table[index] = p;
    return index;
}

struct Password *hash_table_lookup(int password, char* u, unsigned long int index)
{
    if(hash_table[index] != NULL && strncmp(hash_table[index]->username, u, TABLE_SIZE)==0 && (hash_table[index]->pass == password))
    {
        return hash_table[index];
    }
    else
    {
        return NULL;
    }
}

int modes(int mode, char opsi)  //menentukan mode yang digunakan
{
    init_hash_table();

    struct Password admin = {.username="admin", .pass=123456};
    unsigned long int index = hash_table_insert(&admin); //Safe index dlm variable agar password input user dapat dicek

    if(opsi=='+'||opsi=='*')   //ganti mode
    {
        char temp_username[30];
        int temp_password;
        fflush(stdin);
        printf("\nUsername: ");
        scanf("%s", temp_username);
        printf("Password: ");
        scanf("%d", &temp_password);

        struct Password *tmp = hash_table_lookup(temp_password, temp_username, index);//pengecekan id dan password yang dimatikan
        if(tmp == NULL)
        {
            printf("Username atau password salah!\n");
            fflush(stdin);
            printf("Apakah anda ingin mencoba lagi? [+: ya , -: tidak] : ");
            char temp_opsi;
            scanf("%c", &temp_opsi);
            modes(mode, temp_opsi);
        }
        else
        {
            if(opsi=='+')
                return 1;
            else
                return 2;
        }

    }
    else if(opsi=='-')//kembali kemode normal
    {
        return 0;
    }
    else
    {
        return mode;
    }
}

void pushstaf(Gtree **root, Gtree **Curr, char *nama, int n)//Memasukan pekerja dalam binary tree
{
    Gtree *node;
    node= (Gtree *) malloc(sizeof(Gtree));
    node->fchild=NULL;
    node->nextsib=NULL;
    strcpy(node->nama, nama);

    if((*root)==NULL)
        (*root)=node;
    else if(n>0)
    {
        if(n==1)
        {
            (*Curr)=(*root);
            if((*Curr)->fchild!=NULL)
                (*Curr)=(*Curr)->fchild;
            while((*Curr)->nextsib!=NULL)
                (*Curr)=(*Curr)->nextsib;
            if((*Curr)->fchild==NULL)
            {
                (*Curr)->fchild=node;
                (*Curr)=node;
                return;
            }
            else
                (*Curr)=(*Curr)->fchild;
            while((*Curr)->nextsib!=NULL)
                (*Curr)=(*Curr)->nextsib;
        }
        (*Curr)->nextsib=node;
        (*Curr)=node;
    }
    else
    {
        if((*root)->fchild==NULL)
        {
            (*root)->fchild=node;
            (*Curr)=node;
            return;
        }
        else if(n==0)
        {
            (*Curr)=(*root);
            if((*Curr)->fchild!=NULL)
                (*Curr)=(*Curr)->fchild;
            while((*Curr)->nextsib!=NULL)
                (*Curr)=(*Curr)->nextsib;
        }
        (*Curr)->nextsib=node;
        (*Curr)=node;
    }
}

void dfs(Gtree *root)//menampilkan daftar pekerja bila sudah di inisiasi
{
    Gtree *curr;
    int i=1;
    printf("Owner   : %s\n\n", root->nama);
    root=root->fchild;
    while(root!=NULL)
    {
        printf("%d. Manager :%s\n",i++, root->nama);
        curr=root->fchild;
        if(curr!=NULL)
            printf("*Karyawan :\n");
        while(curr!=NULL)
        {
            printf("  - %s\n",curr->nama);
            curr=curr->nextsib;
        }
        root=root->nextsib;
    }
}
