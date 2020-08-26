/* 
Ben Foxman - netid btf28
Implementation of the Deque ADT using interface 
defined in Hwk4/Deque.h
*/

#include <stdio.h>
#include <stdlib.h>
#include "/c/cs223/Hwk4/Deque.h"

//head and tail of the Deque will be a linked list
typedef struct node {
    char *value;
    struct node *next;
} Node;


struct deque {
    Node *head; 
    Node *tail;
};

// Implementation of createD()
bool createD (Deque *d)
{
    //malloc fails
    if (!(*d = malloc(sizeof(**d)))){
        return false;
    }

    //malloc head + tail of Deque *d, set to null
    Node *h = malloc(sizeof(Node));
    h -> next = NULL;
    h -> value = NULL;
    Node *t = malloc(sizeof(Node));
    t -> next = NULL;
    t -> value = NULL;
   
    (*d) -> head = h;
    (*d) -> tail = t;
    
    return true;
}

// Implementation of isEmptyD()
bool isEmptyD (Deque *d)
{   
    //empty if and only if head and tail point to null
    return (*d) -> head -> next == NULL && (*d) -> tail -> next == NULL;
}


// Implementation of addD()
bool addD (Deque *d, char *s)
{
    Node *new = NULL; 

    //malloc fails
    if (!(new = malloc(sizeof(*new)))){
        return false;
    }

    //tail points to a new node with the specified value
    new -> value = s;
    new -> next = (*d) -> tail -> next;
    (*d) -> tail -> next = new;

    return true;
}


// Implementation of remD()
bool remD (Deque *d, char **s)
{   
    //no elements in Deque
    if (isEmptyD(d)){
        *s = NULL;
        return false;
    }
    //head is empty, tail is not. push everything onto head.
    else if ((*d) -> head -> next == NULL){
        Node *curr = (*d) -> tail -> next;
        while (curr != NULL){
            pushD(d, curr -> value);
            (*d) -> tail -> next = curr -> next;
            free(curr);
            curr = (*d) -> tail -> next;
        }
    }

    //remove first node of head 
    Node *first = (*d) -> head -> next;
    (*d) -> head -> next = first -> next;
    *s = first -> value;
    free(first);

    return true;
}


// Implementation of headD()
bool headD (Deque *d, char **s)
{
    //no elements in Deque
    if (isEmptyD(d)) {
        *s = NULL;
        return false;
    }
    //head is empty, tail is not. push everything onto head. 
    else if ((*d) -> head -> next == NULL){
        Node *curr = (*d) -> tail -> next;
        while (curr != NULL){
            pushD(d, curr -> value);
            (*d) -> tail -> next = curr -> next;
            free(curr);
            curr = (*d) -> tail -> next;
        }
    }

    //store the value in the string pointer s
    *s = (*d) -> head -> next -> value;
    return true;
}


// Implementation of pushD()
bool pushD (Deque *d, char *s)
{
    Node *new = NULL;
    
    //malloc fails
    if (!(new = malloc(sizeof(*new)))){
        return false;
    }

    //head points to a new node with the specified value
    new -> value = s;
    new -> next = (*d) -> head -> next;
    (*d) -> head -> next = new;

    return true;
}


// Implementation of destroyD()
bool destroyD (Deque *d)
{
    char *temp = NULL;

    //remove all elements in the head and tail
    while(!(isEmptyD(d))){
        remD(d, &temp);
    }

    //free the head and tail
    free((*d) -> head);
    (*d) -> head = NULL;
    free((*d) -> tail);
    (*d) -> tail = NULL;

    //free the Deque
    free(*d);
    *d = NULL;
    
    return true;
}
