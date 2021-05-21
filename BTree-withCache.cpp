// AISD PG WETI PROJECT NR 4
// Damian Strojek s184407 2021 IT/CS
// @ Copyright 2021, Damian Strojek, All rights reserved.
// The code I wrote is based on the theory described in the book "Introduction to Algorithms"
// by Thomas H. Cormen, Charles E. Leiserson and Ronald L. Rivest
#include <iostream>     // input output
#include <string>       // std::string
#include <sstream>      // istringstream(std::string)
#define UNDEFINED -1

struct BTreeNode {
    int order;
    int numberOfKeys;
    int *keys;
    bool isLeaf;
    BTreeNode **childs;
};

struct BTree {
    BTreeNode *root;
    int order;
};

struct nodeLFU {
    int valueInCache = 0;
    int frequencyOfUsage = UNDEFINED;
};

void setOrder(BTree &tree, int newOrder);                       // Setting new order for the tree
void insertNewKey(BTree &tree, int key);                        // Inserting new key value to the tree
void splitChild(BTreeNode *x, int i, BTreeNode *y);             // Spliting childs
void insertNonFull(BTreeNode *x, int key);                      // Inserting key value when node is not full
void printTree(BTreeNode *x);                                   // Printing tree normally
void printTreeSave(BTreeNode *x);                               // Printing tree with brackets (nodes)
bool searchTree(BTreeNode *x, int key);                         // Searching the tree for key value
void removeKey(BTree tree, int key);                            // Removing key value from tree
void removeKeyFromNode(BTreeNode *x, int key);                  // Removing key value from node
int findIndex(BTreeNode *x, int key);                           // Finding index of a key value
void removeKeyFromLeaf(BTreeNode *x, const int index);          // Removing key value from leaf node
void removeKeyFromNonLeaf(BTreeNode *x, const int index);       // Removing key value from non leaf node
int getPredecessor(BTreeNode *x, const int index);              // Getting largest key on the left child of a node
int getSuccesor(BTreeNode *x, const int index);                 // Getting smallest key on the right child of a node
void fillNode(BTreeNode *x, const int index);                   // Filling node when removing key value
void borrowFromPrevious(BTreeNode *x, const int index);         // Node x->childs[index] borrows key from x->childs[index-1] 
void borrowFromNext(BTreeNode *x, const int index);             // Node x->childs[index] borrows key from x->childs[index+1]        
void mergeNode(BTreeNode *x, const int index);                  // Merging nodes x->childs[index] and x->childs[index+1]
void loadNewTree(BTree &tree, int newOrder);                    // Loading new tree with new order and calling load node on root
void loadNewNode(BTreeNode *x);                                 // Recursively loading new nodes onto this particular tree
int searchLevelOfTheKey(BTreeNode *x, int key, int level);      // Returning height at which our key value is
int searchAccessOfTheKey(BTreeNode *x, int key, int access);    // Returning access time that takes to find our key value
void calculateCache(BTree tree, int cacheSize);                 // Calculating cache impact in mr. Tytus way
void calculateCacheLFU(BTree tree, int cacheSize);              // Calculating cache impact according to Least Frequently Used policy (LFU)
void destructorOfTree(BTree &tree);                             // Freeing space after we are done with the program
int getFirstKey(BTreeNode *x);                                  // Returns first key in node

int main(){
    std::string command;
    int key;
    BTree tree{NULL, UNDEFINED};
    
    while(std::cin >> command){
        if(command == "I"){
            std::cin >> key;
            setOrder(tree, key);
        }
        else if(command == "A"){
            std::cin >> key;
            insertNewKey(tree, key);
        }
        else if(command == "P"){
            printTree(tree.root);
            printf("\n");
        }
        else if(command == "?"){
            std::cin >> key;
            if(searchTree(tree.root, key)){
                printf("%d +\n", key);
            }
            else{
                printf("%d -\n", key);
            }
        }
        else if(command == "R"){
            std::cin >> key;
            removeKey(tree, key);
        }
        else if(command == "C"){
            std::cin >> key;
            calculateCache(tree, key);
        }
        else if(command == "#"){
            std::string comment;
            std::getline(std::cin, comment);
        }
        else if(command == "X"){
            exit(1);
        }
        else if(command == "S"){ 
            printf("%d\n", tree.order);
            printTreeSave(tree.root);
            printf("\n");
        }
        else if(command == "L"){
            std::cin >> key;
            loadNewTree(tree, key);
        }
        else if(command == "CLFU"){
            std::cin >> key;
            calculateCacheLFU(tree, key);
        }
    }
    destructorOfTree(tree);
    return 0;
};

