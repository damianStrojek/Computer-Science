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
#define RECORDSIZE sizeof(Record)
#define INDEXSIZE sizeof(Index)

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
void addNewRecord(double voltage, double amperage, int& read, int& write);
void sort(Record buffer[], int howMany);
int searchIndex(int key, int& read);
void createIndex(int& read, int& write);
void showIndex();
void showFile();
void reorganize(int& read, int& write);
bool deleteRecord(int key, int& read, int& write);
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
    int i = (int) ceil (((double) (maxPrimary + maxOverflow)) / (double) PAGESIZE);
    while (i--) {
        fwrite(buffer, RECORDSIZE, PAGESIZE, file);
        write++;
    }

    delete[] buffer;
    fclose(file);
    createIndex(read, write);

    std::cout << "\nSetting up initial values:\nReads = " <<
        read << ". Writes = " << write << "\n";

    // Changing first record so it has the lowest possible key
    addNewRecord(0.5, 1.0, read, write);
};

// Adding new record
void addNewRecord(double voltage, double amperage, int& read, int& write) {
    // If overflow is full we need to reorganize
    if (recordsInOverflow == maxOverflow) reorganize(read, write);

    int key = (int)(voltage / amperage);
    if (key < 0) {
        std::cout << "You can't input a record with key less than 1.\n";
        return;
    }
    else if (!key) {
        std::cout << "Record with inputted key doesn't exist.\n";
        return;
    }

    int pointer, i = 0, j, page;
    double temp, changeIndex = 0;

    Record* buffer = new Record[PAGESIZE];          // Buffer for data file
    FILE* fileRead = fopen(DATAFILE, "rb");         // Data file
    FILE* fileWrite = fopen(TEMPDATAFILE, "wb");    // Temporary file
    page = searchIndex(key, read);                  // Looking for specific page

    // Rewriting whole pages from primary area to temporary file (to specific page)
    for (i; i < page; i++) {
        fread(buffer, RECORDSIZE, PAGESIZE, fileRead);
        read++;
        fwrite(buffer, RECORDSIZE, PAGESIZE, fileWrite);
        write++;
    }

    // Loading page to which we want to add record
    fread(buffer, RECORDSIZE, PAGESIZE, fileRead);
    read++;

    // Checking if this page has free space
    if (buffer[PAGESIZE - 1].key == -1) {
        // Adding new record
        for (i = 0; i < PAGESIZE; i++) {
            // Key already exists
            if (buffer[i].key == key) {
                std::cout << "Record with that key already exists.\n";
                fclose(fileRead);
                fclose(fileWrite);
                remove(TEMPDATAFILE);
                delete[] buffer;
                return;
            }
            // Adding to blank page
            else if (buffer[i].key == -1) {
                // If this is first record on the page we need to update index
                if (i == 0) changeIndex = 1;
                buffer[i].key = key;
                buffer[i].voltage = voltage;
                buffer[i].amperage = amperage;
                buffer[i].pointer = -1;
                recordsInPrimary++;
                
                sort(buffer, i);
                // Save page that was changed
                fwrite(buffer, RECORDSIZE, PAGESIZE, fileWrite);
                write++;
                // Rewrite rest of the primary area
                for (i = 0; i < page; i++) {
                    fread(buffer, RECORDSIZE, PAGESIZE, fileRead);
                    read++;
                    fwrite(buffer, RECORDSIZE, PAGESIZE, fileWrite);
                    write++;
                }
                // Rewrite overflow
                i = (int)ceil((double)maxOverflow / (double)PAGESIZE);
                for (int j = 0; j < i; j++) {
                    fread(buffer, RECORDSIZE, PAGESIZE, fileRead);
                    read++;
                    fwrite(buffer, RECORDSIZE, PAGESIZE, fileWrite);
                    write++;
                }

                delete[] buffer;
                fclose(fileRead);
                fclose(fileWrite);
                remove(DATAFILE);
                rename(TEMPDATAFILE, DATAFILE);
                break;
            }
        }
        if (changeIndex) createIndex(read, write);
    }
    // Adding to overflow if there is no free space
    else {
        for (i = 0; i < PAGESIZE; i++) {
            // Key already exists
            if (buffer[i].key == key) {
                std::cout << "Record with that key already exists.\n";
                fclose(fileRead);
                fclose(fileWrite);
                remove(TEMPDATAFILE);
                delete[] buffer;
                return;
            }
            // Found specific record
            else if (buffer[i].key > key) break;
        }
        i--;
        
        // Set up the pointer
        pointer = buffer[i].pointer;
        if (buffer[i].pointer == -1 || buffer[i].pointer > key) buffer[i].pointer = key;

        // Save page that was changed
        fwrite(buffer, RECORDSIZE, PAGESIZE, fileWrite);
        write++;
        // Rewrite rest of the primary area
        for (i = 0; i < page; i++) {
            fread(buffer, RECORDSIZE, PAGESIZE, fileRead);
            read++;
            fwrite(buffer, RECORDSIZE, PAGESIZE, fileWrite);
            write++;
        }
        // Rewrite overflow
        delete[] buffer;
        buffer = new Record[maxOverflow];
        fread(buffer, RECORDSIZE, maxOverflow, fileRead);
        read++;

        // Adding file to the end of overflow while changing pointers of all records
        for (j = 0; j < maxOverflow; j++) {
            // Adding to the free space in overflow
            if (buffer[j].key == -1) {
                buffer[j].key = key;
                buffer[j].voltage = voltage;
                buffer[j].amperage = amperage;
                // ----------------------------------------------------
                // TO-DO: Check if this assignment of pointer is alright
                // ----------------------------------------------------
                buffer[j].pointer = pointer;        
                sort(buffer, j);
                break;
            }
            // Found specific record
            else if (buffer[j].key == pointer) {
                // If key of found record is lower
                if (buffer[j].key < key) {
                    // If it's not pointing to specific element it is pointing to new record
                    if (buffer[j].pointer == -1) {
                        buffer[j].pointer = key;
                        pointer = -1;
                    }
                    // If it's pointing to bigger element we change the pointer
                    else if (buffer[j].pointer > key) {
                        pointer = buffer[j].pointer;
                        buffer[j].pointer = key;
                    }
                    // If it's pointing to lower element we change the pointer and look more
                    else pointer = buffer[j].pointer;
                }
                // If key of found record is bigger
                else pointer = buffer[j].key;
            }
        }

        recordsInOverflow++;
        fwrite(buffer, RECORDSIZE, maxOverflow, fileWrite);
        write++;
        delete[] buffer;
        fclose(fileRead);
        fclose(fileWrite);
        remove(DATAFILE);
        rename(TEMPDATAFILE, DATAFILE);
    }
};

