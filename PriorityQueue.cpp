#include "PriorityQueue.h"
#include <iostream>
using namespace std;

/// @brief adds a new item to the queue
/// @param item the data to be pushed
/// @param priority the priority of the item; lower values are sorted first
void PriorityQueue::push(void *item, int priority)
{

    // init next node
    PQNode *toInsert = new PQNode();
    toInsert->data = item;
    toInsert->prio = priority;

    // init temp vars
    PQNode *toLoop = first;
    PQNode *previous = nullptr;

    // loop thru everything
    while (toLoop != nullptr)
    {
        if (toLoop->prio > priority)
        {
            break;
        }
        previous = toLoop;
        toLoop = toLoop->next;
    }

    // insert
    toInsert->next = toLoop;
    if (previous != nullptr)
    {
        previous->next = toInsert;
    }
    else
    {
        first = toInsert;
    }
}

/// @return the first node's data
void *PriorityQueue::top()
{
    if (first == nullptr)
        return nullptr;
    return first->data;
}

/// @brief removes the top node from the queue
void PriorityQueue::pop()
{
    if (first == nullptr)
        return;
    PQNode *old = first;
    first = first->next;
    delete old;
}

void PriorityQueue::display()
{
    PQNode *ptr = first;

    while (ptr != nullptr)
    {
        cout << ptr->prio << " " << (char *)ptr->data << endl;
        ptr = ptr->next;
    }
}