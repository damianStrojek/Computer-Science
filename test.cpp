// Damian Strojek s184407
// Database structures
// File records: voltage and amperage
// index-sequential file organization
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>

#define DATAFILE "data"
#define TEMPDATAFILE "data2"
#define INDEXFILE "indexes"
#define PAGESIZE 10
#define ALFA 0.5

int recordsInPrimary = 0;           // Number of records in primary area
int recordsInOverflow = 0;          // Number of records in overflow area
int maxPrimary = 10;                // Maximum number of records in primary area
int maxOverflow = maxPrimary / 5;   // Maximum number of records in overflow area

// Description of a single index
struct Index {
	int key;	// Data
	int page;	// Number of page on which the data is located
};

// Description of a single record (row) in file with data
struct Record {
	int key;
	double voltage, amperage;	// Records
	int pointer;				// Place with specific record

	Record() {
		this->key = -1;
		this->voltage = 0;
		this->amperage = 0;
		this->pointer = -1;
	};
};

void initialize();
void addNewRecord(double voltage, double amperage, int &read, int &write);
void sort(Record buffer[], int howMany);
intw searchIndex(int key, int &read);
void createIndex(int &read, int &write);
void showIndex();
void showFile();
void reorganize(int &read, int &write);
bool deleteRecord(int key, int &read, int &write);
void update(int key, double voltage, double amperage);

int main() {




	return 0;
};

// Initialize start values
void initialize() {
	Record* buffer = new Record[PAGESIZE];
	FILE* file = fopen(DATAFILE, "wb");
	int read = 0, write = 0;

    // Creating datafile and writting empty records to it
	int i = (int) ceil (((double)(maxPrimary + maxOverflow)) / (double)PAGESIZE);
    while(i--){
        fwrite(buffer, sizeof(Record), PAGESIZE, file);
        write++;
    }

    delete[] buffer;
    fclose(plik);
    createIndex(read, write);

    std::cout << "\nSetting up initial values:\nReads = " << 
        read << ". Writes = " << write << "\n";
    
    // Changing first record so it has the lowest possible key
    addNewRecord(0.5, 1.0, read, write);
};

// Adding new record
void addNewRecord(double voltage, double amperage, int &read, int &write){
    // If overflow is full we need to reorganize
    if(recordsInOverflow == maxOverflow) reorganize(read, write);

    int key = (int)(voltage/amperage);
    if(key < 0) {
        std::cout << "You can't input a record with key less than 1.\n";
        return;
    }
    else if(!key){
        std::cout << "Record with inputted key doesn't exist.\n";
        return;
    }

    int pointer, i = 0, j, page;
    double temp, changeIndex = 0;

    Record *buffer = new Record[PAGESIZE];          // Buffer for data file
    FILE *fileRead = fopen(DATAFILE, "rb");         // Data file
    FILE *fileWrite = fopen(TEMPDATAFILE, "wb");    // Temporary file
    page = searchIndex(key, read);                  // Looking for specific page
    
    // Rewriting whole pages from primary area to temporary file (to specific page)
    for(i; i < page; i++){
        fread(buffer, sizeof(Record), PAGESIZE, fileRead);
        read++;
        fwrite(buffer, sizeof(Record), PAGESIZE, fileWrite);
        write++;
    }

    // Loading page to which we want to add record
    fread(buffer, sizeof(Record), PAGESIZE, fileRead);
    read++;

    // Checking if this page has free space
    if(buffer[PAGESIZE-1].key == -1){

    }

};
