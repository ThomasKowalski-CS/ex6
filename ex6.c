/****************************
 * Name: Thomas Kowalski
 * ID: *********
 * Assignment: Ex6
****************************/

#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;

    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// case 2: Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// BFS display
void displayBFS(PokemonNode *root) {
    BFSGeneric(root, printPokemonNode);
}

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    Queue queue;
    queue.front = (QueueNode*) malloc(sizeof(QueueNode));
    if (!queue.front) {
        printf("malloc failed\n");
        exit(1);
    }
    queue.rear = queue.front;
    queue.front->next = NULL;
    queue.front->pokemonNode = root;
    
    QueueNode *cur = queue.front;

    while (cur != NULL) {
    visit(cur->pokemonNode);
    if (cur->pokemonNode->left) {
        addChildToQueue(&queue, cur->pokemonNode->left);
    }
    if (cur->pokemonNode->right) {
        addChildToQueue(&queue, cur->pokemonNode->right);
    }
    cur = queue.front->next;
    free(queue.front);
    queue.front = cur;
    }
}

// DFS PreOrder display
void preOrderTraversal(PokemonNode *root) {
    preOrderGeneric(root, printPokemonNode);
}

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) {
        return;
    }
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}

// DFS InOrder display
void inOrderTraversal(PokemonNode *root) {
    inOrderGeneric(root, printPokemonNode);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) {
        return;
    }
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}

// DFS PostOrder display
void postOrderTraversal(PokemonNode *root) {
    postOrderGeneric(root, printPokemonNode);
}

void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) {
        return;
    }
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}

// Alphabetic display
void displayAlphabetical(PokemonNode *root) {
    // check bst length
    int BSTSize = checkBSTLength(root);

    // create and initialize the node array
    NodeArray arr;
    arr.size = 0;
    arr.capacity = BSTSize;
    initNodeArray(&arr, arr.capacity);
    // add all the nodes to the array
    collectAll(root, &arr);
    // sort the array
    bubbleSortArr(&arr);
    // print the array and free it
    for (int i = 0; i < arr.size; i++) {
        printPokemonNode(arr.nodes[i]);
    }
    free(arr.nodes);
}

int checkBSTLength(PokemonNode *root) {
    if (!root) {
        return 0;
    }
    return 1 + checkBSTLength(root->left) + checkBSTLength(root->right);
}

void initNodeArray(NodeArray *na, int cap) {
    na->nodes = (PokemonNode**) malloc(sizeof(PokemonNode*) * cap);
    if (!na->nodes) {
        printf("malloc failed\n");
        exit(1);
    }
}

void addNode(NodeArray *na, PokemonNode *node) {
    if (na->size == na->capacity) {
        na->capacity++;
        na->nodes = (PokemonNode**) realloc(na->nodes, sizeof(PokemonNode*) * na->capacity);
        if(!na->nodes) {
            printf("realloc failed\n");
            exit(1);
        }
    }
    na->nodes[na->size] = node;
    na->size++;
}

void collectAll(PokemonNode *root, NodeArray *na) {
    if (!root) {
        return;
    }
    addNode(na, root);
    collectAll(root->left, na);
    collectAll(root->right, na);
}

