#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
#define JUMLAH_NOTES_MAKSIMAL 100

typedef struct{
	char judul[30], deskripsi[100], matkul[30];
	int tanggal, bulan, tahun, progress;
}Notes;

void lihatNotes();
void tambahNote();
void hapusNote();
void editProgress();
void cariNote();
void urutkanNote();
void imporNote();
void eksporNote();
void tampilanMenu(int *menu_pilihan);

int main(){
	
	int menu_pilihan = 0, jumlah_notes = 0, note_tersimpan;
	Notes notes[JUMLAH_NOTES_MAKSIMAL+1];
	
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	notes[0].tahun   = timeinfo->tm_year + 1900;
	notes[0].bulan   = timeinfo->tm_mon + 1;
	notes[0].tanggal = timeinfo->tm_mday;
		
	tampilanMenu(&menu_pilihan);
	
	switch(menu_pilihan){
		case 0:
			break;
			return 0;
		case 1:
			lihatNotes();
			break;
		case 2:
			tambahNote();
			break;
		case 3:
			hapusNote();
			break;
		case 4:
			editProgress();
			break;
		case 5:
			cariNote();
			break;
		case 6:
			urutkanNote();
			break;
		case 7:
			imporNote();
			break;
		case 8:
			eksporNote();
			break;	
	}	
}

void tampilanMenu(int *menu_pilihan){
	system("cls");
	printf("0. Keluar dari program\n1. Melihat notes\n2. Menambahkan note\n3. Menghapus note\n4. Mengedit progress\n5. Mencari note\n6. Mengurutkan note\n7. Mengimpor note\n8. Mengekspor note\n\nInput:");
		scanf("%d", &menu_pilihan);
	system("cls");
}