// Sort buffer with length of sizeToSort
void sort(Record buffer[], int sizeToSort) {
    Record temp;
    bool change;

    for (int j = sizeToSort; j >= 0; j--) {
        change = 0;
        for (int i = 0; i < j; i++) {
            if (buffer[i].key > buffer[i + 1].key) {
                change = 1;
                temp = buffer[i];
                buffer[i] = buffer[i + 1];
                buffer[i + 1] = temp;
            }
        }
        if (!change) break;
    }
};

// We are looking for a number of page that has specific key
int searchIndex(int key, int& read) {
    Index* buffer = new Index[PAGESIZE]; // Buffer of index file
    FILE* file = fopen(INDEXFILE, "rb"); // Index file
    int i = maxPrimary / PAGESIZE, j, k, l, page = -1;

    // We are loading indexes by pages and looking for specific index
    while (i--) {
        l = fread(buffer, INDEXSIZE, PAGESIZE, file);
        read++;

        for (k = 0; k < l; k++) {
            if (buffer[k].key > key) {
                page = buffer[k].page - 1;
                break;
            }
            else page = -1;
        }
        if (page != -1) break;
    }

    fclose(file);
    // If primary area is empty
    if (page == -1) page = buffer[l-1].page;
    delete[] buffer;

    // We are returning number of page that has specific key
    return page;
};

