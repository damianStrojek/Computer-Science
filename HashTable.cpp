// AISD PG WETI LABS NR 5
// Damian Strojek s184407 2021 IT/CS
// @ Copyright 2021, Damian Strojek, All rights reserved.

// THE IDEA IS THAT THERE IS 23 MAIN ACCOUNTS AND EVERY MAIN ACCOUNTS CONSISTS OF 10 SUB-ACCOUNTS
// EVERY SUB-ACCOUNT CONSISTS OF INT_MAXSIZE RECIPIENTS AND OWNER OF SUB-ACCOUNT
// SO THERE CAN BE MAXIMUM 10 OWNERS OF ACCOUNT WITH THE SAME HASH CODE
#include <iostream>
#include <string>
#include <string.h>

// ONE RECIPIENT CONSISTS OF HIS NAME AND VALUE THAT HE RECEIVED FROM OWNER OF ACCOUNT
struct recipient {
    std::string nameOfRecipient = "";
    int valueReceived = 0;
};

struct node {
    recipient *array;
    int length = 0;
    node *next = NULL;
};

class List {
    private:
        int length;
        node *head, *tail;
    public:
        List();
        void add(recipient newRecipient);
        int getLength() const;
        node *getTail() const;
        node *getHead() const;
    friend class HashTable;
};

List::List(){
    length = 0;
    tail = NULL;
};

void List::add(recipient newRecipient){
    node *newNode = new node;
    newNode->array = new recipient[1000];

    if(length == 0){
        head = newNode;
        tail = newNode;
        length = 1;
    }
    else{
        tail->next = newNode;
        tail = newNode;
        length++;
    }
};

int List::getLength() const{
    return length;
};

node *List::getTail() const{
    return tail;
};

node *List::getHead() const{
    return head;
}
//  ONE ACCOUNT CONSISTS OF OWNER OF THIS ACCOUNT AND RECIPIENTS OF HIS TRANSACTIONS
struct account {
    std::string ownerOfAccount = "";
    List recipients;
};

// ONE ACCOUNTS CONSISTS OF 10 SMALLER ACCOUNTS AND COUNTER OF HOW MANY ACCOUNTS THERE ALREADY IS
struct accounts {
    account *tableOf10Accounts = new account[10];
    int counterOfAccounts = 0;
};

class HashTable {
    private:
        // LENGTH OF TABLE OF ACCOUNTS
        int lengthOfTable;
        // TABLE OF 23 ACCOUNTS AND EVERY ACCOUNT CAN HAVE UP TO 10 OWNERS WITH THE SAME HASH CODE
        accounts *tableOf23Accounts;
    public:
        HashTable();
        int hashFunction(std::string nameOfOwner) const;                                      // SUMING ASCII CHARACTERS AND MODULO THE SUM BY MAX AMOUNT OF ACCOUNTS
        void pays(std::string nameOfOwner, std::string nameOfRecipient, int value);     // VOID TO PAY THE RECIPIENT
        int checkBalance(std::string nameOfOwner, std::string nameOfRecipient);         // VOID TO CHECK BALANCE OF ONE PARTICULAR RECIPIENT
        void debug();                                                                   // REALLY NICE DEBUGGING FUNCTION, GO TRY IT OUT
        ~HashTable();
};

HashTable::HashTable(){
    lengthOfTable = 23;                     // ONE OF FIRST PRIME NUMBERS
    tableOf23Accounts = new accounts[lengthOfTable];
}

// SUMING ASCII CHARACTERS AND MODULO THE SUM BY MAX AMOUNT OF ACCOUNTS
int HashTable::hashFunction(std::string nameOfOwner) const{
    int sumOfASCII = 0;
    for(size_t i = 0; i < nameOfOwner.length(); i++){
        sumOfASCII += nameOfOwner[i];
    }
    return (sumOfASCII % lengthOfTable);
}

