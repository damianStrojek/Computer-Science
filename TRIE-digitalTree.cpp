// AISD PG WETI LABS NR 6
// Damian Strojek s184407 2021 IT/CS
// @ Copyright 2021, Damian Strojek, All rights reserved.
#include <iostream>
#define CHAR_SIZE 128

class Trie {
    private:
        bool isLeaf;
        Trie *character[CHAR_SIZE];
        std::string translation;
    public:
        Trie();
        void insert(std::string word, std::string translation);
        void search(std::string word);
        void searchAll(std::string word);
        void searchIn(Trie *curr);
};

Trie::Trie(){
    this->isLeaf = 0;
    for(int i = 0; i < CHAR_SIZE; i++){
        this->character[i] = NULL;
    }
};

void Trie::insert(std::string word, std::string _translation){
    Trie *curr = this;
    for(int i = 0; i < word.length(); i++){
        if(curr->character[word[i]] == NULL){
            curr->character[word[i]] = new Trie();
        }
        
        curr = curr->character[word[i]];
    }
    curr->isLeaf = 1;
    curr->translation = _translation;
};

void Trie::search(std::string word){
    if(this == NULL){
        printf("-\n");
        return;
    }
    else{
        Trie *curr = this;
        for(int i = 0; i < word.length(); i++){
            curr = curr->character[word[i]];
            if(curr == NULL){
                printf("-\n");
                return;
            }
        }
        // Jeżeli jeszcze nie wróciliśmy to znaczy że zdania się zgadzają
        if(curr->isLeaf){
            std::cout << curr->translation << "\n";
        }
        else{
            printf("-\n");
            return;
        }
    }
};

void Trie::searchAll(std::string word){
    if(this == NULL){
        printf("-\n");
        return;
    }
    else{
        Trie *curr = this;
        for(int i = 0; i < word.length(); i++){
            curr = curr->character[word[i]];
            if(curr == NULL){
                printf("-\n");
                return;
            }
        }

        searchIn(curr);
    }
};

void Trie::searchIn(Trie *curr){
    if(curr->isLeaf){
        std::cout << curr->translation << "\n";
    }
    for(int i = 0; i < CHAR_SIZE; i++){
        if(curr->character[i] != NULL){
            searchIn(curr->character[i]);
        }
    }
};

int main(){
    Trie *head = new Trie();
    std::string word, translation;
    while(std::cin >> word){
        if(!word.compare("+")){
            std::cin >> word >> translation;
            head->insert(word, translation);
        }
        else if(!word.compare("?")){
            std::cin >> word;
            head->search(word);
        }
        else if(!word.compare("*")){
            std::cin >> word;
            head->searchAll(word);
        }
        else{
            printf("BAD COMMAND\n");
        }
    }
    return 0;
};