void bubbleSortArr(NodeArray *na) {
    int endIndex = na->capacity-1;
    PokemonNode *tmp;
    while (endIndex > 0) {
        for(int i = 0; i < endIndex; i++) {
            if (strcmp(na->nodes[i]->data->name, na->nodes[i+1]->data->name) > 0) {
                // swap
                tmp = na->nodes[i];
                na->nodes[i] = na->nodes[i+1];
                na->nodes[i+1] = tmp;
            }
        }
        endIndex--;
    }
    tmp = NULL;

}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    if (!ownerHead) {
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners
    printf("\nExisting Pokedexes:\n");
    // you need to implement a few things here :)
    OwnerNode *tmpOwner = ownerHead;
    int i = 1; // counter for print
    do {
        printf("%d. %s\n", i, tmpOwner->ownerName);
        i++;
        tmpOwner = tmpOwner->next;
    } while (tmpOwner != ownerHead);

    int chosenOwnerId = readIntSafe("Choose a Pokedex by number: ");
    OwnerNode *cur = ownerHead; 
    for (i = 1; i < chosenOwnerId; i++) { 
        cur = cur->next;
    }

    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(cur);
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            freePokemon(cur, 0);
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// ---------------------------- 
// case 1: Add Pokemon
// ----------------------------
void addPokemon(OwnerNode *owner) {
    int newPokemonId = readIntSafe("Enter ID to add: ");
    // check if tree exists (and create the root if not)
    if (!owner->pokedexRoot) {
        owner->pokedexRoot = (PokemonNode*) malloc(sizeof(PokemonNode));
        if (!owner->pokedexRoot) {
            printf("malloc failed\n");
            exit(1);
        }
        owner->pokedexRoot->data = NULL;
        owner->pokedexRoot->left = NULL;
        owner->pokedexRoot->right = NULL;
        owner->pokedexRoot->parent = NULL;
    }
    
    
    // check if id already in tree
    if (owner->pokedexRoot->data != NULL && searchPokemonBFS(owner->pokedexRoot, newPokemonId) != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", newPokemonId);
        return;
    }

    // add pokemon
    insertPokemonNode(owner, newPokemonId);

    // when done
    printf("Pokemon %s (ID %d) added.\n", pokedex[newPokemonId-1].name, newPokemonId);
    return;
}

PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    if (!root) { // if tree is empty
        return NULL;
    }
    
    if (root->data->id == id) { // 
        return root;
    }

    Queue queue;

    queue.front = (QueueNode*) malloc(sizeof(QueueNode));
    if (!queue.front) {
        printf("malloc failed\n");
        exit(1);
    }
    queue.rear = queue.front;
    queue.front->pokemonNode = root;
    queue.front->next = NULL;
    

    while (queue.front != NULL) {
    // work queue
    QueueNode *currentQueueNode = queue.front;
    if (currentQueueNode->pokemonNode->data->id == id) {
        PokemonNode *found = currentQueueNode->pokemonNode;
        // clear queue
        while (queue.front != NULL) {
            QueueNode *tmp = queue.front->next;
            free(queue.front);
            queue.front = tmp; 
            tmp = NULL;
        }
        return found;
    } 
    // else add children to queue
    if (currentQueueNode->pokemonNode->left) {
        addChildToQueue(&queue, currentQueueNode->pokemonNode->left);
    }

    if (currentQueueNode->pokemonNode->right) {
        addChildToQueue(&queue, currentQueueNode->pokemonNode->right);
    }

    // next one in queue
    QueueNode *tmp = queue.front->next;
    free(queue.front);
    queue.front = tmp; 
    tmp = NULL;

    }
    return NULL;
}

void addChildToQueue(Queue* queue, PokemonNode *child) {
    QueueNode *newQueueNode = malloc(sizeof(QueueNode));
    if (!newQueueNode) {
        printf("malloc failed\n");
        exit(1);
    }

    newQueueNode->pokemonNode = child;
    newQueueNode->next = NULL;
    queue->rear->next = newQueueNode;
    queue->rear = newQueueNode;
    newQueueNode = NULL;
}

void insertPokemonNode(OwnerNode *owner, int id) {
    if (owner->pokedexRoot == NULL) { //handles an empty tree
        owner->pokedexRoot = (PokemonNode*) malloc(sizeof(PokemonNode));
        if (!owner->pokedexRoot) {
            printf("malloc failed\n");
            exit(1);
        }
        owner->pokedexRoot->left = NULL;
        owner->pokedexRoot->right = NULL;
        owner->pokedexRoot->data = NULL;
        owner->pokedexRoot->parent = NULL;

        dupPokemonData(id, owner->pokedexRoot);
        return;
    }
    PokemonNode *cycler = owner->pokedexRoot;

    while (1) {
        if (!cycler->data) { // if tree is empty, add at root
            dupPokemonData(id, cycler);
            return;
        }
        if (cycler->data->id < id) {
            // try right
            if (cycler->right) {
                cycler = cycler->right;
                continue;
            } 
            // if cant, add child there
            cycler->right = malloc(sizeof(PokemonNode));
            if (!cycler->right) {
                printf("malloc failed\n");
                exit(1);
            }
            cycler->right->left = NULL;
            cycler->right->right = NULL;
            cycler->right->data = NULL;
            cycler->right->parent = cycler;
            dupPokemonData(id, cycler->right);
            return;
        }
        else {
            // try left
            if (cycler->left) {
                cycler = cycler->left;
                continue;
            } 
            // if cant add child there
            cycler->left = malloc(sizeof(PokemonNode));
            if (!cycler->left) {
                printf("malloc failed\n");
                exit(1);
            }
            cycler->left->left = NULL;
            cycler->left->right = NULL;
            cycler->left->data = NULL;
            cycler->left->parent = cycler;
            dupPokemonData(id, cycler->left);
            return;
        }
    }
}