// Creating Index based upon records in primary area
void createIndex(int& read, int& write) {
    int i = maxPrimary / PAGESIZE, j, k;        // Number of records
    Record* bufferOne = new Record[PAGESIZE];   // Buffer of data file
    Index* bufferTwo = new Index[PAGESIZE];     // Buffer of index file
    FILE* fileOne = fopen(DATAFILE, "rb");      // File with data
    FILE* fileTwo = fopen(INDEXFILE, "wb");     // Index file 

    // We are loading by pages and saving first key on every page to index
    for (j = 0, k = 0; j < i; j++, k++) {
        if (k == PAGESIZE) {
            fwrite(bufferTwo, INDEXSIZE, k, fileTwo);
            write++;
            k = 0;
        }
        fread(bufferOne, RECORDSIZE, PAGESIZE, fileOne);
        read++;
        bufferTwo[k].key = bufferOne[0].key;
        bufferTwo[k].page = j;
    }
    fwrite(bufferTwo, INDEXSIZE, k, fileTwo);
    write++;

    fclose(fileOne);
    fclose(fileTwo);
    delete[] bufferOne;
    delete[] bufferTwo;
};

// Show file of indexes
void showIndex() {
    int read = 0, k, l;
    Index* buffer = new Index[PAGESIZE];    // Buffer of index file
    FILE* file = fopen(INDEXFILE, "rb");    // Index file
    std::cout << "\nSHOW INDEX FILE\n";

    // ----------------------------------------------------
    // TO-DO: Maybe different order in this while
    // ----------------------------------------------------
    l = fread(buffer, INDEXSIZE, PAGESIZE, file);
    read++;
    while (l) {
        for (k = 0; k < l; k++)
            std::cout << buffer[k].key << " " << buffer[k].page << "\n";
        l = fread(buffer, INDEXSIZE, PAGESIZE, file);
        read++;
    }

    fclose(file);
    delete[] buffer;
    std::cout << "Reads = " << read << "\n";
};

// Show file
void showFile() {
    int read = 0, i = maxPrimary / PAGESIZE, l = 0, pointer;
    Record* primaryBuffer = new Record[PAGESIZE];       // Page buffer of primary area
    Record* overflowBuffer = new Record[maxOverflow];   // Page buffer of overflow area
    FILE* file = fopen(DATAFILE, "rb");                 // Data file

    // Load overflow to buffer
    fseek(file, maxPrimary * RECORDSIZE, SEEK_SET);
    fread(overflowBuffer, RECORDSIZE, maxOverflow, file);
    read++;
    
    fseek(file, 0, SEEK_SET);   // Go to the beggining of file
    std::cout << "\nSHOW FILE\n";

    for (int j = 0; j < i; j++) {
        // Load page from primary area
        fread(primaryBuffer, RECORDSIZE, PAGESIZE, file);
        read++;
        for (int k = 0; k < PAGESIZE; k++) {
            // Show record from primary area
            std::cout << primaryBuffer[k].key << " " << primaryBuffer[k].voltage << 
                " " << primaryBuffer[k].amperage << "\n";
            pointer = primaryBuffer[k].pointer;
            // If pointer is pointing to record in overflow 
            // we need to read records from overflow
            while (l < maxOverflow && overflowBuffer[l].key != -1) {
                if (overflowBuffer[l].key == pointer) {
                    std::cout << overflowBuffer[k].key << " " << overflowBuffer[k].voltage <<
                        " " << overflowBuffer[k].amperage << "\n";
                    pointer = overflowBuffer[k].pointer;
                }
                l++;
            }
        }
    }

    fclose(file);
    delete[] primaryBuffer;
    delete[] overflowBuffer;
    std::cout << "Read = " << read << "\n";
};
