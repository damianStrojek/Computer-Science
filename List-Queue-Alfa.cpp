//  AISD PG WETI PROJECT NR 2
//  Created by Damian Strojek 184407 on 15/04/2021 
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string>

struct Node {
    int data;
    Node *next, *prev, *npx;
};

Node *XOR(Node *a, Node *b){
    return (Node*) ((uintptr_t) a ^ (uintptr_t) b);
};

class LinkedList {
    private:
        int lengthList;
        Node *actual, *head, *tail, *front, *back;
    public:
        LinkedList();
        void ADD_BEG(int data);                    // DONE
        void ADD_END(int data);                    // DONE
        void ADD_ACT(int data);                    // DONE
        void ACTUAL();                             // DONE
        void NEXT();                               // DONE
        void PREV();                               // DONE
        void DEL_BEG();                            // DONE
        void DEL_END();                            // DONE
        void DEL_ACT();                            // DONE
        void PRINT_FORWARD();                      // DONE
        void PRINT_BACKWARD();                     // DONE
        void DEL_VAL(int data);                    // DONE
        // 3 project
        void SIZE();                               // DONE
        int COUNT();                               // DONE
        void PUSH(int data);                       // DONE
        void POP();                                // DONE
        void PRINT_QUEUE();                        // DONE
        void GARBAGE_SOFT();                       // DONE
        void GARBAGE_HARD();                       // DONE
        ~LinkedList();
};

int main(){
    LinkedList LinkedList;
    char command[20];
    int data;
    while(std::cin >> command){
        if(command == "ADD_BEG"){
            std::cin >> data;
            LinkedList.ADD_BEG(data);
        }
        else if(command == "ADD_END"){
            std::cin >> data;
            LinkedList.ADD_END(data);
        }
        else if(command == "ADD_ACT"){
            std::cin >> data;
            LinkedList.ADD_ACT(data);
        }
        else if(command == "DEL_VAL"){
            std::cin >> data;
            LinkedList.DEL_VAL(data);
        }
        else if(command == "ACTUAL"){
            LinkedList.ACTUAL();
        }
        else if(command == "NEXT"){
            LinkedList.NEXT();
            printf("\n");
        }
        else if(command == "PREV"){
            LinkedList.PREV();
            printf("\n");
        }
        else if(command == "DEL_BEG"){
            LinkedList.DEL_BEG();
        }
        else if(command == "DEL_END"){
            LinkedList.DEL_END();
        }
        else if(command == "DEL_ACT"){
            LinkedList.DEL_ACT();
        }
        else if(command == "PRINT_FORWARD"){
            LinkedList.PRINT_FORWARD();
        }
        else if(command == "PRINT_BACKWARD"){
            LinkedList.PRINT_BACKWARD();
        }
        else if(command == "CLEAR"){
            system("cls");
            printf("AISD 2021 XOR LINKED LIST : \n");
        }
        else if(command == "SIZE"){
            LinkedList.SIZE();
        }
        else if(command == "COUNT"){
            printf("%d\n", LinkedList.COUNT());
        }
        else if(command == "PUSH"){
            std::cin >> data;
            LinkedList.PUSH(data);
        }
        else if(command == "POP"){
            LinkedList.POP();
        }
        else if(command == "PRINT_QUEUE"){
            LinkedList.PRINT_QUEUE();
        }
        else if(command == "GARBAGE_SOFT"){
            LinkedList.GARBAGE_SOFT();
        }
        else if(command == "GARBAGE_HARD"){
            LinkedList.GARBAGE_HARD();
        }
        else{
            printf("BAD COMMAND\n");
        }
    }
    return 0;
}

LinkedList::LinkedList(){
    head = NULL;
    actual = NULL;
    tail = NULL;
    front = NULL;
    back = NULL;
    lengthList = 0;
}

void LinkedList::ADD_BEG(int data){
    Node *newNode = new Node{data, nullptr};
    lengthList++;

    // IF LINKED LIST IS EMPTY
    if(head == NULL){
        newNode->npx = NULL;
        newNode->next = newNode->prev = newNode;
        head = tail = actual = newNode;
    }
    // ELSE LINKED LIST HAVE ELEMENTS AND YOU NEED TO REARRANGE NPXS
    else{
        Node *temp = head;

        newNode->npx = XOR(NULL, head);
        head->npx = XOR(newNode, XOR(NULL, head->npx));

        temp->prev = newNode;
        newNode->next = temp;
        newNode->prev = tail;

        head = newNode;
    }
}

void LinkedList::ADD_END(int data){
    if(head == NULL){
        ADD_BEG(data);
    }
    // REARRANGE NPXS IN THE END OF THE LIST
    else{
        Node *newNode = new Node{data, nullptr};
        lengthList++;

        Node *temp = tail;
        newNode->npx = XOR(tail, NULL);
        tail->npx = XOR(newNode, XOR(tail->npx, NULL));
        
        temp->next = newNode;
        newNode->next = head;
        newNode->prev = temp;

        tail = newNode;
    }
}