// ---------------------------- 
// case 3: Free Pokemon
// ----------------------------
void freePokemon(OwnerNode *owner, int id) {
    if (!owner->pokedexRoot) {
        printf("No Pokemon to release.\n");
        return;
    }
    int chosenPokemonId = id;
    if (chosenPokemonId == 0) {
        chosenPokemonId = readIntSafe("Enter Pokemon ID to release: ");
    }
    // find it
    PokemonNode *target = searchPokemonBFS(owner->pokedexRoot, chosenPokemonId);
    if (!target) { // if not exist
        printf("No Pokemon with ID %d found.\n", chosenPokemonId);
        return;
    }
    // else 
    // if no children
    if (target->left == NULL && target->right == NULL) {
        noChildrenRemove(owner, target); 
        target = NULL;
    }
    else if (target->left != NULL && target->right != NULL) { // if have 2 children
        twoChildrenRemove(owner, target);
        target = NULL;
    }
    else { // one child
        oneChildRemove(owner, target);
        target = NULL;
    }
    // print according to caller (id != 0 when called from evolution)
    printf("Removing Pokemon %s (ID %d).\n", pokedex[chosenPokemonId-1].name, chosenPokemonId);

}

PokemonNode* findClosestId(PokemonNode *start) {
    if (start->left == NULL) {
        return start;
    }
    return findClosestId(start->left);
}

void freePokemonNode(PokemonNode *node) {
    free(node->data->name);
    free(node->data);
    free(node);
}

void noChildrenRemove(OwnerNode *owner, PokemonNode *target) {
    if (target->parent == NULL) { // if its the root
        owner->pokedexRoot = NULL;
    }
    else { // else point parent to NULL
        if (target->parent->left == target) {
            target->parent->left = NULL;
        }
        else {
            target->parent->right = NULL;
        }
    }
    freePokemonNode(target);
}

void oneChildRemove(OwnerNode *owner, PokemonNode *target) {
    PokemonNode *child = (target->left != NULL) ? target->left : target->right;
    if (target->parent == NULL) { // if it's the root
        owner->pokedexRoot = child;
        child->parent = NULL;
    }
    else { // point child and parent
        if (target->parent->left == target) {
            target->parent->left = child;
            child->parent = target->parent;
        }
        else {
            target->parent->right = child;
            child->parent = target->parent;
        }
    }
    freePokemonNode(target);
}

void twoChildrenRemove(OwnerNode *owner, PokemonNode *target) {
    // find replacement
    PokemonNode *replacement = findClosestId(target->right);
    // switch data
    PokemonData *tmp = target->data;
    target->data = replacement->data;
    replacement->data = tmp;
    tmp = NULL;
    
    // replacement is childless or has one child by definition
    if (replacement->left == NULL && replacement->right ==NULL) {
        noChildrenRemove(owner, replacement);
        replacement = NULL;
    }
    else {
        oneChildRemove(owner, replacement);
        replacement = NULL;
    }
}

// ---------------------------- 
// case 4: Pokemon Fight
// ----------------------------

