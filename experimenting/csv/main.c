#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/* --- Trie for Succinct Dictionary --- */
typedef struct TrieNode {
    char letter;
    int isEnd;
    int index; // Assigned unique index if terminal.
    struct TrieNode** children;
    int childCount;
    int childrenCapacity;
} TrieNode;

TrieNode* createTrieNode(char letter) {
    TrieNode* node = malloc(sizeof(TrieNode));
    node->letter = letter;
    node->isEnd = 0;
    node->index = -1;
    node->childCount = 0;
    node->childrenCapacity = 2;
    node->children = malloc(node->childrenCapacity * sizeof(TrieNode*));
    return node;
}

// Insert 'word' into trie. 'originalWord' is stored in wordList if new.
int trieInsert(TrieNode* root, const char* word, const char* originalWord,
               char*** wordList, int* wordCount, int* wordListCapacity) {
    TrieNode* current = root;
    const char* ptr = word;
    while(*ptr) {
        int found = 0;
        TrieNode* child = NULL;
        for (int i = 0; i < current->childCount; i++) {
            if (current->children[i]->letter == *ptr) {
                child = current->children[i];
                found = 1;
                break;
            }
        }
        if (!found) {
            if (current->childCount == current->childrenCapacity) {
                current->childrenCapacity *= 2;
                current->children = realloc(current->children, current->childrenCapacity * sizeof(TrieNode*));
            }
            child = createTrieNode(*ptr);
            current->children[current->childCount++] = child;
        }
        current = child;
        ptr++;
    }
    if (!current->isEnd) {
        current->isEnd = 1;
        current->index = (*wordCount);
        if (*wordCount == *wordListCapacity) {
            *wordListCapacity *= 2;
            *wordList = realloc(*wordList, (*wordListCapacity) * sizeof(char*));
        }
        (*wordList)[*wordCount] = strdup(originalWord);
        (*wordCount)++;
    }
    return current->index;
}

int trieSearch(TrieNode* root, const char* word) {
    TrieNode* current = root;
    while(*word) {
        int found = 0;
        for (int i = 0; i < current->childCount; i++) {
            if (current->children[i]->letter == *word) {
                current = current->children[i];
                found = 1;
                break;
            }
        }
        if (!found)
            return -1;
        word++;
    }
    return (current->isEnd ? current->index : -1);
}

void freeTrie(TrieNode* root) {
    for (int i = 0; i < root->childCount; i++)
        freeTrie(root->children[i]);
    free(root->children);
    free(root);
}

/* --- LOUDS and (stub) RRR Compression --- */
typedef struct {
    uint8_t* bits;  // LOUDS bit vector (each element is 0 or 1)
    size_t bitCount;
    char* labels;   // Child labels in LOUDS order
    size_t labelCount;
} LOUDS;

typedef struct Queue {
    TrieNode** data;
    int front;
    int rear;
    int capacity;
} Queue;

Queue* createQueue(int capacity) {
    Queue* q = malloc(sizeof(Queue));
    q->data = malloc(capacity * sizeof(TrieNode*));
    q->front = 0;
    q->rear = 0;
    q->capacity = capacity;
    return q;
}

void enqueue(Queue* q, TrieNode* node) {
    if(q->rear == q->capacity) {
        q->capacity *= 2;
        q->data = realloc(q->data, q->capacity * sizeof(TrieNode*));
    }
    q->data[q->rear++] = node;
}

TrieNode* dequeue(Queue* q) {
    if(q->front == q->rear)
        return NULL;
    return q->data[q->front++];
}

int isQueueEmpty(Queue* q) {
    return q->front == q->rear;
}

void freeQueue(Queue* q) {
    free(q->data);
    free(q);
}

