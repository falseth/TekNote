/*
 *	Nama Program	: TekNote - Proyek Tengah Semester Kelompok 12
 *	Tanggal			: 4 April 2021
 *	Nomor Grup		: 12
 *	Nama Anggota	:
 *		Muhammad Irsyad Fakhruddin	- 2006468850
 *		Muhammad Roland Maulana		- 2006520784
 *		Yehezkiel Jonatan			- 2006520235
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define MAX_NOTE_COUNT 100
#define TRUE 1
#define FALSE 0
#define INIT_CONSOLE() HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;\
	WORD saved_attributes;\
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);\
	saved_attributes = consoleInfo.wAttributes
#define SET_COLOR(x) SetConsoleTextAttribute(hConsole, (x))
#define RESET_COLOR() SetConsoleTextAttribute(hConsole, saved_attributes)

typedef struct {
	char judul[50], deskripsi[1000], matkul[30];
	int tanggal, bulan, tahun; // waktu deadline
	int progress; // dalam persen (0-100)
} Note;

void displayMenuAndTitle(char *title_string[], int title_size, char *menu_array[], int menu_count, int *menu_selected);
void displayTitle(char *title_string[], int title_size);
void displayMenu(char *menu_array[], int menu_count, int menu_selected);
void displayMenuExit(void);
int  getInteger(int lower_bound, int upper_bound, char *message);
char *getString(char *string, int size, char *message);
char *lowercase(char *string);
void findWords(char *string, char **words, int *word_count);
void printNote(Note *notes, int start, int length, char *message, int selected);
void printNoteDescription(Note *notes, int index);
void addNote(Note *notes, int index);
void deleteNote(Note *notes, int index, int note_count);
void selectNote(Note *notes, int note_count, int note_selected);
void displayNoteSelected(Note *notes, int note_count, int *note_selected);
int  deadlineCompare(Note note1, Note note2);
void searchJudul(Note *notes, int note_count, char *key);
void searchMatkul(Note *notes, int size, char *key);
void sortNote(Note *destination, Note *source, int i_begin, int i_end, int mode);
void mergeProgressAscending(Note *destination, Note *source, int i_begin, int i_middle, int i_end);
void mergeProgressDescending(Note *destination, Note *source, int i_begin, int i_middle, int i_end);
void mergeDeadlineAscending(Note *destination, Note *source, int i_begin, int i_middle, int i_end);
void mergeDeadlineDescending(Note *destination, Note *source, int i_begin, int i_middle, int i_end);
int  emptyNoteError(int note_count);
int  maxNoteError(int note_count);
void menuViewNote(Note *notes, int note_count);
void menuAddNote(Note *notes, int *note_count);
void menuDeleteNote(Note *notes, int *note_count);
void menuEditNoteProgress(Note *notes, int *note_count);
void menuSearchNote(Note *notes, int note_count);
void menuSortNote(Note *notes, int note_count);
void menuImportNote(Note *notes, int *note_count);
void menuExportNote(Note *notes, int note_count);

int main(void) {
	Note notes[MAX_NOTE_COUNT + 1]; // notes disimpan mulai dari index 1
	int note_count = 0, menu_selected = 0, note_is_saved = TRUE;

	time_t rawtime;
	struct tm *timeinfo;

	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                         TekNote                         *\n",
		"*            Proyek Tengah Semester Kelompok 12           *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};
	char *menu[] = {
		"Keluar dari program",
		"Melihat note",
		"Menambahkan note",
		"Menghapus note",
		"Mengedit progress",
		"Mencari note",
		"Mengurutkan note",
		"Mengimpor note",
		"Mengekspor note"
	};

	// Program akan terus berjalan sampai user memilih opsi exit
	while (TRUE) {
		// menyimpan waktu sekarang di notes[0] untuk acuan waktu deadline
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		notes[0].tahun   = timeinfo->tm_year + 1900;
		notes[0].bulan   = timeinfo->tm_mon + 1;
		notes[0].tanggal = timeinfo->tm_mday;

		displayMenuAndTitle(title, 6, menu, 9, &menu_selected);

		switch (menu_selected) {
			case 0: // exit program
				if (note_is_saved == FALSE) {
					printf("Note belum tersimpan, tetap keluar program? (tekan y untuk konfirmasi)");
					if (getch() != 'y')
						break;
				}
				return 0;
			case 1:
				menuViewNote(notes, note_count);
				break;
			case 2:
				menuAddNote(notes, &note_count);
				note_is_saved = FALSE;
				break;
			case 3:
				menuDeleteNote(notes, &note_count);
				note_is_saved = FALSE;
				break;
			case 4:
				menuEditNoteProgress(notes, &note_count);
				note_is_saved = FALSE;
				break;
			case 5:
				menuSearchNote(notes, note_count);
				break;
			case 6:
				menuSortNote(notes, note_count);
				break;
			case 7:
				menuImportNote(notes, &note_count);
				break;
			case 8:
				menuExportNote(notes, note_count);
				note_is_saved = TRUE;
				break;
		}
	}
}

void displayMenuAndTitle(
	char *title_string[],
	int title_size,
	char *menu_array[],
	int menu_count,
	int *menu_selected
) {
	char char_input;
	do {
		system("cls");
		displayTitle(title_string, title_size);
		displayMenu(menu_array, menu_count, *menu_selected);
		printf("Gunakan W dan S untuk navigasi.\n");
		printf("Tekan Enter untuk memilih menu. ");

		// Mengatur perpindahan menu yang dipilih
		char_input = getch();
		if (char_input == 'w' || char_input == 'W') {
			if (*menu_selected <= 0)
				*menu_selected = menu_count - 1;
			else
				(*menu_selected)--;
		} else if (char_input == 's' || char_input == 'S') {
			if (*menu_selected >= menu_count - 1)
				*menu_selected = 0;
			else
				(*menu_selected)++;
		}
	} while (char_input != '\r');
	system("cls");
}

void displayTitle(char *title_string[], int title_size) {
	int i;
	INIT_CONSOLE();

	SET_COLOR(BACKGROUND_BLUE | BACKGROUND_INTENSITY);
	for (i = 0; i < title_size; i++)
		printf(title_string[i]);
	RESET_COLOR();
}

void displayMenu(char *menu_array[], int menu_count, int menu_selected) {
	int i;
	INIT_CONSOLE();

	printf("\n-----------------------------------------------------------\n\n");
	for (i = 0; i < menu_count; i++) {
		if (menu_selected == i) {
			SET_COLOR(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			printf("--> %s\n", menu_array[i]);
			RESET_COLOR();
		}
		else
			printf("    %s\n", menu_array[i]);
	}
	printf("\n-----------------------------------------------------------\n\n");
}

void displayMenuExit(void) {
	printf("\n\npress any key to continue...");
	getch();
	system("cls");
}

int getInteger(int lower_bound, int upper_bound, char *message) {
	int integer, scanf_return;

	// Terus meminta integer sampai input user valid
	do {
		printf(
			"%s ( %d sampai dengan %d ): ",
			message,
			lower_bound,
			upper_bound
		);

		// seharusnya scanf me-return nilai 1 jika input valid
		scanf_return = scanf("%d", &integer);
		fflush(stdin); // membersihkan input buffer

		if (scanf_return != 1) {
			printf("Input invalid!\n\n");
		} else if (integer < lower_bound) {
			printf("Angka terlalu kecil!\n\n");
		} else if (integer > upper_bound) {
			printf("Angka terlalu besar!\n\n");
		}
	} while (scanf_return != 1 || integer < lower_bound || integer > upper_bound);

	return integer;
}

char *getString(char *string, int size, char *message) {
	int i, length;

	do {
		printf("%s: ", message);
		fgets(string, size, stdin); // memasukkan input ke string
		sscanf(string, "%[^\n]", string); // memformat string
		fflush(stdin); // membersihkan input buffer

		// memastikan string tidak kosong
		length = strlen(string);
		if (length <= 1 && string[0] == '\n') {
			printf("ERROR: input tidak boleh kosong!\n\n");
			i = !length;
			continue;
		}

		// memastikan string tidak ada tanda koma
		for (i = 0; i < length; i++) {
			if (string[i] == ',') {
				printf("ERROR: input tidak boleh ada tanda koma (,)!\n\n");
				break;
			}
		}
	} while (i != length);

	return string;
}

char *lowercase(char *string) {
	int i;
	for (i = 0; string[i] != '\0'; i++)
		string[i] = tolower(string[i]);
	return string;
}

void findWords(char *string, char **words, int *word_count) {
// Mencari kata-kata pada string
// words adalah pointer ke kata-kata yang ada pada string
// word_count merupakan jumlah kata yang ada di words
	int i, j = 0, length = strlen(string), word_begin, mode = 0;
	*word_count = 0;

	for (i = 0; i < length + 1; i++) {
		// mode 0: mencari index pertama kata (word_begin)
		if (mode == 0 && string[i] != '\0' && string[i] != ' ') {
			word_begin = i;
			mode = 1;
		// mode 1: mencari akhir kata
		} else if (string[i] == '\0' || string[i] == ' ') {
			string[i] = '\0';
			words[j] = &string[word_begin];
			(*word_count)++;
			j++;
			mode = 0;
		}
	}
}

int deadlineCompare(Note note1, Note note2) {
	int selisih_tahun   = note1.tahun   - note2.tahun;
	int selisih_bulan   = note1.bulan   - note2.bulan;
	int selisih_tanggal = note1.tanggal - note2.tanggal;
	return (selisih_tahun * 365) + (selisih_bulan * 30) + selisih_tanggal;
}

void printNote(Note *notes, int start, int length, char *message, int selected) {
// menampilkan judul, matkul, deadline, dan progress
// start adalah index pertama note yang ditampilkan
// length adalah banyaknya notes yang ditampilkan

	int i;
	INIT_CONSOLE();

	// Jika tidak ada message, maka bagian title dan header tidak akan ditampilkan
	if (strlen(message) != 0) {
		printf("\n%90s\n\n", message);
		printf("%50s" , "JUDUL");
		printf("|%30s", "MATA KULIAH");
		printf("|%14s", "DEADLINE");
		printf("|%15s", "PROGRESS");
		printf("|%7s" , "INDEX");
		printf("\n");
	}

	for (i = start; i < start + length; i++) {
		// menampilkan note berwarna sesuai urgency deadline dan progress
		int comparison = deadlineCompare(notes[0], notes[i]);
		if (i == selected)
			SET_COLOR(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		else if (notes[i].progress >= 100)
			SET_COLOR(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		else if (comparison >= -1)
			SET_COLOR(FOREGROUND_RED);
		else if (comparison >= -7)
			SET_COLOR(FOREGROUND_RED | FOREGROUND_GREEN);

		printf("%50s"     , notes[i].judul);
		printf("|%30s"    , notes[i].matkul);
		printf("|    %02d", notes[i].tanggal);
		printf("-%02d"    , notes[i].bulan);
		printf("-%04d"    , notes[i].tahun);
		printf("|%14d%%"  , notes[i].progress);
		printf("|%7d"     , i);
		printf("\n");

		RESET_COLOR();
	}
}

void selectNote(Note *notes, int note_count, int note_selected) {
	INIT_CONSOLE();
	printNote(notes, 1, note_count, "--- ALL NOTES ---", note_selected);

	if (note_selected == 0)
		SET_COLOR(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("\nKeluar dari menu\n");
	RESET_COLOR();
}

void printNoteDescription(Note *notes, int index) {
// Menampilkan suatu note dengan deskripsinya
	INIT_CONSOLE();
	// menampilkan deskripsi note berwarna sesuai urgency deadline dan progress
	int comparison = deadlineCompare(notes[0], notes[index]);
	if (notes[index].progress >= 100)
		SET_COLOR(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else if (comparison >= -1)
		SET_COLOR(FOREGROUND_RED);
	else if (comparison >= -7)
		SET_COLOR(FOREGROUND_RED | FOREGROUND_GREEN);

	printf("\n");
	printf("JUDUL: %s\n"      , notes[index].judul);
	printf("MATA KULIAH: %s\n", notes[index].matkul);
	printf(
		"DEADLINE: %02d-%02d-%04d\n",
		notes[index].tanggal,
		notes[index].bulan,
		notes[index].tahun
	);
	printf("PROGRESS: %d%%\n", notes[index].progress);
	printf("DESKRIPSI: %s\n" , notes[index].deskripsi);

	RESET_COLOR();
}

void addNote(Note *notes, int index) {
	int tahun_skrg   = notes[0].tahun;
	int bulan_skrg   = notes[0].bulan;
	int tanggal_skrg = notes[0].tanggal;

	// Meminta user memasukkan data-data untuk satu note
	printf("\n-----------------------------------------------------------\n\n");
	getString(notes[index].judul,     50,   "Masukkan judul");
	getString(notes[index].deskripsi, 1000, "Masukkan deskripsi");
	getString(notes[index].matkul,    30,   "Masukkan mata kuliah");

	// Memastikan user tidak dapat memasukkan deadline yang sudah berlalu
	notes[index].tahun = getInteger(tahun_skrg, tahun_skrg + 5, "Masukkan tahun deadline");
	if (notes[index].tahun == tahun_skrg) {
		notes[index].bulan = getInteger(bulan_skrg, 12, "Masukkan bulan deadline");
		if (notes[index].bulan == bulan_skrg)
			notes[index].tanggal = getInteger(tanggal_skrg, 31, "Masukkan tanggal deadline");
		else
			notes[index].tanggal = getInteger(1, 31, "Masukkan tanggal deadline");
	} else {
		notes[index].bulan = getInteger(1, 12, "Masukkan bulan deadline");
		notes[index].tanggal = getInteger(1, 31, "Masukkan tanggal deadline");
	}

	notes[index].progress = getInteger(0, 100, "Masukkan persentase progress");
	printf("\n-----------------------------------------------------------\n\n");
}

void deleteNote(Note *notes, int index, int note_count) {
	int i;
	// menindihkan (overwriting) note pada index sehingga terhapus
	for (i = index; i <= note_count; i++) {
		notes[i] = notes[i + 1];
	}
}

void searchJudul(Note *notes, int note_count, char *key) {
	int i, j, k, key_count = 0, judul_count = 0, found = FALSE, match;
	char *key_words[25], *judul_words[25], judul[50];

	lowercase(key);
	findWords(key, key_words, &key_count);

	// linar search
	for (i = 1; i <= note_count; i++) {
		match = TRUE;
		strcpy(judul, notes[i].judul);
		lowercase(judul);
		findWords(judul, judul_words, &judul_count);

		for (j = 0; j < key_count; j++) {
			for (k = 0; k < judul_count; k++) {
				if (strcmp(key_words[j], judul_words[k]) == 0) {
					break;
				}
			}
			if (k == judul_count) {
				match = FALSE;
				break;
			}
		}

		if (match == TRUE && found == FALSE) {
			found = TRUE;
			printNote(notes, i, 1, "--- FOUND ---", -1);
		} else if (match == TRUE) {
			printNote(notes, i, 1, "", -1);
		}
	}

	if (found == FALSE)
		printf("NOT FOUND\n");
}

void searchMatkul(Note *notes, int note_count, char *key) {
	int i, j, k, key_count = 0, matkul_count = 0, found = FALSE, match;
	char *key_words[25], *matkul_words[25], matkul[50];

	lowercase(key);
	findWords(key, key_words, &key_count);

	// linar search
	for (i = 1; i <= note_count; i++) {
		match = TRUE;
		strcpy(matkul, notes[i].matkul);
		lowercase(matkul);
		findWords(matkul, matkul_words, &matkul_count);

		for (j = 0; j < key_count; j++) {
			for (k = 0; k < matkul_count; k++) {
				if (strcmp(key_words[j], matkul_words[k]) == 0) {
					break;
				}
			}
			if (k == matkul_count) {
				match = FALSE;
				break;
			}
		}

		if (match == TRUE && found == FALSE) {
			found = TRUE;
			printNote(notes, i, 1, "--- FOUND ---", -1);
		} else if (match == TRUE) {
			printNote(notes, i, 1, "", -1);
		}
	}

	if (found == FALSE)
		printf("NOT FOUND\n");
}

void sortNote(Note *destination, Note *source, int i_begin, int i_end, int mode) {
	if (i_end - i_begin <= 1)
		return;

	int i_middle = (i_begin + i_end) / 2;

	sortNote(source, destination, i_begin, i_middle, mode);
	sortNote(source, destination, i_middle, i_end, mode);

	switch (mode) {
		case 0:
			mergeDeadlineAscending(destination, source, i_begin, i_middle, i_end);
			break;
		case 1:
			mergeDeadlineDescending(destination, source, i_begin, i_middle, i_end);
			break;
		case 2:
			mergeProgressAscending(destination, source, i_begin, i_middle, i_end);
			break;
		case 3:
			mergeProgressDescending(destination, source, i_begin, i_middle, i_end);
			break;
	}
}

void mergeProgressAscending(Note *destination, Note *source, int i_begin, int i_middle, int i_end) {
	int i = i_begin, j = i_middle, k;
	for (k = i_begin; k < i_end; k++) {
		if (i < i_middle && (j >= i_end || source[i].progress <= source[j].progress)) {
			destination[k] = source[i];
			i++;
		} else {
			destination[k] = source[j];
			j++;
		}
	}
}

void mergeProgressDescending(Note *destination, Note *source, int i_begin, int i_middle, int i_end) {
	int i = i_begin, j = i_middle, k;
	for (k = i_begin; k < i_end; k++) {
		if (i < i_middle && (j >= i_end || source[i].progress >= source[j].progress)) {
			destination[k] = source[i];
			i++;
		} else {
			destination[k] = source[j];
			j++;
		}
	}
}

void mergeDeadlineAscending(Note *destination, Note *source, int i_begin, int i_middle, int i_end) {
	int i = i_begin, j = i_middle, k;
	for (k = i_begin; k < i_end; k++) {
		if (i < i_middle && (j >= i_end || deadlineCompare(source[i], source[j]) <= 0)) {
			destination[k] = source[i];
			i++;
		} else {
			destination[k] = source[j];
			j++;
		}
	}
}

void mergeDeadlineDescending(Note *destination, Note *source, int i_begin, int i_middle, int i_end) {
	int i = i_begin, j = i_middle, k;
	for (k = i_begin; k < i_end; k++) {
		if (i < i_middle && (j >= i_end || deadlineCompare(source[i], source[j]) >= 0)) {
			destination[k] = source[i];
			i++;
		} else {
			destination[k] = source[j];
			j++;
		}
	}
}

void displayNoteSelected(Note *notes, int note_count, int *note_selected) {
	char char_input;
	do {
		system("cls");
		selectNote(notes, note_count, *note_selected);
		char_input = getch();
		if (char_input == 'w' || char_input == 'W') {
			if (*note_selected <= 0)
				*note_selected = note_count;
			else
				(*note_selected)--;
		} else if (char_input == 's' || char_input == 'S') {
			if (*note_selected >= note_count)
				*note_selected = 0;
			else
				(*note_selected)++;
		}
	} while (char_input != '\r');
	system("cls");
}

int emptyNoteError(int note_count) {
	if (note_count <= 0) {
		printf("ERROR: note kosong!\n");
		displayMenuExit();
		return 1;
	}
	return 0;
}

int maxNoteError(int note_count) {
	if (note_count >= MAX_NOTE_COUNT) {
		printf("ERROR: note penuh!\n");
		displayMenuExit();
		return 1;
	}
	return 0;
}

void menuViewNote(Note *notes, int note_count) {
	if (emptyNoteError(note_count))
		return;

	int note_selected = 0;
	while (TRUE) {
		displayNoteSelected(notes, note_count, &note_selected);
		if (note_selected <= 0) {
			return;
		} else {
			printNoteDescription(notes, note_selected);
			displayMenuExit();
		}
	}
}

void menuAddNote(Note *notes, int *note_count) {
	if (maxNoteError(*note_count))
		return;

	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                         ADD MENU                        *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};
	displayTitle(title, 5);
	addNote(notes, (*note_count) + 1);
	(*note_count)++;
	displayMenuExit();
}

void menuDeleteNote(Note *notes, int *note_count) {
	if (emptyNoteError(*note_count))
		return;

	int note_selected = 0;
	while (TRUE) {
		displayNoteSelected(notes, *note_count, &note_selected);
		if (note_selected <= 0) {
			return;
		} else {
			printNote(notes, note_selected, 1, "--- YANG AKAN DIHAPUS ---", -1);
			printf("\n\nApakah anda yakin untuk menghapus note tersebut? (tekan y untuk konfirmasi)");
			if (getch() == 'y') {
				deleteNote(notes, note_selected, *note_count);
				if (note_selected == *note_count)
					note_selected--;
				(*note_count)--;
				system("cls");
				printf("Note telah dihapus.\n");
				displayMenuExit();
			}
		}
	}
}

void menuEditNoteProgress(Note *notes, int *note_count) {
	if (emptyNoteError(*note_count))
		return;

	int note_selected = 0;
	while (TRUE) {
		displayNoteSelected(notes, *note_count, &note_selected);
		if (note_selected <= 0) {
			return;
		} else {
			notes[note_selected].progress = getInteger(0, 100, "Masukkan persentase progress baru");
		}
	}
}

void menuSearchNote(Note *notes, int note_count) {
	if (emptyNoteError(note_count))
		return;

	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                       SEARCH MENU                       *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};
	char *menu[] = {
		"Keluar dari menu",
		"Mencari judul",
		"Mencari mata kuliah"
	};
	int menu_selected = 0;
	while (TRUE) {
		char key[50]; // string yang dicari
		displayMenuAndTitle(title, 5, menu, 3, &menu_selected);
		switch (menu_selected) {
			case 0:
				return;
			case 1:
				getString(key, 50, "Masukkan judul yang ingin dicari");
				searchJudul(notes, note_count, key);
				displayMenuExit();
				break;
			case 2:
				getString(key, 30, "Masukkan mata kuliah yang ingin dicari");
				searchMatkul(notes, note_count, key);
				displayMenuExit();
				break;
		}
	}
}

void menuSortNote(Note *notes, int note_count) {
	if (emptyNoteError(note_count))
		return;

	int i, menu_selected = 0;
	Note sort_array[MAX_NOTE_COUNT + 1]; // array untuk merge sort
	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                        SORT MENU                        *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};
	char *menu[] = {
		"Keluar dari menu",
		"Mengurutkan berdasarkan deadline (ascending)",
		"Mengurutkan berdasarkan deadline (descending)",
		"Mengurutkan berdasarkan progress (ascending)",
		"Mengurutkan berdasarkan progress (descending)"
	};

	while (TRUE) {
		displayMenuAndTitle(title, 5, menu, 5, &menu_selected);
		for(i = 1; i <= note_count; i++) {
			sort_array[i] = notes[i];
		}
		switch (menu_selected) {
			case 0:
				return;
			case 1:
				sortNote(notes, sort_array, 1, note_count + 1, 0);
				printNote(notes, 1, note_count, "--- DEADLINE ASCENDING ---", 0);
				displayMenuExit();
				break;
			case 2:
				sortNote(notes, sort_array, 1, note_count + 1, 1);
				printNote(notes, 1, note_count, "--- DEADLINE DESCENDING ---", 0);
				displayMenuExit();
				break;
			case 3:
				sortNote(notes, sort_array, 1, note_count + 1, 2);
				printNote(notes, 1, note_count, "--- PROGRESS ASCENDING ---", 0);
				displayMenuExit();
				break;
			case 4:
				sortNote(notes, sort_array, 1, note_count + 1, 3);
				printNote(notes, 1, note_count, "--- PROGRESS DESCENDING ---", 0);
				displayMenuExit();
				break;
		}
	}
}

void menuImportNote(Note *notes, int *note_count) {
	char file_name[50];
	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                       IMPORT MENU                       *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};

	displayTitle(title, 5);
	printf("\n-----------------------------------------------------------\n\n");
	getString(file_name, 50, "Masukkan nama file");

	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("ERROR: tidak dapat membuka file!\n");
		displayMenuExit();
		return;
	}

	int i;
	char buff[2000];
	*note_count = 0;

	if (fgets(buff, 2000, fptr) == NULL) {
		printf("ERROR: file kosong!\n");
		displayMenuExit();
		return;
	}

	if (strcmp(buff, "JUDUL,DESKRIPSI,MATA KULIAH,TANGGAL,BULAN,TAHUN,PROGRESS\n") != 0) {
		printf("ERROR: format file tidak sesuai!\n");
		displayMenuExit();
		return;
	}

	for (i = 1; fgets(buff, 2000, fptr) != NULL; i++) {
		if (maxNoteError(*note_count))
			return;

		sscanf(
			buff,
			"%[^,],%[^,],%[^,],%d,%d,%d,%d",
			notes[i].judul,
			notes[i].deskripsi,
			notes[i].matkul,
			&notes[i].tanggal,
			&notes[i].bulan,
			&notes[i].tahun,
			&notes[i].progress
		);

		(*note_count)++;
	}
	fclose(fptr);
	printf("Import berhasil!\n");
	printf("\n-----------------------------------------------------------\n\n");
	displayMenuExit();
}

void menuExportNote(Note *notes, int note_count) {
	if (emptyNoteError(note_count))
		return;

	int i;
	char file_name[50];
	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                       EXPORT MENU                       *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};

	displayTitle(title, 5);
	printf("\n-----------------------------------------------------------\n\n");
	getString(file_name, 50, "Masukkan nama file");

	FILE *fptr = fopen(file_name, "w");
	if (fptr == NULL) {
		printf("Error: creating file failed\n");
		displayMenuExit();
		return;
	}

	fprintf(fptr, "JUDUL,DESKRIPSI,MATA KULIAH,TANGGAL,BULAN,TAHUN,PROGRESS\n");
	for (i = 1; i <= note_count; i++) {
		fprintf(
			fptr,
			"%s,%s,%s,%d,%d,%d,%d\n",
			notes[i].judul,
			notes[i].deskripsi,
			notes[i].matkul,
			notes[i].tanggal,
			notes[i].bulan,
			notes[i].tahun,
			notes[i].progress
		);
	}
	fclose(fptr);
	printf("Export berhasil!\n");
	printf("\n-----------------------------------------------------------\n\n");
	displayMenuExit();
}