void setOrder(BTree &tree, int newOrder){
    // Changing order of the tree, we do this with every I command
    tree.order = newOrder;
};

void insertNewKey(BTree &tree, int key){
    // If our tree is empty we allocate memory for new root and add new key to root
    if(tree.root == NULL){
        tree.root = new BTreeNode{tree.order, 1, new int[2*tree.order-1], true, new BTreeNode *[2*tree.order]};
        tree.root->keys[0] = key;
    }
    // If it's not empty we allocate new node if root is full or we add to the root
    else{
        BTreeNode *newNode = tree.root;
        if(tree.root->numberOfKeys == 2*tree.order-1){
            BTreeNode *newRoot = new BTreeNode{tree.order, 0, new int[2*tree.order-1], false, new BTreeNode *[2*tree.order]};
            tree.root = newRoot;
            newRoot->childs[0] = newNode;
            splitChild(newRoot, 0, newNode);
            insertNonFull(newRoot, key);
        }
        else{
            insertNonFull(newNode, key);
        }
    }
};

void splitChild(BTreeNode *x, int i, BTreeNode *y){
    // We allocate memory for new Node that is going to be children of our x
    BTreeNode *z = new BTreeNode{x->order, x->order-1, new int[2*x->order-1], y->isLeaf, new BTreeNode *[2*x->order]};
    // We move keys from y to newly allocated node
    for(int j = 0; j < x->order-1; j++){
        z->keys[j] = y->keys[j+y->order];
    }
    // If y wasn't a leaf we need to move childrens too
    if(!(y->isLeaf)){
        for(int j = 0; j < y->order; j++){
            z->childs[j] = y->childs[j+y->order];
        }
    }
    y->numberOfKeys = y->order-1;                       // We adjust the key count for y
    // Create space for a node that we allocated before
    for(int j = x->numberOfKeys; j >= i+1; j--){        
        x->childs[j+1] = x->childs[j];
    }
    x->childs[i+1] = z;                                 // Putting this node to the children array
    // Moveing keys in our x node
    for(int j = x->numberOfKeys-1; j >= i; j--){ 
        x->keys[j+1] = x->keys[j];
    }
    x->keys[i] = y->keys[y->order-1];                   // and finally moving the median key from y up to x node
    // Adjusting x's keys count
    x->numberOfKeys++;    
};

void insertNonFull(BTreeNode *x, int key){
    int i = x->numberOfKeys-1;
    // Inserting key into node x
    if(x->isLeaf){
        while(i >= 0 && key < x->keys[i]){
            x->keys[i+1] = x->keys[i];
            i--;
        }
        x->keys[i+1] = key;
        x->numberOfKeys++;
    }
    // If node x is not leaf we need to make space for new value
    else{
        // We look for good place to store it
        while(i >= 0 && key < x->keys[i]){
            i--;
        }
        
        i++;                
        if(x->childs[i]->numberOfKeys == 2*x->order-1){     // We check if this is full child
            splitChild(x, i, x->childs[i]);                 // if it is we need to split this child
            if(key > x->keys[i]){
                i++;
            }
        }
        insertNonFull(x->childs[i], key);                   // Finally we recursionaly add this value
    }
};

void printTree(BTreeNode *x){
    if(x != NULL){
        // Recursively printing every node from left to right
        int i;
        for(i = 0; i < x->numberOfKeys; i++){
            if(!x->isLeaf){
                printTree(x->childs[i]);
            }
            printf("%d ", x->keys[i]);
        }

        if(!x->isLeaf){
            printTree(x->childs[i]);
        }
    }
};

void printTreeSave(BTreeNode *x){
    if(x != NULL){
        // Recursively printing every node from left to right but we add brackets that means Node
        printf("( ");
        int i;
        for(i = 0; i < x->numberOfKeys; i++){
            if(!x->isLeaf){
                printTreeSave(x->childs[i]);
            }
            printf("%d ", x->keys[i]);
        }

        if(!x->isLeaf){
            printTreeSave(x->childs[i]);
        }
        printf(") ");
    }
};

bool searchTree(BTreeNode *x, int key){
    int i = 0;

    // Looking for a place where our key could be (because its organized)
    while(i < x->numberOfKeys && key > x->keys[i]){
        i++;
    }
    // If keys are matching we return true
    if(i < x->numberOfKeys && key == x->keys[i]){
        return true;
    }
    // If this is leaf we have nowhere else to go
    else if(x->isLeaf){
        return false;
    }
    // If this is not leaf we are searching childrens of this node
    return (searchTree(x->childs[i], key));
};

