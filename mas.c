#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>

#define HEAP_SIZE 2048
#define ROOT_SIZE 8
#define TEXT_SIZE 256
#define MAX_PC 500000

#define PRINT_DIAGNOSTICS_INFO true

#define INSTR_SET_SIZE 3

#define NOP 0
#define OOK 1
#define KOK 2

const int instructionSet[INSTR_SET_SIZE] = { NOP, OOK, KOK };

#define CP_SIZE 3
const char constantPool[CP_SIZE][TEXT_SIZE] = {"ook", "oook", "ook!"};

#define REPORT_AFTER 10

typedef struct DataStructure Data;

struct DataStructure {

    bool marked;

    char text[TEXT_SIZE];
    Data* next;
    Data* prec;
};

Data* heap[HEAP_SIZE];
Data* root[ROOT_SIZE];

int allocatedRecords = 0;

void randInit() {
  time_t t;
  srand((unsigned) time(&t));
}

void reportOutOfMemoryAndExit() {
    fprintf(stderr, "Out of memory, bye!");
    exit(1);
};

void reportPC(int pc) {
  printf("\nPC:\t%d\n", pc);
};

void reportHeapStatus() {
  printf("\nalloc:\t%d\nfree:\t%d\nMAX:\t%d\n", allocatedRecords, HEAP_SIZE-allocatedRecords, HEAP_SIZE);
};

int getTailLength(Data* data) {
    int result = 0;
    while ((data = data->next) != NULL) {
        result++;
    }
    return result;
}

void printTail(Data* data) {
    while ((data = data->next) != NULL) {
        printf("->%s (%s)", data->text, data->prec ? "O" : "I");
    }
}

Data* getTail(Data* root) {
    Data* result = root;
    while (result->next != NULL) {
      result = result->next;
    }
    return result;
}

void reportRoots() {
    for (int i=0; i<ROOT_SIZE; i++) {
        if (root[i]) {
          printf("[%d]: %s (%d)", i, root[i]->text, getTailLength(root[i]));
          printTail(root[i]);
          printf("\n");
        }
    }
};

int findFreeSlotInHeap(){
    for (int i=0; i<HEAP_SIZE; i++) {
        if (heap[i] == 0) {
            return i;
        }
    }
    reportOutOfMemoryAndExit();
};

Data* allocateNewData() {

    // find free slot in heap:
    int heapIndex = findFreeSlotInHeap();

    // claim room for the data structure
    Data* newData = (Data*)malloc(sizeof(Data));
    if (newData == NULL) {
        reportOutOfMemoryAndExit();
    }

    // book the record
    heap[heapIndex] = newData;
    allocatedRecords++;

    // initialize the structure
    newData->marked = false;
    memset(newData->text, 0, TEXT_SIZE);

    return newData;
};

void ook() { // single OOK operation of our program

    // a = new Data();
    Data* data = allocateNewData();

    // a.text = "..."
    strcpy(data->text, (char*)&constantPool[rand() % CP_SIZE]);
    // a.next = null | roots[<random index>]
    data->next = (rand() % 2) ?
              NULL
            : root[rand() % (sizeof(root) / sizeof(Data*))];

    // roots[<random index>] = a
    root[rand() % (sizeof(root) / sizeof(Data*))] = data;
}

void kok() { // single KOK operation of our program

    // a = roots[<random index>]
    Data* data = root[rand() % (sizeof(root) / sizeof(Data*))];

    if (data != NULL) {
      Data* tail = getTail(data);
      tail->prec = root[rand() % (sizeof(root) / sizeof(Data*))];
    }
}

void singleProgramStep() {

  int instruction = instructionSet[rand() % INSTR_SET_SIZE];
  switch (instruction) {
      case NOP: // do nothing
          break;
      case OOK: // new item
          ook();
          break;
      case KOK: // make cycle in 10 % cases
          if ((rand() % 100) > 90) {
              kok();
          }
          break;
  };
}

int main(int argc, char** argv) {

    // init:
    randInit();
    memset(heap, 0, sizeof(heap));
    memset(root, 0, sizeof(root));

    int pc = 0;


    // run
    for(;pc<MAX_PC;) {
      pc++;
      singleProgramStep();
      if ((pc % REPORT_AFTER) == 0) {
        if (PRINT_DIAGNOSTICS_INFO) {
          reportPC(pc);
          reportHeapStatus();
          reportRoots();
        }
      }
    }
    return 0;
};
