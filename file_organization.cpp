// Damian Strojek s184407
// Database structures
// File records: voltage and amperage
// index-sequential file organization
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>

#define DATAFILE "data_file"
#define TEMPDATAFILE "data_temp_file"
#define INDEXFILE "index_file"
#define PAGESIZE 10
#define ALFA 0.5
#define RECORDSIZE sizeof(Record)
#define INDEXSIZE sizeof(Index)

// One structure is better than global variables
struct GlobalInformation {
    int recordsInPrimary = 0;           // Number of records in primary area
    int recordsInOverflow = 0;          // Number of records in overflow area
    int maxPrimary = 10;                // Maximum number of records in primary area
    int maxOverflow = maxPrimary / 5;   // Maximum number of records in overflow area
} globalInf;

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
void readRecord(int key);
bool deleteRecord(int key, int& read, int& write);
void update(int key, double voltage, double amperage);
void clearBuffer(Record* buffer, int counter);

int main() {
    // We need to initialize with start values
    initialize();

    bool escape = false;
    int choice, key, read, write;
    double voltage, amperage;
    std::string command;

    std::cout << "\n\tDAMIAN STROJEK S184407\n\tINDEX-SEQUENTIAL FILE ORGANISATION\n";
    std::cout << "\n\tChoose data source:\n\t\t1 - Keyboard\n\t\t2 - File\n\n\tChoice: ";
    std::cin >> choice;

    if (choice == 1) {
        while (!escape) {
            read = write = 0;
            std::cout << "\n\tWhat do you want to do?\n\t'add vol amp' - Adding new record" <<
                "\n\t'upd key vol amp' - Update existing record\n\t'read key' - Read existing" <<
                " record\n\t'del key' - Delete existing record\n\t'reorg' - Reorganise file" <<
                "\n\t'show file' - Show data file in the order of keys\n\t'show index' - Show" <<
                " index file\n\t'quit' - End the program\n\n\tYour command: ";
            std::cin >> command;
            // Add new record
            if (command == "add") {
                std::cin >> voltage >> amperage;
                addNewRecord(voltage, amperage, read, write);
                std::cout << "\n\n\tADDING NEW RECORD\n\n\tReads = " << read << " Writes = "
                    << write << "\n";
            }
            // Update existing record with key 'key'
            else if (command == "upd") {
                std::cin >> key >> voltage >> amperage;
                update(key, voltage, amperage);
            }
            // Read existing record with key 'key'
            else if (command == "read") {
                std::cin >> key;
                readRecord(key);
            }
            // Delete existing record with key 'key'
            else if (command == "del") {
                std::cin >> key;
                if (!deleteRecord(key, read, write))
                    std::cout << "\n\tThere is no record with inputted key.\n";
                std::cout << "\n\n\tDELETING EXISTING RECORD\n\n\tReads = " << read
                    << " Writes = " << write << "\n";
            }
            else if (command == "reorg") {
                reorganize(read, write);
                std::cout << "\n\n\tREORGANIZATION\n\n\tReads = " << read
                    << " Writes = " << write << "\n";
            }
            else if (command == "show") {
                std::cin >> command;
                if (command == "file") showFile();
                else showIndex();
            }
            else if (command == "quit") {
                escape = true;
            }
            else {
                std::cout << "\n\tChoose one of the available options.\n";
            }
        }
    }
    else if (choice == 2) {
        std::cout << "\n\n\tFile you want to open: ";
        std::cin >> command;
        std::ifstream inputFile;
        inputFile.open(command);
        if (!inputFile.is_open()) {
            std::cout << "\n\t!!! Couldn't open file " << command << " !!!.\n";
            return 0;
        }

        while (!escape) {
            read = write = 0;
            inputFile >> command;
            // Add new record
            if (command == "add") {
                inputFile >> voltage >> amperage;
                addNewRecord(voltage, amperage, read, write);
                std::cout << "\n\n\tADDING NEW RECORD\n\n\tReads = " << read << " Writes = "
                    << write << "\n";
            }
            // Update existing record with key 'key'
            else if (command == "upd") {
                inputFile >> key >> voltage >> amperage;
                update(key, voltage, amperage);
            }
            // Read existing record with key 'key'
            else if (command == "read") {
                inputFile >> key;
                readRecord(key);
            }
            // Delete existing record with key 'key'
            else if (command == "del") {
                inputFile >> key;
                if (!deleteRecord(key, read, write))
                    std::cout << "\n\tThere is no record with inputted key.\n";
                std::cout << "\n\n\tDELETING EXISTING RECORD\n\n\tReads = " << read
                    << " Writes = " << write << "\n";
            }
            else if (command == "reorg") {
                reorganize(read, write);
                std::cout << "\n\n\tREORGANIZATION\n\n\tReads = " << read
                    << " Writes = " << write << "\n";
            }
            else if (command == "show") {
                inputFile >> command;
                if (command == "file") showFile();
                else showIndex();
            }
            else if (command == "quit") {
                escape = true;
            }
            else {
                std::cout << "\n\tThere is a typo in inputted file.\n";
            }
        }
        inputFile.close();
    }
    else {
        std::cout << "You need to choose either 1 or 2.\n";
    }
    system("pause");
    return 0;
};