void removeKey(BTree tree, int key){
    // If there is nothing to be removed
    if(tree.root == NULL){
        return;
    }
    // Removing key
    removeKeyFromNode(tree.root, key);
};

void removeKeyFromNode(BTreeNode *x, int key){
    // Looking for index of this key/node
    const int index = findIndex(x, key);
    // If this is right index
    if(index < x->numberOfKeys && x->keys[index] == key){
        // 1 situation : If it is leaf node we just change the structure of keys
        if(x->isLeaf){
            removeKeyFromLeaf(x, index);
        }
        // 2 situation : We know at what index is our node so we dont need to give "key"
        else{
            removeKeyFromNonLeaf(x, index);
        }
    }
    // 3 situation : We look for this key in our nodes
    else{
        // If this is leaf we can close the function, this value doesnt exist in our tree
        if(x->isLeaf){
            return;
        }
        // If our index equals number of keys in this node we mark flag true
        bool flag = ((index == x->numberOfKeys) ? true : false);
        // We need to fill our x node
        if(x->childs[index]->numberOfKeys < x->order){
            fillNode(x, index);
        }
        // If number of keys equals our index 
        if(flag && index > x->numberOfKeys){
            removeKeyFromNode(x->childs[index-1], key);
        }
        else{
            removeKeyFromNode(x->childs[index], key);
        }
    }
};

int findIndex(BTreeNode *x, int key){
    // Returning index at which our key might be
    int index = 0;
    while(index < x->numberOfKeys && x->keys[index] < key){
        index++;
    }
    return index;
};

void removeKeyFromLeaf(BTreeNode *x, const int index){
    // Simply removing our key from the scope by moving all keys
    // I change this key to UNDEFINED and then I replace it with i+1 key
    x->keys[index] = UNDEFINED;
    for(int i = index+1; i < x->numberOfKeys; i++){
        x->keys[i-1] = x->keys[i];
    }
    x->numberOfKeys--;
};

void removeKeyFromNonLeaf(BTreeNode *x, const int index){
    // Our key that we want to remove, i save it because of the merge
    int k = x->keys[index];
    // Situation a
    // If the child hash at least order keys then find the predecessor key' of k in the subtree
    // rooted at this index. Recursively delete key' and replace key by key' in x.
    if(x->childs[index]->numberOfKeys >= x->order){
        int predecessor = getPredecessor(x, index);           // We get predecessor of our key
        x->keys[index] = predecessor;                        // set it in our node x
        removeKeyFromNode(x->childs[index], predecessor);    // and call to remove this predecessor from node y
    }
    // Situation b
    // If the child z that follows k in node x has at least order keys we find the successor key' of key in the subtree
    // rooted at z. Recursively delete key' and replace key by key' in x.
    else if(x->childs[index+1]->numberOfKeys >= x->order){
        int succesor = getSuccesor(x, index);               // We get succesor of our key
        x->keys[index] = succesor;                          // set it in our node x
        removeKeyFromNode(x->childs[index+1], succesor);    // and call to remove this succesor from node y
    }
    // Situation c
    else{
        mergeNode(x, index);
        removeKeyFromNode(x->childs[index], k);
    }
};

int getPredecessor(BTreeNode *x, const int index){
    // Getting largest key on the left child of a node
    BTreeNode *temp = x->childs[index];
    while(!(temp->isLeaf)){
        temp = temp->childs[temp->numberOfKeys];
    }
    return temp->keys[temp->numberOfKeys-1];
};

int getSuccesor(BTreeNode *x, const int index){
    // Getting smallest key on the right child of a node
    BTreeNode *temp = x->childs[index+1];
    while(!(temp->isLeaf)){
        temp = temp->childs[0];
    }
    return temp->keys[0];
};

void fillNode(BTreeNode *x, const int index){
    // Filling by borrowing from index-1
    if((index != 0) && (x->childs[index-1]->numberOfKeys >= x->order)){
        borrowFromPrevious(x, index);
    }
    // Filling by borrowing from index+1
    else if((index != x->numberOfKeys) && (x->childs[index+1]->numberOfKeys >= x->order)){
        borrowFromNext(x, index);
    }
    // Merging 
    else{
        if(index != x->numberOfKeys){
            mergeNode(x, index);
        }
        // Merging with node at index-1
        else{
            mergeNode(x, index-1);
        }
    }
};