void LinkedList::ADD_ACT(int data){
    if(head == NULL || head == actual){
        ADD_BEG(data);
    }
    else{
        Node *newNode = new Node{data, nullptr};
        lengthList++;

        Node *temp = actual->prev;
        newNode->npx = XOR(actual->prev, actual);

        temp->next = newNode;
        actual->prev = newNode;
        newNode->prev = temp;
        newNode->next = actual;
    }
}

void LinkedList::DEL_BEG(){
    if(head == NULL){
        return;
    }
    else{
        lengthList--;

        // IF BEGIN OF LIST IS QUEUE BACK
        if(back == head && front != head){
            back = head->next;
        }
        // IF BEGIN OF LIST IS QUEUE FRONT
        else if(front == head && back != head){
            front = tail;
        }
        // IF ITS LAST NODE OF QUEUE
        else if(front == back && front == head){
            front = NULL;
            back = NULL;
        }
        // IN CASE THERE IS ONLY ONE NODE
        if(actual->next == actual && actual->prev == actual){
            delete actual;
            head = NULL;
            front = back = head;
            return;
        }

        Node *temp = head;
        tail->next = head->next;
        head->next->prev = tail;
        head = head->next;
        if(actual == temp){
            actual = tail;
        }
        delete temp;
    }
}

void LinkedList::DEL_END(){
    if(head == NULL){
        return;
    }
    else{
        lengthList--;

        // IF FRONT IS IN THE END OF THE LIST
        if(front == tail){
            front = tail->prev;
        }
        // IF BACK IS IN THE END OF THE LIST
        else if(back == tail){
            back = head;
        }
        // IF THERE IS ONLY ONE NODE IN QUEUE
        else if(front == back && front == tail){
            front = NULL;
            back = NULL;
        }
        // IF THERE IS ONLY ONE NODE IN LIST
        if(actual->next == actual && actual->prev == actual){
            delete actual;
            head = NULL;
            front = back = head;
            return;
        }

        Node *temp = tail;
        head->prev = tail->prev;
        tail->prev->next = head;
        tail = tail->prev;
        if(actual == temp){
            actual = tail;
        }
        delete temp;
    }
}

void LinkedList::DEL_ACT(){
    if(head == NULL){
        return;
    }
    else if(actual == head){
        DEL_BEG();
    }
    else if(actual == tail){
        DEL_END();
    }
    else{
        lengthList--;
        // IF FRONT IS IN THE ACTUAL WE BACK IT UP
        if(front == actual){
            front = actual->prev;
        }
        // IF BACK IS IN THE ACTUAL WE CHANGE IT TO THE NEXT
        else if(back == actual){
            back = actual->next;
        }
        // IF THERE IS ONLY ONE NODE IN QUEUE
        else if(front == back && front == actual){
            front = NULL;
            back = NULL;
        }
        // IF THERE IS ONLY ONE NODE IN LIST
        if(actual->next == actual && actual->prev == actual){
            delete actual;
            head = NULL;
            front = back = head;
            return;
        }

        Node *temp = actual;
        actual->next->prev = actual->prev;
        actual->prev->next = actual->next;
        actual = actual->prev;
        delete temp;
    }

}

void LinkedList::ACTUAL(){
    if(head == NULL){
        printf("NULL\n");
    }
    else{
        printf("%d\n", actual->data);
    }
}

void LinkedList::NEXT(){
    if(head == NULL){
        printf("NULL");
    } 
    // WE NEED TO JUMP
    else if(tail == actual){
        printf("%d ", head->data);
        actual = head;
    }
    else{
        printf("%d ", actual->next->data);
        actual = actual->next;
    }
}

void LinkedList::PREV(){
    if(head == NULL){
        printf("NULL");
    }
    // WE NEED TO JUMP
    else if(actual == head){
        printf("%d ", tail->data);
        actual = tail;
    }
    else{
        printf("%d ", actual->prev->data);
        actual = actual->prev;
    }
}

void LinkedList::PRINT_FORWARD(){
    Node *tempA;
    tempA = actual;

    if(head == NULL){
        printf("NULL\n");
    }
    // IN CASE ONLY ONE NODE
    else if(head == tail){
        printf("%d\n", head->data);
    }
    else{
        actual = head;
        printf("%d ", actual->data);
        do{
            NEXT();
        }while(actual != tail);

        printf("\n");
    }
    actual = tempA;
}

