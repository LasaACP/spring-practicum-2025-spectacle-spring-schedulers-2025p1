struct PQNode
{
    int prio;
    void *data;
    struct PQNode *next;
};

class PriorityQueue
{

private:
    PQNode *first;

public:
    PriorityQueue()
    {
        first = nullptr;
    }

    void push(void *item, int priority);
    void *top();
    void pop();
    void display();
};