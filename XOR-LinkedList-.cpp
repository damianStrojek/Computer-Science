// AISD PG WETI PROJECT NR 2
// Damian Strojek s184407 2021 IT/CS
// @ Copyright 2021, Damian Strojek, All rights reserved.
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <iomanip>

struct Node {
    int data;
    struct Node *link;
};

Node *head, *tail, *next, *prev, *actual;

Node *XOR(Node *a, Node *b){
    return (Node*) ((uintptr_t) a ^ (uintptr_t) b);
}

void ADD_BEG(int data);     //DONE
void ADD_END(int data);     //DONE
void ADD_ACT(int data);     //DONE
void ACTUAL();              //DONE
void NEXT();                //DONE 
void PREV();                //DONE 
void DEL_BEG();             //DONE
void DEL_END();             //DONE
void DEL_ACT();             //DONE
void PRINT_FORWARD();       //DONE
void PRINT_BACKWARD();      //DONE
void DEL_VAL(int data);     //DONE

int main(){
    head = NULL;
    char command[20];
    int help;
    while(std::cin >> command){
        if(!strcmp(command,"ADD_BEG")){
            std::cin >> help;
            ADD_BEG(help);
        }
        else if(!strcmp(command,"ADD_END")){
            std::cin >> help;
            ADD_END(help);
        }
        else if(!strcmp(command,"ADD_ACT")){
            std::cin >> help;
            ADD_ACT(help);
        }
        else if(!strcmp(command,"ACTUAL")){
            ACTUAL();
            printf("\n");
        }
        else if(!strcmp(command,"NEXT")){
            NEXT();
            ACTUAL();
            printf("\n");
        }
        else if(!strcmp(command,"PREV")){
            PREV();
            ACTUAL();
            printf("\n");
        }
        else if(!strcmp(command,"DEL_BEG")){
            DEL_BEG();
        }
        else if(!strcmp(command,"DEL_END")){
            DEL_END();
        }
        else if(!strcmp(command,"DEL_ACT")){
            DEL_ACT();
        }
        else if(!strcmp(command,"PRINT_FORWARD")){
            PRINT_FORWARD();
        }
        else if(!strcmp(command,"PRINT_BACKWARD")){
            PRINT_BACKWARD();
        }
        else if(!strcmp(command,"DEL_VAL")){
            std::cin >> help;
            DEL_VAL(help);
        }
        else if(!strcmp(command, "CLEAR")){
            system("cls");
            printf("AISD 2021 XOR LINKED LIST : \n");
        }
        else{
            printf("BAD COMMAND\n");
        }
    }
}

void ADD_BEG(int data){
    Node *pointer = new Node();
    pointer->data = data;

    if(head == NULL){      //GDY LISTA JEST PUSTA
        pointer->link = NULL;
        head = tail = pointer;
        actual = tail;
        next = prev = actual;
    }
    else{                   //GDY LISTA MA JAKIES ELEMENTY
        pointer->link = XOR(NULL, head);
        head->link = XOR(pointer, XOR(NULL, head->link));
        head = pointer;

        if(tail == prev){
            prev = pointer;
        }
        else if(tail == actual){
            next = pointer;
        }
    }
    
}

void ADD_END(int data){
    if(head == NULL){
        ADD_BEG(data);
    }
    else{
        Node *pointer = new Node();
        pointer->data = data;
        pointer->link = XOR(tail, NULL);
        if(tail == actual){
            next = pointer;
        }
        tail->link = XOR(pointer, XOR(tail->link, NULL));
        tail = pointer;

        if(next == head){
            next = pointer;
        }
        else if(actual == head){
            prev = pointer;
        }
    }
}

void ADD_ACT(int data){
    if(head == NULL || actual == head){
        ADD_BEG(data);
    }
    else{
        Node *pointer = new Node();
        pointer->data = data;

        pointer->link = XOR(prev, actual);
        actual->link = XOR(pointer, next);

        if(prev == head){
            prev->link = pointer;
        }
        else{
            prev->link = XOR(XOR(prev->link, actual), pointer);
        }
        prev = pointer;
    }
}

void ACTUAL(){
    if(head == NULL){
        printf("NULL");
    }
    else{
        printf("%d ", actual->data);
    }
}

void PRINT_FORWARD(){
    Node *tempA, *tempN, *tempP;
    tempA = actual;
    tempN = next;
    tempP = prev;

    if(head == NULL){
        printf("NULL\n");
    }
    else if(head == tail){
        printf("%d\n", head->data);
    }
    else{
        prev = tail;
        actual = head;
        next = XOR(NULL, actual->link);
        printf("%d ", actual->data);

        do{
            NEXT();
            ACTUAL();
        }while(actual != tail);

        printf("\n");
    }

    actual = tempA;
    next = tempN;
    prev = tempP;
}

