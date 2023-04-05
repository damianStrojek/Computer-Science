// Damian Strojek s184407
// Database structures
// File records: voltage and amperage. Sorted by electric power
// a two-phase, three-strip sorting algorithm by natural joining was used
#include <iostream>
#include <time.h>
#include <fstream>
#include <string>
#include <climits>

#define BUFFER_SIZE 100

// Had to define it globally because I was not able
// to modify it in destructor
struct DiskInformation {
	unsigned long long int countRead = 0;
	unsigned long long int countWrite = 0;
	unsigned long long int countSplitting = 0;
	unsigned long long int countMerge = 0;
} diskInformation;

struct Record {
	double voltage;
	double amperage;
	double electricPower;

	Record() {};

	Record(double vol, double amp) {
		this->voltage = vol;
		this->amperage = amp;
		this->electricPower = vol * amp;
	};

	Record(double vol, double amp, double pow) {
		this->voltage = vol;
		this->amperage = amp;
		this->electricPower = pow;
	};
};

struct File {
	std::string fileName;
	int indexStop;			// Place in the file where we stopped reading

	File(std::string name, int index) {
		this->fileName = name;
		this->indexStop = index;
	};

	void clearFile() { remove(this->fileName.c_str()); };
};

// Class describing read buffer
class MainBuffer {
public:
	Record* buffer;		// Table of records
	int indexActual;	// Index of next record to read
	int bufferSize;		// Size of the buffer
	int counter;		// Index of last record in the buffer
	bool endOfFile;		// Whether EOL
	File* fileToRead;	// Information about the file to read

	MainBuffer(File* file) {
		this->bufferSize = BUFFER_SIZE;
		this->buffer = new Record[BUFFER_SIZE];
		this->indexActual = BUFFER_SIZE;
		this->endOfFile = false;
		this->counter = 0;
		this->fileToRead = file;
	};

	void clearBuffor() {
		/*
		for (int i = 0; i < this->bufferSize; i++) {
			this->buffer[i].amperage = -1;
			this->buffer[i].voltage = -1;
			this->buffer[i].electricPower = -1;
		}
		*/

		/*
		for(int i = 0; i < this->bufferSize; i++) { delete &(this->buffer[i]); }
		*/
	};

	// Download record from the buffer - only when copying files
	Record* readRecord() {
		// If buffer is empty - default settings
		if (this->indexActual == this->bufferSize) {
			// Increase number of readings and clear buffer
			diskInformation.countRead++;
			this->clearBuffor();

			// Temp variables
			double tempVol, tempAmp;
			Record* newRecord;

			std::ifstream input(this->fileToRead->fileName);
			if (!input.good()) {
				std::cout << "Cannot open the file: " <<
					this->fileToRead->fileName << "\n"; return NULL;
			}

			// Move to the place in the file that was read recently
			input.seekg(this->fileToRead->indexStop, std::ios::beg);
			this->counter = 0;

			while ((this->counter < this->bufferSize) && (!this->endOfFile)) {
				if (input >> tempVol >> tempAmp) {
					// If we are able to read data we need new object
					newRecord = new Record(tempVol, tempAmp);

					// Saving data to bufor and writing file to output
					this->buffer[this->counter] = *newRecord;
					std::cout << "U = " << tempVol << "V I = " << tempAmp << "A\n";
					this->counter++;
				}
				else this->endOfFile = true;
			}

			// Save the final index
			this->fileToRead->indexStop = input.tellg();
			input.close();
			this->indexActual = 0;
		}

		// If I've printed the entire buffer and we've reached the EOF, we're done
		if ((this->indexActual >= this->counter) && this->endOfFile) return NULL;
		this->indexActual++;
		return &(this->buffer[this->indexActual - 1]);
	};

	// Download record from the buffer - only when sorting (power is already calculated)
	Record* nextRecord() {
		if (this->indexActual == this->bufferSize) {
			diskInformation.countRead++;
			this->clearBuffor();

			double tempVol, tempAmp, tempPow;
			Record* newRecord;

			std::ifstream input(this->fileToRead->fileName);
			if (!input.good()) {
				std::cout << "Cannot open the file: " <<
					this->fileToRead->fileName << "\n"; return NULL;
			}

			input.seekg(this->fileToRead->indexStop, std::ios::beg);
			this->counter = 0;

			while ((this->counter < this->bufferSize) && (!this->endOfFile)) {
				if (input >> tempVol >> tempAmp >> tempPow) {
					newRecord = new Record(tempVol, tempAmp, tempPow);

					this->buffer[this->counter] = *newRecord;
					this->counter++;
				}
				else this->endOfFile = true;
			}

			this->fileToRead->indexStop = input.tellg();
			input.close();
			this->indexActual = 0;
		}

		if ((this->indexActual >= this->counter) && this->endOfFile) return NULL;
		this->indexActual++;
		return &(this->buffer[this->indexActual - 1]);
	};
};