void borrowFromPrevious(BTreeNode *x, const int index){
    BTreeNode *child = x->childs[index];                // Our child in this index
    BTreeNode *sibling = x->childs[index-1];            // Sibling to the left of our child
    // Making space in our child
    for(int i = child->numberOfKeys-1; i >= 0; i--){
        child->keys[i+1] = child->keys[i];      
    }
    // If our child is not leaf we need to make space in his childs
    if(!(child->isLeaf)){
        for(int i = child->numberOfKeys; i >= 0; i--){
            child->childs[i+1] = child->childs[i];
        }
    }
    // In empty space we put key from x
    child->keys[0] = x->keys[index-1];
    // We check again if it is not a leaf
    if(!(child->isLeaf)){
        child->childs[0] = sibling->childs[sibling->numberOfKeys];
    }
    // To the empty space of x node we add siblings key most to the right
    x->keys[index-1] = sibling->keys[sibling->numberOfKeys-1];
    sibling->keys[sibling->numberOfKeys-1] = UNDEFINED;                  // and make it undefined

    child->numberOfKeys++;
    sibling->numberOfKeys--;
};

void borrowFromNext(BTreeNode *x, const int index){
    BTreeNode *child = x->childs[index];                // Our child in this index
    BTreeNode *sibling = x->childs[index+1];            // Sibling to the right of our child
    // our index is going to child
    child->keys[child->numberOfKeys] = x->keys[index];
    // If our child is not leaf we make it child of sibling
    if(!(child->isLeaf)){
        child->childs[child->numberOfKeys+1] = sibling->childs[0];
    }
    // we change key value at this index to most-left value in sibling's array
    x->keys[index] = sibling->keys[0];
    // and we push all values in sibling to the left so there is no blank space
    sibling->keys[0] = UNDEFINED;
    for(int i = 1; i < sibling->numberOfKeys; i++){
        sibling->keys[i-1] = sibling->keys[i];
    }
    // If sibling is not leaf we need to push all childs of this sibling to the left so we dont have any blank space
    if(!(sibling->isLeaf)){
        for(int i = 1; i <= sibling->numberOfKeys; i++){
            sibling->childs[i-1] = sibling->childs[i];
        }
    }
    // Adjust number of keys in every node
    child->numberOfKeys++;
    sibling->numberOfKeys--;
};

void mergeNode(BTreeNode *x, const int index){
    BTreeNode *child = x->childs[index];                // child at this index
    BTreeNode *sibling = x->childs[index+1];            // sibling to the right
    // Our index value is going to the child
    child->keys[child->order-1] = x->keys[index];
    // We move key values from sibling to our child
    for(int i = 0; i < sibling->numberOfKeys; i++){
        child->keys[i+child->order] = sibling->keys[i];
    }
    // and we do the same with childs of sibling if they exist
    if(!(child->isLeaf)){
        for(int i = 0; i <= sibling->numberOfKeys; i++){
            child->childs[i+child->order] = sibling->childs[i];
        }
    }

    // Then we rearrange our keys in x node
    x->keys[index] = UNDEFINED;
    for(int i = index+1; i < x->numberOfKeys; i++){
        x->keys[i-1] = x->keys[i];
    }
    // and do the same for childs of our x node
    x->childs[index+1] = NULL;
    for(int i = index+2; i <= x->numberOfKeys; i++){
        x->childs[i-1] = x->childs[i];
    }
    // Adjust the number of keys in every node
    child->numberOfKeys += sibling->numberOfKeys+1;
    x->numberOfKeys--;
    delete sibling;
};

void loadNewTree(BTree &tree, int newOrder){
    setOrder(tree, newOrder);
    tree.root = new BTreeNode{newOrder, 0, new int[2*newOrder-1], true, new BTreeNode *[2*newOrder]};
    std::string keysLoad;
    std::cin >> keysLoad;
    if(keysLoad == "("){
        loadNewNode(tree.root);
    }
};

void loadNewNode(BTreeNode *x){
    int childsLoaded = 0;
    std::string keysLoad;
    // Loading up all values and brackets
    while(std::cin >> keysLoad){
        if(keysLoad == "("){
            x->isLeaf = false;
            x->childs[childsLoaded] = new BTreeNode{x->order, 0, new int[2*x->order-1], true, new BTreeNode *[2*x->order]};
            loadNewNode(x->childs[childsLoaded]);
            childsLoaded++;
        }
        else if(isdigit(keysLoad[0])){
            x->keys[x->numberOfKeys] = std::stoi(keysLoad);
            x->numberOfKeys++;
        }
        else if(keysLoad == ")"){
            return;
        }
    }
};

