/*
 *	Nama Program	: TekNote - Proyek Akhir Semester Kelompok 7
 *	Tanggal			: 24 Juni 2021
 *	Nomor Grup		: 7
 *	Nama Anggota	:
 *		Muhammad Irsyad Fakhruddin	- 2006468850 (tidak berkontribusi)
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
#define TRUE 1
#define FALSE 0
#define INIT_CONSOLE() HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;\
	WORD saved_attributes;\
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);\
	saved_attributes = consoleInfo.wAttributes
#define SET_COLOR(x) SetConsoleTextAttribute(hConsole, (x))
#define RESET_COLOR() SetConsoleTextAttribute(hConsole, saved_attributes)

typedef struct Node {
	char judul[50], deskripsi[1000], matkul[30];
	int tanggal, bulan, tahun; // waktu deadline
	int progress; // dalam persen (0-100)
	struct Node *next;
} Node;

typedef Node* NodePtr;

void displayMenuAndTitle(char *title_string[], int title_size, char *menu_array[], int menu_count, int *menu_selected);
void displayTitle(char *title_string[], int title_size);
void displayMenu(char *menu_array[], int menu_count, int menu_selected);
void displayMenuExit(void);
int  getInteger(int lower_bound, int upper_bound, char *message);
char *getString(char *string, int size, char *message);
char *lowercase(char *string);
void findWords(char *string, char **words, int *word_count);
void printNote(NodePtr notes, int start, int length, char *message, int selected);
void printNoteDescription(NodePtr notes, int index);
void deleteNote(NodePtr *notes, int index);
void selectNote(NodePtr notes, int note_count, int note_selected);
void displayNoteSelected(NodePtr notes, int *note_selected);
int  daysToDeadline(NodePtr notes);
void searchJudul(NodePtr notes, char *key);
void searchMatkul(NodePtr notes, char *key);
void sortNote(Node *destination, Node *source, int i_begin, int i_end, int mode);
void mergeProgressAscending(Node *destination, Node *source, int i_begin, int i_middle, int i_end);
void mergeProgressDescending(Node *destination, Node *source, int i_begin, int i_middle, int i_end);
void mergeDeadlineAscending(Node *destination, Node *source, int i_begin, int i_middle, int i_end);
void mergeDeadlineDescending(Node *destination, Node *source, int i_begin, int i_middle, int i_end);
int  emptyNoteError(NodePtr notes);
void menuViewNote(NodePtr notes);
void menuAddNote(NodePtr *notes);
void menuDeleteNote(NodePtr *notes);
void menuEditNote(NodePtr *notes);
void menuSearchNote(NodePtr notes);
void menuSortNote(NodePtr *notes);
void menuImportNote(NodePtr *notes);
void menuExportNote(NodePtr notes);

int main(void) {
	NodePtr notes = NULL, current_ptr;
	int menu_selected = 0, note_is_saved = TRUE;

	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                         TekNote                         *\n",
		"*             Proyek Akhir Semester Kelompok 7            *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};
	char *menu[] = {
		"Keluar dari program",
		"Melihat note",
		"Menambahkan note",
		"Menghapus note",
		"Mengedit note",
		"Mencari note",
		"Mengurutkan note",
		"Mengimpor note",
		"Mengekspor note"
	};

	// Program akan terus berjalan sampai user memilih opsi exit
	while (TRUE) {
		displayMenuAndTitle(title, 6, menu, 9, &menu_selected);

		switch (menu_selected) {
			case 0: // exit program
				if (note_is_saved == FALSE) {
					printf("Note belum tersimpan, tetap keluar program? (tekan y untuk konfirmasi)");
					if ((getch() | 32) != 'y')
						break;
				}
				
				while (notes != NULL) {
					current_ptr = notes->next;
					free(notes);
					notes = current_ptr;
				}
				return 0;
			case 1:
				menuViewNote(notes);
				break;
			case 2:
				menuAddNote(&notes);
				note_is_saved = FALSE;
				break;
			case 3:
				menuDeleteNote(&notes);
				note_is_saved = FALSE;
				break;
			case 4:
				menuEditNote(&notes);
				note_is_saved = FALSE;
				break;
			case 5:
				menuSearchNote(notes);
				break;
			case 6:
				//menuSortNote(&notes);
				break;
			case 7:
				//menuImportNote(&notes);
				break;
			case 8:
				menuExportNote(notes);
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
	fflush(stdin);
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

int daysToDeadline(NodePtr notes) {
	// menyimpan waktu sekarang untuk acuan waktu deadline
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	// menghitung selisih hari menuju deadline
	int selisih_tahun   = timeinfo->tm_year + 1900 - notes->tahun;
	int selisih_bulan   = timeinfo->tm_mon + 1     - notes->bulan;
	int selisih_tanggal = timeinfo->tm_mday        - notes->tanggal;
	return (selisih_tahun * 365) + (selisih_bulan * 30) + selisih_tanggal;
}

void printNote(NodePtr notes, int start, int length, char *message, int selected) {
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
	
	for (i = 1; i < start; i++) {
		notes = notes->next;
	}

	for (i = start; i < start + length; i++) {
		// menampilkan note berwarna sesuai urgency deadline dan progress
		if (i == selected) // note dipilih
			SET_COLOR(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		else if (notes->progress >= 100) // note sudah selesai
			SET_COLOR(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		else if (daysToDeadline(notes) >= -1) // deadline besok
			SET_COLOR(FOREGROUND_RED);
		else if (daysToDeadline(notes) >= -7) // deadline minggu depan
			SET_COLOR(FOREGROUND_RED | FOREGROUND_GREEN);

		printf("%50s"     , notes->judul);
		printf("|%30s"    , notes->matkul);
		printf("|    %02d", notes->tanggal);
		printf("-%02d"    , notes->bulan);
		printf("-%04d"    , notes->tahun);
		printf("|%14d%%"  , notes->progress);
		printf("|%7d"     , i);
		printf("\n");

		RESET_COLOR();
		
		notes = notes->next;
	}
}

void selectNote(NodePtr notes, int note_count, int note_selected) {
	INIT_CONSOLE();
	printNote(notes, 1, note_count, "--- ALL NOTES ---", note_selected);

	if (note_selected == 0)
		SET_COLOR(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("\nKeluar dari menu\n");
	RESET_COLOR();
}

void printNoteDescription(NodePtr notes, int index) {
// Menampilkan suatu note dengan deskripsinya
	int i;
	INIT_CONSOLE();
	// menampilkan deskripsi note berwarna sesuai urgency deadline dan progress
	if (notes->progress >= 100) // note sudah selesai
		SET_COLOR(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else if (daysToDeadline(notes) >= -1) // deadline besok
		SET_COLOR(FOREGROUND_RED);
	else if (daysToDeadline(notes) >= -7) // deadline minggu depan
		SET_COLOR(FOREGROUND_RED | FOREGROUND_GREEN);

	for (i = 1; i < index; i++) {
		notes = notes->next;
	}

	printf("\n");
	printf("JUDUL: %s\n"      , notes->judul);
	printf("MATA KULIAH: %s\n", notes->matkul);
	printf(
		"DEADLINE: %02d-%02d-%04d\n",
		notes->tanggal,
		notes->bulan,
		notes->tahun
	);
	printf("PROGRESS: %d%%\n", notes->progress);
	printf("DESKRIPSI: %s\n" , notes->deskripsi);

	RESET_COLOR();
}

void deleteNote(NodePtr *notes, int index) {
	int i;
	NodePtr temp_ptr, previous_ptr, current_ptr;
	
	if (index == 1) {
		temp_ptr = *notes;
		*notes = (*notes)->next;
		free(temp_ptr);
		return;
	}
	
	current_ptr = *notes;
	for (i = 1; i < index; i++) {
		previous_ptr = current_ptr;
		current_ptr = current_ptr->next;
	}
	temp_ptr = current_ptr;
	previous_ptr->next = current_ptr->next;
	free(temp_ptr);
}

void searchJudul(NodePtr notes, char *key) {
	int i, j, k, key_count = 0, judul_count = 0, found = FALSE, match;
	char *key_words[25], *judul_words[25], judul[50];

	lowercase(key);
	findWords(key, key_words, &key_count);

	// linar search
	for (i = 1; notes != NULL; i++) {
		match = TRUE;
		strcpy(judul, notes->judul);
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
		
		notes = notes->next;
	}

	if (found == FALSE)
		printf("NOT FOUND\n");
}

void searchMatkul(NodePtr notes, char *key) {
	int i, j, k, key_count = 0, matkul_count = 0, found = FALSE, match;
	char *key_words[25], *matkul_words[25], matkul[50];

	lowercase(key);
	findWords(key, key_words, &key_count);

	// linar search
	for (i = 1; notes != NULL; i++) {
		match = TRUE;
		strcpy(matkul, notes->matkul);
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
		
		notes = notes->next;
	}

	if (found == FALSE)
		printf("NOT FOUND\n");
}
/*
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
*/
void displayNoteSelected(NodePtr notes, int *note_selected) {
	NodePtr temp = notes;
	int note_count = 0;
	char char_input;
	
	while (temp != NULL) {
		note_count++;
		temp = temp->next;
	}
	
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

int emptyNoteError(NodePtr notes) {
	if (notes == NULL) {
		printf("ERROR: note kosong!\n");
		displayMenuExit();
		return 1;
	}
	return 0;
}

void menuViewNote(NodePtr notes) {
	if (emptyNoteError(notes))
		return;

	int note_selected = 0;
	while (TRUE) {
		displayNoteSelected(notes, &note_selected);
		if (note_selected <= 0) {
			return;
		} else {
			printNoteDescription(notes, note_selected);
			displayMenuExit();
		}
	}
}

void menuAddNote(NodePtr *notes) {
	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                         ADD MENU                        *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};
	displayTitle(title, 5);
	
	NodePtr new_ptr = malloc(sizeof(Node)), current_ptr = *notes, previous_ptr;

	// Meminta user memasukkan data-data untuk satu note
	printf("\n-----------------------------------------------------------\n\n");
	getString(new_ptr->judul,     50,   "Masukkan judul");
	getString(new_ptr->deskripsi, 1000, "Masukkan deskripsi");
	getString(new_ptr->matkul,    30,   "Masukkan mata kuliah");
	new_ptr->tahun    = getInteger(0, 9999, "Masukkan tahun deadline");
	new_ptr->bulan    = getInteger(1, 12,   "Masukkan bulan deadline");
	new_ptr->tanggal  = getInteger(1, 31,   "Masukkan tanggal deadline");
	new_ptr->progress = getInteger(0, 100,  "Masukkan persentase progress");
	new_ptr->next     = NULL;
	printf("\n-----------------------------------------------------------\n\n");
	
	if (*notes == NULL) {
		*notes = new_ptr;
		return;
	}
	
	while (current_ptr != NULL) {
		previous_ptr = current_ptr;
		current_ptr = current_ptr->next;
	}
	previous_ptr->next = new_ptr;
	
	displayMenuExit();
}