// Class describing write buffer
class TapeBuffer {
public:
	Record* buffer;
	int indexActual;		// Index of the next element that is going to be written
	int bufferSize;
	bool display;			// whether to display file after successive runs
	File* fileToSave;		// object describing the save file

	TapeBuffer(File* file) {
		this->bufferSize = BUFFER_SIZE;
		this->buffer = new Record[BUFFER_SIZE];
		this->indexActual = 0;
		this->fileToSave = file;
		this->fileToSave->clearFile();
		this->display = false;
	};

	TapeBuffer(File* file, bool disp) {
		this->bufferSize = BUFFER_SIZE;
		this->buffer = new Record[BUFFER_SIZE];
		this->indexActual = 0;
		this->fileToSave = file;
		this->fileToSave->clearFile();
		this->display = disp;
	};

	~TapeBuffer() {
		diskInformation.countWrite++;
		std::ofstream output(this->fileToSave->fileName, std::ios::out | std::ios::app);

		for (int i = 0; i < this->indexActual; i++) {
			output << this->buffer[i].voltage << " " << this->buffer[i].amperage <<
				" " << this->buffer[i].electricPower << std::endl;
			if (this->display) std::cout << "U = " << this->buffer[i].voltage <<
				"V I = " << this->buffer[i].amperage << "A P = " << this->buffer[i].electricPower << "W\n";
		}

		output.close();
	};

	// Save record on the tape (in file) - while sorting
	bool saveRecord(Record* record) {
		if (this->indexActual == this->bufferSize) {
			diskInformation.countWrite++;
			std::ofstream output(this->fileToSave->fileName, std::ios::out | std::ios::app);

			if (this->display) std::cout << "\nFILE AFTER ANOTHER RUN:\n";

			for (this->indexActual = 0; this->indexActual < this->bufferSize; this->indexActual++) {
				output << this->buffer[this->indexActual].voltage << " " <<
					this->buffer[this->indexActual].amperage << " " <<
					this->buffer[this->indexActual].electricPower << std::endl;
				if (this->display) std::cout << "U = " << this->buffer[this->indexActual].voltage << "V I = " <<
					this->buffer[this->indexActual].amperage << "A P = " <<
					this->buffer[this->indexActual].electricPower << "W\n";
			}

			output.close();
			this->indexActual = 0;
		}

		if (record == NULL) return false;
		this->buffer[this->indexActual] = *record;
		this->indexActual++;
		return true;
	};

	// Save record on the tape (in file) - while rewriting sorted file
	bool saveValues(Record* record) {
		if (this->indexActual == this->bufferSize) {
			diskInformation.countWrite++;
			std::ofstream output(this->fileToSave->fileName, std::ios::out | std::ios::app);

			for (this->indexActual = 0; this->indexActual < this->bufferSize; this->indexActual++) {
				output << this->buffer[this->indexActual].voltage << " " <<
					this->buffer[this->indexActual].amperage << std::endl;
				std::cout << (this->indexActual + 1) << " U = " << this->buffer[this->indexActual].voltage <<
					"V I = " << this->buffer[this->indexActual].amperage << "A\n";
			}

			output.close();
			this->indexActual = 0;
		}

		if (record == NULL) return false;
		this->buffer[this->indexActual] = *record;
		this->indexActual++;
		return true;
	};

	// Save rest of the records from buffer to the tape
	void saveRestValues() {
		diskInformation.countWrite++;
		std::ofstream output(this->fileToSave->fileName, std::ios::out | std::ios::app);

		for (int i = 0; i < this->indexActual; i++) {
			output << this->buffer[i].voltage << " " <<
				this->buffer[i].amperage << std::endl;
			std::cout << i + 1 << ". U = " << this->buffer[i].voltage <<
				"V I = " << this->buffer[i].amperage << "A\n";
		}

		output.close();
		this->indexActual = 0;
	};
};

