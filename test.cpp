// Damian Strojek s184407
// Database structures
// File records: voltage and amperage
// index-sequential file organization
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>

#define DATAFILE "data"
#define INDEXFILE "indexes"
#define PAGESIZE 10
#define ALFA 0.5

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

int main() {




	return 0;
};

void initialize() {
	Record* buffer = new Record[PAGESIZE];
	FILE* file = fopen(DATAFILE, "wb");
	int read = 0, write = 0;

	

};
