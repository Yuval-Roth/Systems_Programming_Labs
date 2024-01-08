
#include <malloc.h>

struct ListNode {
    int data;
    struct ListNode *next;
};
struct IntList{
    struct ListNode *first;
    struct ListNode *last;
    int size;
};
void addLast(struct IntList *list, int data){
    // new node
    struct ListNode *newNode = (struct ListNode*) malloc(sizeof(struct ListNode));
    newNode->data = data;
    newNode->next = 0;

    if(list->last != 0){
        list->last->next = newNode;
    }
    list->last = newNode;
    if(list->first == 0){
        list->first = newNode;
    }
    list->size++;
}
int getFirst(struct IntList *list){
    return list->first->data;
}
void removeFirst(struct IntList *list){
    struct ListNode *temp = list->first;
    list->first = list->first->next;
    free(temp);
    list->size--;
    if(list->size == 0){
        list->last = 0;
    }
}
int pop(struct IntList *list){
    int output = getFirst(list);
    removeFirst(list);
    return output;
}
void destroyList(struct IntList *list){
    while(list->first != 0){
        pop(list);
    }
}
struct IntList newIntList(){
    struct IntList list;
    list.first = 0;
    list.last = 0;
    list.size = 0;
    return list;
}