void pokemonFight(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("Pokedex is empty.\n");
        return;
    }

    int id1 = readIntSafe("Enter ID of the first Pokemon: ");
    int id2 = readIntSafe("Enter ID of the second Pokemon: ");
    if (searchPokemonBFS(owner->pokedexRoot, id1) && searchPokemonBFS(owner->pokedexRoot, id2)) { // both exists
        float score1 = (pokedex[id1-1].attack * 1.5 + pokedex[id1-1].hp * 1.2);
        float score2 = (pokedex[id2-1].attack * 1.5 + pokedex[id2-1].hp * 1.2);

        printf("Pokemon 1: %s (Score = %.2f)\n", pokedex[id1-1].name, score1);
        printf("Pokemon 2: %s (Score = %.2f)\n", pokedex[id2-1].name, score2);

        if (score1 < score2) {
            printf("%s wins!\n", pokedex[id2-1].name);
        }
        else if (score2 < score1) {
            printf("%s wins!\n", pokedex[id1-1].name);
        }
        else {
            printf("It’s a tie!\n");
        }
    }
    else {
        printf("One or both Pokemon IDs not found.\n");
    }
}

// ---------------------------- 
// case 5: Evolve Pokemon
// ----------------------------

void evolvePokemon(OwnerNode *owner) {
    // check if pokedex is empty
    if (!owner->pokedexRoot) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    int oldID = readIntSafe("Enter ID of Pokemon to evolve: ");
    // check if id in pokedex
    
    if (!searchPokemonBFS(owner->pokedexRoot, oldID)) {
        printf("No Pokemon with ID %d found.\n", oldID);
        return;
    }
    // check if has an evolution
    if (!pokedex[oldID-1].CAN_EVOLVE) { // if not
        printf("%s (ID %d) cannot evolve.\n", pokedex[oldID-1].name, oldID);
        return;
    }

    // check if evolved version exists
    int evolvedID = oldID+1;
    if (searchPokemonBFS(owner->pokedexRoot, evolvedID)) { // if it does
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n", evolvedID,
        pokedex[evolvedID-1].name, pokedex[oldID-1].name, oldID);
        freePokemon(owner, oldID);
    }
    else { // if it doesn't
        // free old Pokemon
        freePokemon(owner, oldID);
        // add evolved version
        insertPokemonNode(owner, evolvedID);
    }
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", pokedex[oldID-1].name,
    oldID, pokedex[evolvedID-1].name, evolvedID);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}

void freeAllOwners(void) {
    if (ownerHead == NULL) {
        return;
    }

    while (ownerHead != NULL) {
        OwnerNode *cur = ownerHead->next;
        if (cur == ownerHead) { // is the last one
        ownerHead = NULL;
        }
        else {
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        }
        freeOwnerNode(cur);
        cur = NULL;
    }
}



void openPokedexMenu(void) {
    printf("Your name: ");
    char *name = getDynamicInput();
    // check for duplicates
    if (ownerHead && isNameTaken(name)) { // list isn't empty and name is taken
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", name);
        free(name);
        return;
    }
    
    printf("Choose Starter:\n1. %s\n2. %s\n3. %s\n", pokedex[STARTER1-1].name, // STARTERS are changable at .h file :)
    pokedex[STARTER2-1].name, pokedex[STARTER3-1].name);
    int starterChoiceId = readIntSafe("Your choice: ");
    switch (starterChoiceId)
    {
    case 1:
        starterChoiceId = STARTER1;
        break;
    
    case 2:
        starterChoiceId = STARTER2;
        break;
    
    case 3:
        starterChoiceId = STARTER3;
        break;

    default:
        printf("Invalid choice.\n");
        free(name);
        return;
        break;
    } 

    // create pokedex
    if (ownerHead == NULL) { // if list is empty
        ownerHead = (OwnerNode*) malloc(sizeof(OwnerNode));
        if (!ownerHead) {
            printf("malloc failed\n");
            exit(1);
        }
        ownerHead->next = ownerHead;
        ownerHead->prev = ownerHead;
    }
    else {
        // make owner node at end
        OwnerNode *tmp = (OwnerNode*) malloc(sizeof(OwnerNode));
        if (!tmp) {
            printf("malloc failed\n");
            exit(1);
        }
        tmp->next = ownerHead;
        tmp->prev = ownerHead->prev;
        ownerHead->prev->next = tmp;
        ownerHead->prev = tmp;
        tmp = NULL;
    }

    OwnerNode *newOwner = ownerHead->prev;
    newOwner->ownerName = myStrdup(name);
    
    // create the root pokemon node
    newOwner->pokedexRoot = (PokemonNode*) malloc(sizeof(PokemonNode));
    if(!newOwner->pokedexRoot) {
        printf("malloc failed\n");
        exit(1);
    }
    newOwner->pokedexRoot->left = NULL;
    newOwner->pokedexRoot->right = NULL;
    newOwner->pokedexRoot->parent = NULL;

    // create the root's data node
    newOwner->pokedexRoot->data = (PokemonData*) malloc(sizeof(PokemonData));
    if (!newOwner->pokedexRoot->data) {
        printf("malloc failed\n");
        exit(1);
    }
    dupPokemonData(starterChoiceId, newOwner->pokedexRoot);


    // when done
    printf("New Pokedex created for %s with starter %s.\n", name, pokedex[starterChoiceId-1].name);
    free(name);
    return;
}

