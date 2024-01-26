#include <malloc.h>

typedef struct ListNode ListNode;
struct ListNode {
    int data;
    ListNode *next;
} ;

typedef struct {
    ListNode *first;
    ListNode *last;
    int size;
} IntList;

IntList *newIntList(){
    IntList *list = (IntList*) malloc(sizeof(IntList));
    list->first = 0;
    list->last = 0;
    list->size = 0;
    return list;
}

ListNode *newListNode(int data) {
    ListNode *newNode = (ListNode*) malloc(sizeof(ListNode));
    newNode->data = data;
    newNode->next = 0;
    return newNode;
}
void addLast(IntList *list, int data){
    // new node
    ListNode *newNode = newListNode(data);

    if(list->last != 0){
        list->last->next = newNode;
    }
    list->last = newNode;
    if(list->first == 0){
        list->first = newNode;
    }
    list->size++;
}
int getFirst(IntList *list){
    return list->first->data;
}
void removeFirst(IntList *list){
    ListNode *temp = list->first;
    list->first = list->first->next;
    free(temp);
    list->size--;
    if(list->size == 0){
        list->last = 0;
    }
}
int pop(IntList *list){
    int output = getFirst(list);
    removeFirst(list);
    return output;
}
void destroyList(IntList *list){
    while(list->size != 0){
        pop(list);
    }
    free(list);
}