int searchLevelOfTheKey(BTreeNode *x, int key, int level){
    // We are recursively looking for the given key
    // and with every new search we add 1 to the level
    // at the end we return level
    int i = 0;
    while(i < x->numberOfKeys && key > x->keys[i]){
        i++;
    }

    if(x->keys[i] == key || x->isLeaf){
        return level;
    }

    level++;
    return searchLevelOfTheKey(x->childs[i], key, level);
};

void calculateCache(BTree tree, int cacheSize){
    // Space for our cache
    int *cache = new int[cacheSize];
    // For safety loading up only zeros
    for(int i = 0; i < cacheSize; i++){
        cache[i] = 0;
    }

    int sumNoCache = 0, sumCache = 0, temp;
    // Loading up all of the numbers
    std::string toCache;
    std::getline(std::cin, toCache);
    std::istringstream cacheStream(toCache);
    // While there are numbers in this string
    while(cacheStream >> temp){
        // We add to the sumNoCache level of the given key
        sumNoCache += searchLevelOfTheKey(tree.root, temp, 1);
        // And then we check if this given key is in our cache
        bool isInCache = 0;
        for(int i = 0; i < cacheSize; i++){
            if(cache[i] == temp){
                isInCache = 1;
            }
        }
        // If given key is not in our cache we move all elements one step forward
        if(!isInCache){
            sumCache += searchLevelOfTheKey(tree.root, temp, 1);
            for(int i = cacheSize-1; i > 0; i--){
                cache[i] = cache[i-1];
            }
            // and add given key to the beggining
            cache[0] = temp;
        }
    }
    printf("NO CACHE: %d CACHE: %d\n", sumNoCache, sumCache);
    // Deleting our cache
    delete[] cache;
};

int searchAccessOfTheKey(BTreeNode *x, int key, int access){
    // We are recursively looking for the given key
    // and with every new iteration (iteration means going through every key value in particular node
    // or jumping to another node) we add 1 to the access. At the end we return integer access
    int i = 0;
    access++;
    while(i < x->numberOfKeys && key > x->keys[i]){
        i++;
        access++;
    }

    if(x->keys[i] == key || x->isLeaf){
        return access;
    }

    return searchAccessOfTheKey(x->childs[i], key, access);
};

void calculateCacheLFU(BTree tree, int cacheSize){
    // Space for our nodes in cache
    nodeLFU *cache = new nodeLFU[cacheSize];
    // Cache access is the amount of iterations we need to do to get access to our key value
    // Cache frequency is sum of frequencies of our used elements in cache
    int temp, cacheFrequency = 0, cacheAccess = 0;
    int leastFrequency = 1, leastFrequencyIndex = 0;
    // Loading up all of the numbers
    std::string toCache;
    std::getline(std::cin, toCache);
    std::istringstream cacheStream(toCache);
    // While there are numbers in this string
    while(cacheStream >> temp){
        cacheAccess += searchAccessOfTheKey(tree.root, temp, 0);
        // We need to check if this value is in our cache or not
        bool isInCache = false;

        for(int i = 0; i < cacheSize; i++){
            // If it is we just add +1 to the frequency
            if(cache[i].valueInCache == temp){
                isInCache = true;
                cache[i].frequencyOfUsage++;
                cacheFrequency += cache[i].frequencyOfUsage;
            }
        }
        // We look for least frequence of usage to replace it
        for(int i = 0; i < cacheSize; i++){
            // If we find any undefined spot we save it's index
            if(cache[i].frequencyOfUsage == UNDEFINED){
                leastFrequencyIndex = i;
                break;
            }
            // else we look for lower frequency than saved
            else if(cache[i].frequencyOfUsage < leastFrequency){
                leastFrequency = cache[i].frequencyOfUsage;
                leastFrequencyIndex = i;
            }
        }
        // If its not in our cache we replace least aged value in our cache
        if(!(isInCache)){
            cache[leastFrequencyIndex].valueInCache = temp;
            cache[leastFrequencyIndex].frequencyOfUsage++;
            cacheFrequency++;
        }
    }
    // Print the outcome
    printf("CACHEFREQUENCY : %d CACHEACCESS : %d\n", cacheFrequency, cacheAccess);
    delete[] cache;
};

int getFirstKey(BTreeNode *x){
    return x->keys[0];
};

void destructorOfTree(BTree &tree){
    while(tree.root != NULL){
        removeKey(tree, getFirstKey(tree.root));
    }
};