void menuDeleteNote(NodePtr *notes) {
	if (emptyNoteError(*notes))
		return;

	int note_selected;
	while (TRUE) {
		note_selected = 0;
		displayNoteSelected(*notes, &note_selected);
		
		if (note_selected <= 0) {
			return;
		} else {
			printNote(*notes, note_selected, 1, "--- YANG AKAN DIHAPUS ---", -1);
			printf("\n\nApakah anda yakin untuk menghapus note tersebut? (tekan y untuk konfirmasi)");
			if ((getch() | 32) == 'y') {
				deleteNote(notes, note_selected);
				system("cls");
				printf("Note telah dihapus.\n");
				displayMenuExit();
			}
		}
	}
}

void menuEditNote(NodePtr *notes) {
	if (emptyNoteError(*notes))
		return;

	int note_selected = 0;
	while (TRUE) {
		displayNoteSelected(*notes, &note_selected);
		if (note_selected <= 0) {
			return;
		} else {
			int i;
			NodePtr current_ptr = *notes;
			
			for (i = 1; i < note_selected; i++) {
				current_ptr = current_ptr->next;
			}
			
			// Meminta user memasukkan data-data untuk satu note
			printf("\n-----------------------------------------------------------\n\n");
			getString(current_ptr->judul,     50,   "Masukkan judul");
			getString(current_ptr->deskripsi, 1000, "Masukkan deskripsi");
			getString(current_ptr->matkul,    30,   "Masukkan mata kuliah");
			current_ptr->tahun    = getInteger(0, 9999, "Masukkan tahun deadline");
			current_ptr->bulan    = getInteger(1, 12,   "Masukkan bulan deadline");
			current_ptr->tanggal  = getInteger(1, 31,   "Masukkan tanggal deadline");
			current_ptr->progress = getInteger(0, 100,  "Masukkan persentase progress");
			printf("\n-----------------------------------------------------------\n\n");
			
			displayMenuExit();
		}
	}
}