void dupPokemonData(int pokemonId, PokemonNode *targetNode) {
    if (!targetNode || !targetNode->data) {
        targetNode->data = (PokemonData*) malloc(sizeof(PokemonData));
        if (!targetNode->data) {
            printf("malloc failed\n");
            exit(1);
        }
    }
    // set all data
    targetNode->data->id = pokemonId;
    targetNode->data->name = myStrdup(pokedex[pokemonId-1].name);
    targetNode->data->TYPE = pokedex[pokemonId-1].TYPE;
    targetNode->data->hp = pokedex[pokemonId-1].hp;
    targetNode->data->attack = pokedex[pokemonId-1].attack;
    targetNode->data->CAN_EVOLVE = pokedex[pokemonId-1].CAN_EVOLVE;
}

int isNameTaken(char *name) {
    if (ownerHead == NULL) {
        return 0;
    }

    OwnerNode *currentOwner = ownerHead;
    do {
        if (strcmp(name, currentOwner->ownerName) == 0) {
            return 1;
        }
        currentOwner = currentOwner->next; 

    } while (currentOwner != ownerHead);

    return 0;
}


// ---------------------------- 
// Delete Pokedex
// ---------------------------- 
void deletePokedex(void) {
    if (ownerHead == NULL) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    printf("\n=== Delete a Pokedex ===\n");
    printOwnersOnce('F', menuPrint);
    int chosenPokedexID = readIntSafe("Choose a Pokedex to delete by number: ");
    OwnerNode *target = ownerHead;
    for (int i = 1; i < chosenPokedexID; i++) { // gets the owner node by index
        target = target->next;
    }
    printf("Deleting %s's entire Pokedex...\n", target->ownerName);

    // check if the only Owner
    if (target->next == target) {
        ownerHead = NULL;
    }
    else {
        if (ownerHead == target) { // if is the first Owner
            ownerHead = target->next;
        }
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }
    freeOwnerNode(target);
    target = NULL;
    printf("Pokedex deleted.\n");
}

void freeOwnerNode(OwnerNode *target) {
    postOrderGeneric(target->pokedexRoot, freePokemonNode);
    free(target->ownerName);
    free(target);
}


// ----------------------------
// Print Owners
// ----------------------------
void printOwnersCircular(void) {
    if (!ownerHead) {
        printf("No owners.\n");
        return;
    }
    printf("Enter direction (F or B): ");
    char *dir = getDynamicInput();
    int amount = readIntSafe("How many prints? ");
    if (dir[0] == 'f' || dir[0] == 'b') {
        dir[0] = dir[0] - ASCII_GAP; // to UPPER
    } 

    int i = 1;
    OwnerNode *cur = (dir[0] == 'F') ? ownerHead->next : ownerHead->prev;
    
    printf("[%d] %s\n", i, ownerHead->ownerName);
    amount--;
    while (amount > 0) { 
        i++;
        printf("[%d] %s\n", i, cur->ownerName);
        cur = (dir[0] == 'F') ? cur->next : cur->prev;
        amount--;
    }
    cur = NULL;
    free(dir);
}

