// Damian Strojek GUT IT
// 2021-04-28
// SOLVED WITHOUT CONCIDERING MULTIPLE OWNERS WITH THE SAME HASH CODE
#include <iostream>
#include <string.h>

// ONE RECIPIENT CONSISTS OF HIS NAME AND VALUE THAT HE RECEIVED FROM OWNER OF ACCOUNT
struct recipient {
    std::string nameOfRecipient = "";
    int valueReceived = 0;
};

// MY OWN VECTOR FOR RECIPIENTS OF ACCOUNT
class Vector {
    private:
        recipient *array;
        int capacity;
        int length;
    public:
        Vector(int = 10);                           // BASIC LENGTH IS 10 BUT IT CAN DOUBLE ITSELF WHEN NEEDED
        void push_back(recipient newRecipient);     // ADDING NEW RECIPIENTS FOR THIS PARTICULAR OWNER OF ACCOUNT
        int size() const;
        recipient get(int index);
        void changeValue(int index, int value);     // CHANGING VALUE OF ONE OF RECIPIENTS
        ~Vector();
};

Vector::Vector(int n) : capacity(n), array(new recipient[n]), length(0){
}

void Vector::push_back(recipient newRecipient){
    // IF THERE IS NO SPACE LEFT YOU DOUBLE CAPACITY
    if(length == capacity){
        recipient *old = array;
        array = new recipient[capacity *= 2];
        std::copy(old, old+length, array);
        delete[] old;
    }
    array[length] = newRecipient;
    length++;
}

int Vector::size() const{
    return length;
}

recipient Vector::get(int index){
    return array[index];
}

void Vector::changeValue(int index, int value){
    array[index].valueReceived += value;
}

Vector::~Vector(){
    delete[] array;
}

//  ONE ACCOUNT CONSISTS OF OWNER OF THIS ACCOUNT AND RECIPIENTS OF HIS TRANSACTIONS
struct account {
    std::string ownerOfAccount = "";
    Vector recipients;
};

class HashTable {
    private:
        // LENGTH OF TABLE OF ACCOUNTS
        int lengthOfTable;
        // TABLE OF ACCOUNTS
        account *accounts;
    public:
        HashTable();
        int hashFunction(std::string nameOfOwner);                                      // SUMING ASCII CHARACTERS AND MODULO THE SUM BY MAX AMOUNT OF ACCOUNTS
        void pays(std::string nameOfOwner, std::string nameOfRecipient, int value);     // VOID TO PAY THE RECIPIENT
        int checkBalance(std::string nameOfOwner, std::string nameOfRecipient);         // VOID TO CHECK BALANCE OF ONE PARTICULAR RECIPIENT
        void debug();                                                                   // REALLY NICE DEBUGGING FUNCTION, GO TRY IT OUT
        ~HashTable();
};

HashTable::HashTable(){
    lengthOfTable = 23;                     // ONE OF FIRST PRIME NUMBERS
    accounts = new account[lengthOfTable];  // ARRAY OF ACCOUNTS
}

// SUMING ASCII CHARACTERS AND MODULO THE SUM BY MAX AMOUNT OF ACCOUNTS
int HashTable::hashFunction(std::string nameOfOwner){
    int sumOfASCII = 0;
    for(int i = 0; i < nameOfOwner.length(); i++){
        sumOfASCII += nameOfOwner[i];
    }
    return (sumOfASCII % lengthOfTable);
}

void HashTable::pays(std::string nameOfOwner, std::string nameOfRecipient, int value){
    // CALCULATING HASH CODE OF OUR OWNER 
    int index = hashFunction(nameOfOwner);
    // IF THERE IS NO OWNER OF THIS ACCOUNT YOU CHANGE IT
    // I DONT EXPECT OWNERS WITH THE SAME HASH CODES, ITS EASY TO FIX
    if(accounts[index].ownerOfAccount == ""){
        accounts[index].ownerOfAccount = nameOfOwner;
    }
    // I CHECK EVERY RECIPIENT OF THIS OWNER FOR THE SAME RECIPIENT THAT JUST GAVE HIM MONEY
    for(int i = 0; i < accounts[index].recipients.size(); i++){
        if(!(accounts[index].recipients.get(i).nameOfRecipient.compare(nameOfRecipient))){
            accounts[index].recipients.changeValue(i, value);
            return;
        }
    }
    // IF THERE WAS NO SUCH A RECEIVER BEFORE WE ADD HIM TO THE END OF RECIEPINTS LIST
    recipient newRecipier{nameOfRecipient, value};
    accounts[index].recipients.push_back(newRecipier);
}

int HashTable::checkBalance(std::string nameOfOwner, std::string nameOfRecipient){
    int index = hashFunction(nameOfOwner);
    for(int i = 0; i < accounts[index].recipients.size(); i++){
        // CHECKING IF THERE IS RECIPIENT THAT WAS PAID FROM THIS ACCOUNT
        if(!(accounts[index].recipients.get(i).nameOfRecipient.compare(nameOfRecipient))){
            return accounts[index].recipients.get(i).valueReceived;
        }
    }
    // IF NOT RETURN 0 DOLLARS
    return 0;
}

void HashTable::debug(){
    system("cls");
    for(int i = 0; i < lengthOfTable; i++){
        std::cout << i << " numer konta : ";
        for(int j = 0; j < accounts[i].recipients.size(); j++){
            std::cout << " +" << accounts[i].recipients.get(j).valueReceived << " from " << accounts[i].recipients.get(j).nameOfRecipient << ",";
        }
        std::cout << "\n";
    }
}

HashTable::~HashTable(){
    delete[] accounts;
}

int main(){
    HashTable myHashTable;
    std::string nameOfOwner, nameOfRecipient;
    int value;

    while(std::cin >> nameOfOwner){
        if(!nameOfOwner.compare("+")){
            std::cin >> nameOfOwner >> nameOfRecipient;
            std::cin >> value;
            myHashTable.pays(nameOfOwner, nameOfRecipient, value);
        }
        else if(!nameOfOwner.compare("?")){
            std::cin >> nameOfOwner >> nameOfRecipient;
            printf("%d\n",myHashTable.checkBalance(nameOfOwner, nameOfRecipient));
        }
        // JUST FOR FUN, YOU CAN TRY AFTER ADDING SOME ACCOUNTS
        else if(!nameOfOwner.compare("debug")){
            myHashTable.debug();
        }
        else{
            printf("BAD COMMAND\n");
        }
    }
    return 0;
}