void LinkedList::PRINT_BACKWARD(){
    Node *tempA;
    tempA = actual;

    if(head == NULL){
        printf("NULL\n");
    }
    // IN CASE ONLY ONE NODE
    else if(head == tail){
        printf("%d\n", head->data);
    }
    else{
        actual = tail;
        printf("%d ", actual->data);
        do{
            PREV();
        }while(actual != head);

        printf("\n");
    }
    actual = tempA;
}

void LinkedList::DEL_VAL(int data){
    if(head == NULL){
        return;
    }
    else{
        Node *temp = actual;
        int tempData = actual->data;
        actual = head;
        int howMany = 0;
        // FIRST COUNTING NODES
        do{
            if(actual->data == data){
                howMany++;
            }

            if(actual == tail){
                actual = head;
            }
            else{
                actual = actual->next;
            }
        }while(actual != head);
        // DELETING THOSE NODES, HAD TO COUNT THEM TO BE SURE I DELETED EVERY NODE THERE IS
        do{
            if(actual->data == data){
                DEL_ACT();
                howMany--;
            }

            if(actual == tail){
                actual = head;
            }
            else{
                actual = actual->next;
            }
        }while(howMany != 0);
        // IN CASE WE DELETED ACTUAL
        if(tempData == data){
            actual = tail;
        }
        else{
            actual = temp;
        }
    }
}

void LinkedList::SIZE(){
    printf("%d\n", lengthList);
}

int LinkedList::COUNT(){
    if(front == NULL){
        return 0;
    }
    else if(front == back){
        return 1;
    }
    else{
        Node *tempA = actual;
        actual = front;
        int lengthQueue = 0;
        do{
            lengthQueue++;
            if(actual == head){
                actual = tail;
            }
            else{
                actual = actual->prev;
            }
        }while(actual != back);
        // BECAUSE OF THE BACK NODE
        lengthQueue++;
        actual = tempA;
        return lengthQueue;
    }
}

void LinkedList::PUSH(int data){
    // IF LIST AND QUEUE ARE EMPTY
    if(head == NULL){
        ADD_BEG(data);
        front = back = head;
    }
    else if(front == NULL){
        tail->data = data;
        front = tail;
        back = front;
    }
    // IF LIST HAS SOME NODES
    else{  
        // IF THEY ARE NOT EQUAL
        if(COUNT() != lengthList){
            if(back == head){
                back = tail;
                back->data = data;
            }
            else{
                back = back->prev;
                back->data = data;
            }

        }
        // IF THEY ARE EQUAL
        else{
            Node *tempA = actual;
            actual = back;
            ADD_ACT(data);
            back = actual->prev;
            actual = tempA;
        }
    }
}

void LinkedList::POP(){
    if(front == NULL){
        printf("NULL\n");
    }
    else{
        printf("%d\n", front->data);
        // IF THERE IS ONLY ONE ELEMENT IN QUEUE
        if(front == back){
            front = NULL;
            back = NULL;
        }
        // ELSE WE JUST MOVE OUR FRONT "BACKWARDS"
        else{
            front = front->prev;
        }
    }
}

void LinkedList::PRINT_QUEUE(){
    if(front == NULL){
        printf("NULL\n");
    }
    else if(front == back){
        printf("%d\n", front->data);
    }
    else{
        Node *tempA = actual;
        actual = front;

        do{
            printf("%d ", actual->data);
            if(actual == head){
                actual = tail;
            }
            else{
                actual = actual->prev;
            }
        }while(actual != back);

        printf("%d\n", back->data);
        actual = tempA;
    }
}

void LinkedList::GARBAGE_SOFT(){
    if(head == NULL || lengthList == COUNT()){
        return;
    }
    else if(front == NULL){
        Node *tempA = actual;
        actual = head;
        while(actual != tail){
            actual->data = 0;
            if(actual == tail){
                actual = head;
            }
            else{
                actual = actual->next;
            }
        }
        actual->data = 0;
        actual = tempA;
    }
    else{
        Node *tempA = actual;
        actual = front->next;


        while(actual != back){
            actual->data = 0;
            if(actual == tail){
                actual = head;
            }
            else{
                actual = actual->next;
            }
        }

        actual = tempA;
    }
}

void LinkedList::GARBAGE_HARD(){
    if(head == NULL || lengthList == COUNT()){
        return;
    }
    else if(front == NULL){
        while(head != NULL){
            DEL_ACT();
        }
    }
    else{
        Node *tempA = actual;
        actual = front->next;

        while(actual != back){
            DEL_ACT();
            if(actual == tail){
                actual = head;
            }
            else{
                actual = actual->next;
            }
        }

        if(tempA == NULL){
            actual = tail;
        }
        else{    
            actual = tempA;
        }
    }
}

LinkedList::~LinkedList(){
    actual = head;
    while(head != NULL){
        DEL_ACT();
    }
}