void generateRandomRecords(std::string filePath);
void readRecords(std::string filePath);
void readFilePath(std::string& filePath);
void menu(std::string filePath);
void rewriteFile(std::string sourceFile, std::string destinationFile);
void splitting();
bool merging(bool display);
void rewriteSorted();

int main() {
	std::string filePath = "input.csv";
	bool sorted = true, display;

	menu(filePath);
	rewriteFile(filePath, "file_c.csv");
	std::cout << "\n\tDo you want to see a file after each phase of sorting (0 - No, 1 - Yes)? ";
	std::cin >> display;

	// Next runs of sorting
	while (sorted) {
		splitting();
		sorted = merging(display);
	}
	rewriteSorted();

	std::cout << "\n\t\tSTATISTICS:\n\n\t\tPhases of splitting: " << diskInformation.countSplitting <<
		"\n\t\tPhases of merging: " << diskInformation.countMerge << "\n\t\tNumber of reads from the disk: " <<
		diskInformation.countRead << "\n\t\tNumber of writes to the disk: " << diskInformation.countWrite << "\n\n";

	return 0;
};

// When generating records |randomly|
void generateRandomRecords(std::string filePath) {
	double voltage, amperage, generate;
	int numberOfRecords;

	File* output = new File(filePath, 0);
	TapeBuffer* tape = new TapeBuffer(output);
	Record* record = NULL;

	srand(time(NULL));

	std::cout << "\n\tInsert number of records to generate: ";
	std::cin >> numberOfRecords;
	while (numberOfRecords--) {
		// Random integer part and fractional part for Voltage
		generate = rand() % 10000;
		voltage = rand() % 1000000 + generate / 10000;
		// Random integer part and fractional part for Amperage
		generate = rand() % 10000;
		amperage = rand() % 1000000 + generate / 10000;
		// Create object and save record
		record = new Record(voltage, amperage);
		tape->saveValues(record);
	}

	tape->saveRestValues();
	delete tape;
};

// When inputting records by hand
void readRecords(std::string filePath) {
	double voltage, amperage;
	int numberOfRecords, counter = 1;

	File* output = new File(filePath, 0);
	TapeBuffer* tape = new TapeBuffer(output);
	Record* record = NULL;

	std::cout << "\n\tInsert number of records you want to input: ";
	std::cin >> numberOfRecords;

	while (numberOfRecords--) {
		std::cout << "\n\t" << counter << "\n\tInsert voltage: ";
		std::cin >> voltage;
		std::cout << "\n\tInsert amperage: ";
		std::cin >> amperage;
		record = new Record(voltage, amperage);
		tape->saveValues(record);
		counter++;
	}

	tape->saveRestValues();
	delete tape;
};

// When loading records from file
void readFilePath(std::string& filePath) {
	std::string tempFilePath;
	std::cout << "\n\tInsert new filepath: ";
	// First we need to get rid of the newline char
	std::getline(std::cin, tempFilePath);
	std::getline(std::cin, tempFilePath);
};

// Main menu with the choice of generating random records, inputting them, or loading them from file
void menu(std::string filePath) {
	std::cout << "Damian Strojek s184407 2022\n\n\tChoose how to create a file with records:" <<
		"\n\t\t1.Random generator \n\t\t2.Insert all records\n\t\t3.Read records from file\n\n\tYour choice : ";

	int choice;
	std::cin >> choice;
	switch (choice) {
	case 1:
		generateRandomRecords(filePath);
		break;
	case 2:
		readRecords(filePath);
		break;
	case 3:
		readFilePath(filePath);
		break;
	}
};

// Rewriting the file - creating a backup copy
void rewriteFile(std::string sourceFile, std::string destinationFile) {
	File* source = new File(sourceFile, 0);
	File* destination = new File(destinationFile, 0);

	MainBuffer* original = new MainBuffer(source);
	TapeBuffer* copy = new TapeBuffer(destination);

	std::cout << "\nFILE BEFORE SORTING:\n";
	while (copy->saveRecord(original->readRecord()));

	delete original;
	delete copy;
};