// Build a LOUDS representation from the trie.
LOUDS buildLOUDS(TrieNode* root) {
    LOUDS l;
    l.bits = malloc(128 * sizeof(uint8_t));
    l.bitCount = 0;
    l.labels = malloc(128 * sizeof(char));
    l.labelCount = 0;
    size_t bitsCapacity = 128, labelsCapacity = 128;

    Queue* q = createQueue(64);
    enqueue(q, root);
    while(!isQueueEmpty(q)) {
        TrieNode* node = dequeue(q);
        for (int i = 0; i < node->childCount; i++) {
            if(l.bitCount == bitsCapacity) {
                bitsCapacity *= 2;
                l.bits = realloc(l.bits, bitsCapacity * sizeof(uint8_t));
            }
            l.bits[l.bitCount++] = 1;
            if(l.labelCount == labelsCapacity) {
                labelsCapacity *= 2;
                l.labels = realloc(l.labels, labelsCapacity * sizeof(char));
            }
            l.labels[l.labelCount++] = node->children[i]->letter;
            enqueue(q, node->children[i]);
        }
        if(l.bitCount == bitsCapacity) {
            bitsCapacity *= 2;
            l.bits = realloc(l.bits, bitsCapacity * sizeof(uint8_t));
        }
        l.bits[l.bitCount++] = 0;
    }
    freeQueue(q);
    return l;
}

// Stub for RRR compression (returns the same LOUDS in this demo).
LOUDS RRRCompress(LOUDS l) {
    // In a full implementation, compress l.bits and build rank/select structures.
    return l;
}

void printLOUDS(const LOUDS* l) {
    printf("LOUDS bits: ");
    for (size_t i = 0; i < l->bitCount; i++)
        printf("%d", l->bits[i]);
    printf("\nLabels: ");
    for (size_t i = 0; i < l->labelCount; i++)
        printf("%c ", l->labels[i]);
    printf("\n");
}

/* --- Dictionary using the Succinct Trie --- */
typedef struct dict {
    TrieNode* trieRoot;
    char** wordList;      // Maps perfect hash index -> word.
    int wordCount;
    int wordListCapacity;
    uint16_t* indexTable; // Flattened 2D array: row * colCount + col.
    size_t colCount;
    size_t rowCount;
    LOUDS louds;        // LOUDS representation of the trie (RRR-compressed).
} dict;

dict* dictInit(const char* csv) {
    // Count rows and columns
    char* temp = strdup(csv);
    size_t rowCount = 0, colCount = 0;
    char* line = strtok(temp, "\n");
    if(line) {
        colCount = 1;
        for (char* c = line; *c; c++)
            if(*c == ',') colCount++;
        rowCount++;
        while((line = strtok(NULL, "\n")) != NULL)
            rowCount++;
    }
    free(temp);

    dict* d = malloc(sizeof(dict));
    d->colCount = colCount;
    d->rowCount = rowCount;
    d->indexTable = malloc(rowCount * colCount * sizeof(uint16_t));
    d->wordListCapacity = 16;
    d->wordCount = 0;
    d->wordList = malloc(d->wordListCapacity * sizeof(char*));

    d->trieRoot = createTrieNode('\0'); // Root is a dummy node.

    // Parse CSV and fill trie and indexTable.
    char* csv_dup = strdup(csv);
    size_t current_row = 0;
    line = strtok(csv_dup, "\n");
    while(line != NULL) {
        size_t col = 0;
        char* token;
        char* saveptr;
        token = strtok_r(line, ",", &saveptr);
        while(token != NULL && col < colCount) {
            int idx = trieInsert(d->trieRoot, token, token, &d->wordList,
                                 &d->wordCount, &d->wordListCapacity);
            d->indexTable[current_row * colCount + col] = idx;
            col++;
            token = strtok_r(NULL, ",", &saveptr);
        }
        current_row++;
        line = strtok(NULL, "\n");
    }
    free(csv_dup);

    // Build LOUDS representation from the trie and compress using RRR.
    LOUDS l = buildLOUDS(d->trieRoot);
    d->louds = RRRCompress(l);
    return d;
}

char* dictQuery(dict* d, int row, int col) {
    if(!d || row < 0 || (size_t)row >= d->rowCount || col < 0 || (size_t)col >= d->colCount)
        return NULL;
    uint16_t idx = d->indexTable[row * d->colCount + col];
    if(idx < 0 || idx >= d->wordCount)
        return NULL;
    return d->wordList[idx];
}