// Initialize start values
void initialize() {
    Record* buffer = new Record[PAGESIZE];
    FILE* file = fopen(DATAFILE, "wb");
    int read = 0, write = 0;

    // Creating datafile and writting empty records to it
    int i = (int)ceil(((double)(globalInf.maxPrimary + globalInf.maxOverflow)) / (double)PAGESIZE);
    while (i--) {
        fwrite(buffer, RECORDSIZE, PAGESIZE, file);
        write++;
    }

    delete[] buffer;
    fclose(file);
    createIndex(read, write);

    std::cout << "\n\tSetting up initial values:\n\tReads = " <<
        read << ". Writes = " << write << "\n";

    // Changing first record so it has the lowest possible key
    addNewRecord(1.0, 1.0, read, write);
};

// Adding new record
void addNewRecord(double voltage, double amperage, int& read, int& write) {
    // If overflow is full we need to reorganize
    if (globalInf.recordsInOverflow == globalInf.maxOverflow) reorganize(read, write);

    int key = (int)(voltage * amperage);
    if (key < 0) {
        std::cout << "\n\tYou can't input a record with key less than 1.\n";
        return;
    }
    else if (key == 0) {
        std::cout << "\n\tRecord with inputted key doesn't exist.\n";
        return;
    }

    int pointer, i = 0, j, page;
    double changeIndex = 0;

    Record* buffer = new Record[PAGESIZE];          // Buffer for data file
    FILE* fileRead = fopen(DATAFILE, "rb");         // Data file
    FILE* fileWrite = fopen(TEMPDATAFILE, "wb");    // Temporary file
    // CZEMU ZWRACA NIE WIADOMO CO
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
                globalInf.recordsInPrimary++;

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
                i = (int)ceil((double)globalInf.maxOverflow / (double)PAGESIZE);
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
        buffer = new Record[globalInf.maxOverflow];
        fread(buffer, RECORDSIZE, globalInf.maxOverflow, fileRead);
        read++;

        // Adding file to the end of overflow while changing pointers of all records
        for (j = 0; j < globalInf.maxOverflow; j++) {
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

        globalInf.recordsInOverflow++;
        fwrite(buffer, RECORDSIZE, globalInf.maxOverflow, fileWrite);
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
    int i = globalInf.maxPrimary / PAGESIZE, k, l, page = -1;

    // We are loading indexes by pages and looking for specific index
    l = fread(buffer, INDEXSIZE, PAGESIZE, file);
    while (i--) {
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

    // If primary area is empty
    if (page == -1 && l > 0) {
        page = buffer[l - 1].page;
    }

    fclose(file);
    delete[] buffer;

    // We are returning number of page that has/should have specific key
    return page;
};

// Creating Index based upon records in primary area
void createIndex(int& read, int& write) {
    int i = globalInf.maxPrimary / PAGESIZE, j, k;      // Number of records
    Record* bufferOne = new Record[PAGESIZE];           // Buffer of data file
    Index* bufferTwo = new Index[PAGESIZE];             // Buffer of index file
    FILE* fileOne = fopen(DATAFILE, "rb");              // File with data
    FILE* fileTwo = fopen(INDEXFILE, "wb");             // Index file 

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
    std::cout << "\n\tSHOW INDEX FILE\n\n";

    l = fread(buffer, INDEXSIZE, PAGESIZE, file);
    read++;
    while (l) {
        for (k = 0; k < l; k++)
            std::cout << "\t\t" << buffer[k].key << " " << buffer[k].page << "\n";
        l = fread(buffer, INDEXSIZE, PAGESIZE, file);
        read++;
    }

    fclose(file);
    delete[] buffer;
    std::cout << "\n\tReads = " << read << "\n";
};

// Show file
void showFile() {
    int read = 0, i = globalInf.maxPrimary / PAGESIZE, pointer, l = 0;
    Record* primaryBuffer = new Record[PAGESIZE];                // Page buffer of primary area
    Record* overflowBuffer = new Record[globalInf.maxOverflow];  // Page buffer of overflow area
    FILE* file = fopen(DATAFILE, "rb");                          // Data file

    // Load overflow to buffer
    fseek(file, globalInf.maxPrimary * RECORDSIZE, SEEK_SET);
    fread(overflowBuffer, RECORDSIZE, globalInf.maxOverflow, file);
    read++;

    fseek(file, 0, SEEK_SET);   // Go to the beggining of file
    std::cout << "\n\tSHOW FILE\n\n";

    for (int j = 0; j < i; j++) {
        std::cout << "\n\tPAGE " << j + 1 << "\n";
        // Load page from primary area
        fread(primaryBuffer, RECORDSIZE, PAGESIZE, file);
        read++;
        for (int k = 0; k < PAGESIZE; k++) {
            // Show record from primary area
            std::cout << "\t\t" << primaryBuffer[k].key << " " << primaryBuffer[k].voltage <<
                " " << primaryBuffer[k].amperage;
            pointer = primaryBuffer[k].pointer;
            std::cout << " P: " << primaryBuffer[k].pointer << "\n";
        }
    }

    std::cout << "\n\tOVERFLOW\n\n";

    while (l < globalInf.maxOverflow) {
        std::cout << "\t\t" << overflowBuffer[l].key << " " << overflowBuffer[l].voltage <<
            " " << overflowBuffer[l].amperage;
        pointer = overflowBuffer[l].pointer;
        std::cout << " P: " << overflowBuffer[l].pointer << "\n";
        l++;
    }

    fclose(file);
    delete[] primaryBuffer;
    delete[] overflowBuffer;
    std::cout << "\n\tRead = " << read << "\n";
};

// Reorganize both areas -> primary = primary + overflow, overflow = 0
void reorganize(int& read, int& write) {
    int counter = 0, i = globalInf.maxPrimary / PAGESIZE, l = 0, pointer, pages = 0;
    Record* primaryBuffer = new Record[PAGESIZE];               // Buffer of pages from primary area
    Record* overflowBuffer = new Record[globalInf.maxOverflow]; // Buffer of pages from overflow area
    Record* saveBuffer = new Record[PAGESIZE];                  // Buffer to save to
    FILE* fileRead = fopen(DATAFILE, "rb");                     // Data file to read from
    FILE* fileWrite = fopen(TEMPDATAFILE, "wb");                // Save file to save to - will need renaming

    // Loading up overflow
    fseek(fileRead, globalInf.maxPrimary * RECORDSIZE, SEEK_SET);
    fread(overflowBuffer, RECORDSIZE, globalInf.maxOverflow, fileRead);
    read++;
    fseek(fileRead, 0, SEEK_SET);

    for (int j = 0; j < i; j++) {
        // Load up page from primary area
        fread(primaryBuffer, RECORDSIZE, PAGESIZE, fileRead);
        read++;
        for (int k = 0; k < PAGESIZE; k++) {
            // If the save buffer is "alpha filled", we write it to the file
            if (counter == PAGESIZE * ALFA) {
                std::cout << "\nALPHA FILLED TEST\n";
                for (int m = 0; m < counter; m++) saveBuffer[m].pointer = -1;
                fwrite(saveBuffer, RECORDSIZE, PAGESIZE, fileWrite);
                write++;

                // Clear the save Buffer so there is no double values
                clearBuffer(saveBuffer, counter);

                counter = 0;
                pages++;
            }
            saveBuffer[counter] = primaryBuffer[k];
            counter++;
            pointer = primaryBuffer[k].pointer;

            // If record from primary area is pointing on the page from overflow
            while (pointer != -1 && l < globalInf.maxOverflow) {
                // If the save buffer is "alpha filled", we write it to the file
                if (counter == PAGESIZE * ALFA) {
                    for (int m = 0; m < counter; m++) saveBuffer[m].pointer = -1;
                    fwrite(saveBuffer, RECORDSIZE, PAGESIZE, fileWrite);
                    write++;

                    // Clear the save Buffer so there is no double values
                    clearBuffer(saveBuffer, counter);

                    counter = 0;
                    pages++;
                }
                // Saving record from overflow to save buffer
                saveBuffer[counter] = overflowBuffer[l];
                pointer = overflowBuffer[l].pointer;
                counter++;
                l++;
            }
        }
    }

    // If there is something in saveBuffer it means we have to write it to the end
    if (counter > 0) {
        for (int m = 0; m < counter; m++) saveBuffer[m].pointer = -1;
        fwrite(saveBuffer, RECORDSIZE, PAGESIZE, fileWrite);
        write++;
        pages++;
    }

    // OVERFLOW TRZEBA SIE POZBYC JAK JEST JUZ WYCZYSZCZONE WSZYSTKO 

    delete[] primaryBuffer;
    delete[] overflowBuffer;
    delete[] saveBuffer;
    fclose(fileRead);
    fclose(fileWrite);

    remove(DATAFILE);
    rename(TEMPDATAFILE, DATAFILE);
    // Changing number of records in each area
    globalInf.maxPrimary = pages * PAGESIZE;
    globalInf.maxOverflow = globalInf.maxPrimary / 2;
    globalInf.recordsInPrimary += globalInf.recordsInOverflow;
    globalInf.recordsInOverflow = 0;
    // Creating new index on the basis of changed values
    createIndex(read, write);
};

// Clear the buffer so there is no double values saved to output
void clearBuffer(Record *buffer, int counter) {
    for (int i = 0; i < counter; i++) {
        buffer[i].amperage = 0;
        buffer[i].voltage = 0;
        buffer[i].key = -1;
        buffer[i].pointer = -1;
    }
};

// Read record with specified key
void readRecord(int key) {
    std::cout << "\nREADING RECORD\n";
    int read = 0, i, pointer;
    int page = searchIndex(key, read);          // Looking for specified page
    Record* buffer = new Record[PAGESIZE];      // Buffer for data
    FILE* file = fopen(DATAFILE, "rb");         // File to read from
    fseek(file, page * RECORDSIZE, SEEK_SET);   // Setting up cursor in file
    fread(buffer, RECORDSIZE, PAGESIZE, file);  // Loading up specified page
    read++;

    // Looking through loaded page
    for (i = 0; i < PAGESIZE; i++) {
        // If key is lower we change pointer and look more
        if (buffer[i].key < key)
            pointer = buffer[i].pointer;
        else
            break;
    }

    if (buffer[i].key == key)
        std::cout << buffer[i].key << " " << buffer[i].voltage << " " << buffer[i].amperage << "\n";
    else if (pointer == -1)
        std::cout << "There is no record with specified key.\n";
    // Looking for that key in overflow area
    else {
        // Jumping through primary area to overflow area
        fseek(file, globalInf.maxPrimary * RECORDSIZE, SEEK_SET);
        // Loading up pages from overflow area
        while (fread(buffer, RECORDSIZE, PAGESIZE, file) > 0) {
            read++;
            // Looking through loaded page
            for (i = 0; i < PAGESIZE; i++) {
                if (buffer[i].key < key)
                    pointer = buffer[i].pointer;
                else
                    break;
            }
        }

        if (buffer[i].key == key)
            std::cout << buffer[i].key << " " << buffer[i].voltage << " " << buffer[i].amperage << "\n";
        else
            std::cout << "There is no record with specified key.\n";
    }

    fclose(file);
    delete[] buffer;
    std::cout << "Read = " << read << "\n";
};

// Delete record with specific key
bool deleteRecord(int key, int& read, int& write) {
    if (key == 0) {
        std::cout << "You can't delete record with key 0.\n";
        return false;
    }

    int pointer, page = searchIndex(key, read);
    bool deleted = false;

    Record* primaryBuffer = new Record[PAGESIZE];   // Buffer of pages from primary area 
    Record* overflowBuffer = new Record[PAGESIZE];  // Buffer of pages from overflow area
    FILE* fileRead = fopen(DATAFILE, "rb");         // Data file to read from
    FILE* fileWrite = fopen(TEMPDATAFILE, "wb");    // Data file to write to

    // Loading up overflow
    fseek(fileRead, globalInf.maxPrimary * RECORDSIZE, SEEK_SET);
    fread(overflowBuffer, RECORDSIZE, globalInf.maxOverflow, fileRead);
    read++;
    fseek(fileRead, 0, SEEK_SET);

    // Writing all pages from primary area to temporary file (to specific page)
    for (int i = 0; i < page; i++) {
        fread(primaryBuffer, RECORDSIZE, PAGESIZE, fileRead);
        read++;
        fwrite(primaryBuffer, RECORDSIZE, PAGESIZE, fileWrite);
        write++;
    }

    // Loading page on which specific record is located
    fread(primaryBuffer, RECORDSIZE, PAGESIZE, fileRead);
    read++;
    pointer = -1;

    // Looking for this record on page
    for (int i = 0; i < PAGESIZE; i++) {
        // If we found this key
        if (primaryBuffer[i].key == key) {
            // If record is not pointing to the next one
            if (primaryBuffer[i].pointer == -1) {
                // Moving this record one up
                for (; i < PAGESIZE - 1; i++) primaryBuffer[i] = primaryBuffer[i + 1];
                // Changing values of the last record on the page - it's free now
                primaryBuffer[i].key = primaryBuffer[i].pointer = -1;
                primaryBuffer[i].voltage = primaryBuffer[i].amperage = 0;
                deleted = true;
                globalInf.recordsInPrimary--;
            }
            // If record from primary is pointing to record in overflow
            else {
                for (int j = 0; j < globalInf.maxOverflow; j++) {
                    // If we found the record
                    if (overflowBuffer[j].key == pointer) {
                        // Moving record from overflow to primary
                        primaryBuffer[i] = overflowBuffer[j];
                        for (i++; i < PAGESIZE; i++) if (primaryBuffer[i].key == -1) break;
                        // Sorting primary page
                        sort(primaryBuffer, i);
                        // Rewriting records from overflow by one up
                        for (; j < globalInf.maxOverflow - 1; j++) overflowBuffer[j] = overflowBuffer[j + 1];
                        // Last record from overflow is now free
                        overflowBuffer[j].key = overflowBuffer[j].pointer = -1;
                        overflowBuffer[j].voltage = overflowBuffer[j].amperage = 0;
                        deleted = true;
                        globalInf.recordsInOverflow--;
                        break;
                    }
                }
            }
            break;
        }
        // If we are looking for record that is not in primary area
        else if (primaryBuffer[i].key > key) {
            if (pointer != -1) {
                // Looking for this record in overflow area
                int j = 0;
                while (j < globalInf.maxOverflow && pointer != -1) {
                    if (overflowBuffer[j].key == key) {
                        primaryBuffer[i - 1].pointer = overflowBuffer[j].pointer;
                        for (; j < globalInf.maxOverflow - 1; j++) overflowBuffer[j] = overflowBuffer[j + 1];
                        // Last record from overflow is now free
                        overflowBuffer[j].key = overflowBuffer[j].pointer = -1;
                        overflowBuffer[j].voltage = overflowBuffer[j].amperage = 0;
                        deleted = true;
                        globalInf.recordsInOverflow--;
                    }
                    else pointer = overflowBuffer[j].pointer;
                    j++;
                }
            }
        }
        pointer = overflowBuffer[i].pointer;
    }

    // Saving changed up page
    fwrite(primaryBuffer, RECORDSIZE, PAGESIZE, fileWrite);
    write++;

    // Rewriting rest of the primary area
    for (int i = page + 1; i < globalInf.maxPrimary / PAGESIZE; i++) {
        fread(primaryBuffer, RECORDSIZE, PAGESIZE, fileRead);
        read++;
        fwrite(primaryBuffer, RECORDSIZE, PAGESIZE, fileWrite);
        write++;
    }
    globalInf.recordsInPrimary--;

    // Rewriting the overflow
    fwrite(overflowBuffer, RECORDSIZE, globalInf.maxOverflow, fileWrite);
    write++;

    delete[] primaryBuffer;
    delete[] overflowBuffer;
    fclose(fileRead);
    fclose(fileWrite);
    remove(DATAFILE);
    rename(TEMPDATAFILE, DATAFILE);

    return deleted;
};

// Update the specific key with new voltage and amperage
void update(int key, double voltage, double amperage) {
    std::cout << "\nUPDATE\n";
    int read = 0, write = 0;
    // Delete this record and then add new record
    if (deleteRecord(key, read, write))
        addNewRecord(voltage, amperage, read, write);
    else
        std::cout << "There is no record with specified key.\n";
    std::cout << "Read = " << read << " Write = " << write << "\n";
};