// First phase of every run - splitting input tape (Main Buffer) to two help tapes (Tape Buffers)
void splitting() {
	diskInformation.countSplitting++;

	File* input = new File("file_c.csv", 0);
	File* outputA = new File("file_a.csv", 0);
	File* outputB = new File("file_b.csv", 0);

	MainBuffer* tapeInput = new MainBuffer(input);
	TapeBuffer* tapeOutputA = new TapeBuffer(outputA);
	TapeBuffer* tapeOutputB = new TapeBuffer(outputB);

	Record* record = NULL;
	double tempValue = 0;
	TapeBuffer* tapeTemp = tapeOutputA;

	while (true) {
		record = tapeInput->nextRecord();
		if (record == NULL) break;

		if (tempValue > record->electricPower) {
			if (tapeTemp == tapeOutputA) tapeTemp = tapeOutputB;
			else tapeTemp = tapeOutputA;
		}
		tapeTemp->saveRecord(record);
		tempValue = record->electricPower;
	}

	/*
	record = tapeInput->nextRecord();
	while (record != NULL) {
		if (tempValue > record->electricPower) {
			if (tapeTemp == tapeOutputA) tapeTemp = tapeOutputB;
			else tapeTemp = tapeOutputA;
		}

		tapeTemp->saveRecord(record);
		tempValue = record->electricPower;
		record = tapeInput->nextRecord();
	}
	*/

	delete tapeInput;
	delete tapeOutputA;
	delete tapeOutputB;
};

// Second phase of every run - merging
bool merging(bool display) {
	diskInformation.countMerge++;

	File* inputA = new File("file_a.csv", 0);
	File* inputB = new File("file_b.csv", 0);
	File* output = new File("file_c.csv", 0);

	MainBuffer* tapeInputA = new MainBuffer(inputA);
	MainBuffer* tapeInputB = new MainBuffer(inputB);
	TapeBuffer* tapeOutput = new TapeBuffer(output, display);

	double tempValueA = 0, tempValueB = 0;
	Record* recordA = tapeInputA->nextRecord(),
		* recordB = tapeInputB->nextRecord();
	// If second tape is clean - records are already sorted and on the first tape
	if (recordB == NULL) return false;

	// This while is better than while(true), I have no idea how to rewrite it differently
	while (diskInformation.countWrite < ULLONG_MAX) {
		// 1. If both tapes are not clean
		if ((recordA != NULL) && (recordB != NULL)) {
			// 1.1. We check if we have reached the end of the series in tape A
			if (recordA->electricPower < tempValueA) {
				// If yes then we are writting the rest of the tape B
				while ((recordB != NULL) && (recordB->electricPower > tempValueB)) {
					tapeOutput->saveRecord(recordB);
					tempValueB = recordB->electricPower;
					recordB = tapeInputB->nextRecord();
				}
				tempValueA = 0;
				tempValueB = 0;
			}
			// 1.2. We check if we have reached the end of the series in tape B
			else if (recordB->electricPower < tempValueB) {
				// If yes then we are writting the rest of the tape A
				while ((recordA != NULL) && (recordA->electricPower > tempValueA)) {
					tapeOutput->saveRecord(recordA);
					tempValueA = recordA->electricPower;
					recordA = tapeInputA->nextRecord();
				}
				tempValueA = 0;
				tempValueB = 0;
			}
			// 1.3. If both series didn't end we check which one is smaller
			else {
				if (recordA->electricPower < recordB->electricPower) {
					tapeOutput->saveRecord(recordA);
					tempValueA = recordA->electricPower;
					recordA = tapeInputA->nextRecord();
				}
				else {
					tapeOutput->saveRecord(recordB);
					tempValueB = recordB->electricPower;
					recordB = tapeInputB->nextRecord();
				}
			}
		}
		// 2. If records from tape A are already written to the output
		else if (recordA == NULL) {
			while (recordB != NULL) {
				tapeOutput->saveRecord(recordB);
				recordB = tapeInputB->nextRecord();
			}
			break;
		}
		// 3. If records from tape B are already written to the output
		else if (recordB == NULL) {
			while (recordA != NULL) {
				tapeOutput->saveRecord(recordA);
				recordA = tapeInputA->nextRecord();
			}
			break;
		}
	}

	delete tapeInputA;
	delete tapeInputB;
	delete tapeOutput;

	return true;
};

// Rewriting sorted tape to the output file
void rewriteSorted() {
	File* input = new File("file_a.csv", 0);
	File* output = new File("sorted.csv", 0);

	MainBuffer* original = new MainBuffer(input);
	TapeBuffer* copy = new TapeBuffer(output);

	// Rewrite the file and write it to the output
	std::cout << "\nFILE AFTER SORTING:\n";
	while (copy->saveValues(original->nextRecord()));

	// Rewrite the rest of the records from buffer to the tape
	copy->saveRestValues();

	// Delete objects and additional Tapes
	delete original;
	delete copy;
	remove("file_a.csv");
	remove("file_b.csv");
};