void dictPrint(const dict* d) {
    if(!d) return;
    for (size_t row = 0; row < d->rowCount; row++) {
        for (size_t col = 0; col < d->colCount; col++) {
            printf("%s", dictQuery((dict*)d, row, col));
            if(col < d->colCount - 1)
                printf(", ");
        }
        printf("\n");
    }
}

void dictPrintIndexes(const dict* d) {
    if(!d) return;
    for (size_t row = 0; row < d->rowCount; row++) {
        for (size_t col = 0; col < d->colCount; col++) {
            printf("%u", d->indexTable[row * d->colCount + col]);
            if(col < d->colCount - 1)
                printf(", ");
        }
        printf("\n");
    }
}

/* --- Main with Profiling --- */
int main(void) {
    const char *csv_data =
    "dayOfTheWeek,wokeBeforeNoon,hasEaten,mealPlanned\n"
    "Sunday,no,no,Bread\n"
    "Sunday,no,no,Tofu\n"
    "Monday,no,no,Yams\n"
    "Tuesday,no,yes,Steak\n"
    "Thursday,yes,yes,Yams\n"
    "Saturday,yes,yes,Tofu\n"
    "Monday,yes,yes,Eggs\n"
    "Monday,yes,yes,Eggs\n"
    "Tuesday,no,yes,Bread\n"
    "Wednesday,no,yes,Banana\n"
    "Thursday,yes,yes,Eggs\n"
    "Friday,no,no,Yams\n"
    "Friday,yes,yes,Tofu\n"
    "Saturday,no,yes,Tofu\n"
    "Monday,no,no,Banana\n"
    "Friday,no,yes,Yams\n"
    "Saturday,yes,yes,Steak\n"
    "Tuesday,yes,yes,Eggs\n"
    "Sunday,yes,yes,Banana\n"
    "Saturday,yes,yes,Steak\n"
    "Thursday,no,no,Steak\n"
    "Friday,yes,no,Yams\n"
    "Monday,yes,no,Banana\n"
    "Tuesday,yes,yes,Waffle\n"
    "Sunday,no,no,Bread\n"
    "Wednesday,yes,no,Bread\n"
    "Thursday,no,yes,Banana\n"
    "Saturday,yes,no,Yams\n"
    "Tuesday,yes,no,Waffle\n"
    "Saturday,no,no,Eggs\n"
    "Saturday,yes,yes,Steak\n"
    "Sunday,yes,yes,Bread\n";

    dict *d = dictInit(csv_data);
    if(!d) {
        fprintf(stderr, "Failed to initialize dictionary\n");
        return 1;
    }

    // Profile memory footprint of the compressed CSV.
    size_t total_mem = sizeof(dict);
    total_mem += d->wordListCapacity * sizeof(char*);
    for (int i = 0; i < d->wordCount; i++)
        total_mem += strlen(d->wordList[i]) + 1;
    total_mem += d->rowCount * d->colCount * sizeof(uint16_t);
    total_mem += d->louds.bitCount * sizeof(uint8_t);
    total_mem += d->louds.labelCount * sizeof(char);
    printf("Memory footprint of compressed CSV: %zu bytes\n", total_mem);

    // Profile query time over 1,000,000 queries.
    clock_t start = clock();
    volatile char* result;
    for (int i = 0; i < 1000000; i++) {
        int row = i % d->rowCount;
        int col = i % d->colCount;
        result = dictQuery(d, row, col);
    }
    clock_t end = clock();
    double query_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for querying: %f seconds\n", query_time);

    // Example prints.
    printf("\nCSV (reconstructed):\n");
    dictPrint(d);
    printf("\nIndex Table:\n");
    dictPrintIndexes(d);
    printf("\nLOUDS representation:\n");
    printLOUDS(&d->louds);

    // Cleanup.
    for (int i = 0; i < d->wordCount; i++)
        free(d->wordList[i]);
    free(d->wordList);
    free(d->indexTable);
    free(d->louds.bits);
    free(d->louds.labels);
    freeTrie(d->trieRoot);
    free(d);
    return 0;
}