// thought it will be useful, but ended up using it only for one case, still was a good practice of generic functions
void printOwnersOnce(char dir, PrintOwnerFunc customPrint) { 
    OwnerNode *cur = (dir == 'F') ? ownerHead->next : ownerHead->prev;
    
    int i = 1;
    customPrint(ownerHead, i);
    while (cur != ownerHead) {
        i++;
        customPrint(cur, i);
        cur = (dir == 'F') ? cur->next : cur->prev;
    }
    cur = NULL;
}

void menuPrint(OwnerNode *Node, int i) {
    printf("%d. %s\n", i, Node->ownerName);
}

// ----------------------------
// Merging Pokedexes
// ----------------------------
void mergePokedexMenu(void) {
    // check if there are atleast 2 pokedexes
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }

    printf("\n=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char *Owner1Name = getDynamicInput();

    printf("Enter name of second owner: ");
    char *Owner2Name = getDynamicInput();


    // get first and second owners node
    OwnerNode *Owner1Node = ownerHead, *Owner2Node = ownerHead; // init value ownerhead
    while (strcmp(Owner1Node->ownerName, Owner1Name)) {
        Owner1Node = Owner1Node->next;
    }

    while (strcmp(Owner2Node->ownerName, Owner2Name)) {
        Owner2Node = Owner2Node->next;
    }

    printf("Merging %s and %s...\n", Owner1Name, Owner2Name);
    Queue queue;
    queue.front = (QueueNode*) malloc(sizeof(QueueNode));
    if (!queue.front) {
        printf("malloc failed\n");
        exit(1);
    }
    queue.rear = queue.front;
    queue.front->pokemonNode = Owner2Node->pokedexRoot;
    queue.front->next = NULL;

    QueueNode *cur = queue.front;

    while (cur != NULL) {
        if (!searchPokemonBFS(Owner1Node->pokedexRoot, cur->pokemonNode->data->id)) { // if pokemon not in pokedex
            insertPokemonNode(Owner1Node, cur->pokemonNode->data->id); // add it
        }
        if (cur->pokemonNode->left) {
            addChildToQueue(&queue, cur->pokemonNode->left);
        }
        if (cur->pokemonNode->right) {
            addChildToQueue(&queue, cur->pokemonNode->right);
        }
        cur = queue.front->next;
        free(queue.front);
        queue.front = cur;
    }

    printf("Merge completed.\n");

    // remove second owner
    if (ownerHead == Owner2Node) { // if second owner is the OwnerHead then point at the next owner
        ownerHead = Owner2Node->next;
    }
    Owner2Node->prev->next = Owner2Node->next;
    Owner2Node ->next->prev = Owner2Node->prev;
    freeOwnerNode(Owner2Node);
    Owner2Node = NULL;
    printf("Owner '%s' has been removed after merging.\n", Owner2Name);

    //free the names
    free(Owner1Name);
    free(Owner2Name);
}

// ----------------------------
// Sort Owners
// ----------------------------
void sortOwners(void) {
    if (!ownerHead) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }

    if (ownerHead->next == ownerHead) {
        printf("Owners sorted by name.\n");
        return;
    }

    // count num of owners
    int ownersCount = 0;
    OwnerNode *cycle = ownerHead;
    do {
        ownersCount++;
        cycle = cycle->next;
    } while (cycle != ownerHead);
    // bubble sort the list
    while (ownersCount > 0) {
        int didSwap = 0;
        cycle = ownerHead;
        for (int i = 0; i < ownersCount; i++) {
            if (strcmp(cycle->ownerName,cycle->next->ownerName) > 0) {
                swapOwnerData(cycle, cycle->next);
                didSwap = 1;
            }
            cycle = cycle->next;
        }
        if (didSwap == 0) {
            break;
        }
        ownersCount--;
    }

    printf("Owners sorted by name.\n");
    return;
}

void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    OwnerNode tmp;
    tmp.ownerName = b->ownerName;
    tmp.pokedexRoot = b->pokedexRoot;
    
    b->ownerName = a->ownerName;
    b->pokedexRoot = a->pokedexRoot;

    a->ownerName = tmp.ownerName;
    a->pokedexRoot = tmp.pokedexRoot;
}