void HashTable::pays(std::string nameOfOwner, std::string nameOfRecipient, int value){
    // CALCULATING HASH CODE OF OUR OWNER 
    int index = hashFunction(nameOfOwner);
    // IF THIS OWNER IS HERE FOR THE FIRST TIME
    if(tableOf23Accounts[index].counterOfAccounts < 10){
        bool firstTime = true;
        for(int i = 0; i < tableOf23Accounts[index].counterOfAccounts; i++){
            if(!(tableOf23Accounts[index].tableOf10Accounts[i].ownerOfAccount.compare(nameOfOwner))){
                firstTime = false;
            }
        }
        if(firstTime){
            tableOf23Accounts[index].tableOf10Accounts[tableOf23Accounts[index].counterOfAccounts].ownerOfAccount = nameOfOwner;
            tableOf23Accounts[index].counterOfAccounts++;
        }
    }
    // I CHECK EVERY RECIPIENT OF THIS OWNER FOR THE SAME RECIPIENT THAT JUST GAVE HIM MONEY
    for(int i = 0; i < tableOf23Accounts[index].counterOfAccounts; i++){
        // WE CHECK FOR THIS PARTICULAR OWNER
        if(!(tableOf23Accounts[index].tableOf10Accounts[i].ownerOfAccount.compare(nameOfOwner))){
            node *newNode;
            newNode = tableOf23Accounts[index].tableOf10Accounts[i].recipients.getHead();

            while(newNode != tableOf23Accounts[index].tableOf10Accounts[i].recipients.getTail()){
                if()
            }
            for(int j = 0; j < tableOf23Accounts[index].tableOf10Accounts[i].recipients.getLength(); j++){
                // WE CHECK FOR THIS PARTICULAR RECEIVER    
                if(!tableOf23Accounts[index].tableOf10Accounts[i].recipients.getHead())
                if(!(tableOf23Accounts[index].tableOf10Accounts[i].recipients.get(j).nameOfRecipient.compare(nameOfRecipient))){
                    tableOf23Accounts[index].tableOf10Accounts[i].recipients.changeValue(j, value);
                    return;
                }
            }
        }
    }
    // IF THERE WAS NO SUCH A RECEIVER BEFORE WE ADD HIM TO THE END OF RECIPIENTS LIST
    recipient newRecipier{nameOfRecipient, value};
    for(int i = 0; i < tableOf23Accounts[index].counterOfAccounts; i++){
        // WE CHECK FOR THIS PARTICULAR OWNER
        if(!(tableOf23Accounts[index].tableOf10Accounts[i].ownerOfAccount.compare(nameOfOwner))){
            // WE ADD NEW RECEIVER AT THE END OF THE LIST
            tableOf23Accounts[index].tableOf10Accounts[i].recipients.push_back(newRecipier);
            return;
        }
    }
}

int HashTable::checkBalance(std::string nameOfOwner, std::string nameOfRecipient){
    int index = hashFunction(nameOfOwner);
    // IF THERE IS ANY ACCOUNT UNDER THIS INDEX
    if(tableOf23Accounts[index].counterOfAccounts > 0){
        // WE CHECK FOR THIS PARTICULAR OWNER
        for(int i = 0; i < tableOf23Accounts[index].counterOfAccounts; i++){
            if(!(tableOf23Accounts[index].tableOf10Accounts[i].ownerOfAccount.compare(nameOfOwner))){
                // WE CHECK FOR THIS PARTICULAR RECEIVER
                for(int j = 0; j < tableOf23Accounts[index].tableOf10Accounts[i].recipients.size(); j++){
                    if(!(tableOf23Accounts[index].tableOf10Accounts[i].recipients.get(j).nameOfRecipient.compare(nameOfRecipient))){
                        return tableOf23Accounts[index].tableOf10Accounts[i].recipients.get(j).valueReceived;
                    }
                }
            }
        }
    }
    // IF NOT RETURN 0 DOLLARS
    return 0;
}

void HashTable::debug(){
    for(int i = 0; i < lengthOfTable; i++){
        std::cout << i << " main account : \n";
        for(int j = 0; j < tableOf23Accounts[i].counterOfAccounts; j++){
            std::cout << "          Account of " << tableOf23Accounts[i].tableOf10Accounts[j].ownerOfAccount << ". Recipients : ";
            for(int k = 0; k < tableOf23Accounts[i].tableOf10Accounts[j].recipients.size(); k++){
                std::cout << tableOf23Accounts[i].tableOf10Accounts[j].recipients.get(k).nameOfRecipient << " received " << tableOf23Accounts[i].tableOf10Accounts[j].recipients.get(k).valueReceived << ". ";
            }
            std::cout << "\n";
        }
    }
}

HashTable::~HashTable(){
    delete[] tableOf23Accounts;
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
            printf("%d\n", myHashTable.checkBalance(nameOfOwner, nameOfRecipient));
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