void PRINT_BACKWARD(){
    Node *tempA, *tempN, *tempP;
    tempA = actual;
    tempN = next;
    tempP = prev;

    if(head == NULL){
        printf("NULL\n");
    }
    else if(head == tail){
        printf("%d\n", head->data);
    }
    else{
        actual = tail;
        next = head;
        prev=XOR(tail->link, NULL);
        printf("%d ", actual->data);
        do{
            PREV();
            ACTUAL();
        }while(actual != head);

        printf("\n");
    }

    actual = tempA;
    next = tempN;
    prev = tempP;
}

void NEXT(){
    if(head == NULL){
        return;
    }
    else if(next != tail && actual != tail){       //GDY JESTESMY W SRODKU
        prev = actual;                      //PREV
        actual = next;                      //ACTUAL
        next = XOR(prev, actual->link);     //NEXT
    }
    else if(next == tail){       //GDY ACTUAL BĘDZIE NA KOŃCU
        prev = actual;                      //PREV
        actual = tail;                      //ACTUAL
        next = head;                       //NEXT
    }
    else if(next == head && actual == tail){     //GDY PRZESKAKUJEMY
        prev = tail;                         //PREV
        actual = head;                     //ACTUAL
        next = XOR(NULL, actual->link);     //NEXT
    }
}

void PREV(){
    if(head == NULL){
        return;
    }
    else if(prev != head && actual != head){     //GDY JESTESMY W SRODKU
        next = actual;                      //NEXT
        actual = prev;                      //ACTUAL
        prev = XOR(actual->link, next);     //PREV
    }
    else if(prev == head){     //GDY ACTUAL BEDZIE NA POCZATKU
        next = actual;                      //NEXT
        actual = prev;                      //ACTUAL
        prev = tail;                         //PREV
    }
    else if(prev == tail && actual == head){       //GDY PRZESKAKUJEMY W TYŁ
        actual = prev;                     //ACTUAL
        next = head;                      //NEXT
        prev = XOR(prev->link, NULL);       //PREV
    }
}

void DEL_BEG(){
    if(head == NULL){
        return;
    }
    else{
        if(head == next){
            next = XOR(NULL, head->link);
        }
        else if(head == actual){
            actual = tail;
            prev = XOR(tail->link, NULL);
        }
        else if(head == prev){
            prev = tail;
        }

        if(head == tail){
            delete head;
            head = NULL;
        }
        else{
            Node *temp = head;
            Node *nextHEAD = XOR(NULL, head->link);
            if(nextHEAD == NULL){
                head = NULL;
            }
            else{
                nextHEAD->link = XOR(temp, XOR(nextHEAD->link, NULL));
            }

            head = nextHEAD;
            delete temp;
        }

    }
}

void DEL_END(){
    if(head == NULL){
        return;
    }
    else{
        if(tail == actual && prev != NULL){
            actual = prev;
            prev = XOR(actual->link, tail);
            next = head;
        }
        else if(tail == next){
            next = head;
        }
        else if(tail == prev){
            prev = XOR(prev->link, NULL);
        }

        if(head == tail){
            delete head;
            head = NULL;
        }
        else{
            Node *temp = tail;
            Node *prevTAIL = XOR(temp->link, NULL);
            if(prevTAIL == NULL){
                head = NULL;
            }
            else{
                prevTAIL->link = XOR(temp, XOR(prevTAIL->link, NULL));
            }
            tail = prevTAIL;
            
            delete temp;
        }
    }
}

void DEL_ACT(){
    if(actual == head){
        DEL_BEG();
    }
    else if(actual == tail){
        DEL_END();
    }
    else if(head == tail){
        delete head;
        head = NULL;
    }
    else{
        Node *temp = actual;
        actual = prev;
        prev = XOR(prev->link, temp);
        actual->link = XOR(next, prev);
        next->link = XOR(XOR(next->link, temp), actual);
        if(prev == head){
            prev->link = actual;
        }
        if(next == tail){
            next->link = actual;
        }
        delete temp;
    }
}

void DEL_VAL(int data){
    if(head != NULL){
        int howMany = 0;

        Node *temp = actual;
        int tempData = actual->data;
        actual = head;
        prev = tail;
        next = XOR(NULL, head->link);

        do{
            if(actual->data == data){
                howMany++;
            }
            NEXT();
        }while(actual != head);

        do{
            if(actual->data == data){
                DEL_ACT();
                howMany--;
            }
            NEXT();
        }while(howMany != 0);

        if(tempData == data){
            actual = temp;
        }
        else{
            actual = tail;
        }
    }
}