void menuSearchNote(NodePtr notes) {
	if (emptyNoteError(notes))
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
				searchJudul(notes, key);
				displayMenuExit();
				break;
			case 2:
				getString(key, 30, "Masukkan mata kuliah yang ingin dicari");
				searchMatkul(notes, key);
				displayMenuExit();
				break;
		}
	}
}
/*
void menuSortNote(NodePtr *notes) {
	if (emptyNoteError(*notes))
		return;

	int i, note_count = 0, menu_selected = 0;
	NodePtr temp = *notes;
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
	
	while (temp != NULL) {
		note_count++;
		temp = temp->next;
	}
	
	// Array untuk merge sort
	Node note_array[] = malloc(sizeof(Node) * note_count),
		sort_array[] = malloc(sizeof(Node) * note_count);

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
				free(note_array);
				free(sort_array);
				break;
		}
	}
}

void menuImportNote(NodePtr *notes) {
	char file_name[64];
	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                       IMPORT MENU                       *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};
	NodePtr new_ptr = malloc(sizeof(Node)), current_ptr;

	displayTitle(title, 5);
	printf("\n-----------------------------------------------------------\n\n");
	getString(file_name, 64, "Masukkan nama file (tanpa ekstensi)");
	strcat(file_name, ".teknote");

	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("ERROR: tidak dapat membuka file!\n");
		displayMenuExit();
		return;
	}

	int i;
	char buff[2000];

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

	while (*notes != NULL){
		current_ptr = (*notes)->next;
		free(*notes);
		*notes = current_ptr;
	}
	
	while (fgets(buff, 2000, fptr) != NULL) {
		sscanf(
			buff,
			"%[^,],%[^,],%[^,],%d,%d,%d,%d",
			new_ptr->judul,
			new_ptr->deskripsi,
			new_ptr->matkul,
			new_ptr->tanggal,
			new_ptr->bulan,
			new_ptr->tahun,
			new_ptr->progress
		);
		new_ptr->next = NULL;
		
		if (*notes == NULL) {
			*notes = new_ptr;
			current_ptr = *notes;
			return;
		}
		
		return;
		current_ptr->next = new_ptr;
		current_ptr = current_ptr->next;
	}
	
	fclose(fptr);
	printf("Import berhasil!\n");
	printf("\n-----------------------------------------------------------\n\n");
	displayMenuExit();
}
*/
void menuExportNote(NodePtr notes) {
	if (emptyNoteError(notes))
		return;

	int i;
	char file_name[64];
	char *title[] = {
		"***********************************************************\n",
		"*                                                         *\n",
		"*                       EXPORT MENU                       *\n",
		"*                                                         *\n",
		"***********************************************************\n"
	};

	displayTitle(title, 5);
	printf("\n-----------------------------------------------------------\n\n");
	getString(file_name, 64, "Masukkan nama file (tanpa ekstensi)");
	strcat(file_name, ".teknote");

	FILE *fptr = fopen(file_name, "w");
	if (fptr == NULL) {
		printf("Error: creating file failed\n");
		displayMenuExit();
		return;
	}

	fprintf(fptr, "JUDUL,DESKRIPSI,MATA KULIAH,TANGGAL,BULAN,TAHUN,PROGRESS\n");
	for (; notes != NULL; notes = notes->next) {
		fprintf(
			fptr,
			"%s,%s,%s,%d,%d,%d,%d\n",
			notes->judul,
			notes->deskripsi,
			notes->matkul,
			notes->tanggal,
			notes->bulan,
			notes->tahun,
			notes->progress
		);
	}
	
	fclose(fptr);
	printf("Export berhasil!\n");
	printf("\n-----------------------------------------------------------\n\n");
	displayMenuExit();
}
