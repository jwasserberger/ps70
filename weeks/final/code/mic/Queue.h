class Queue
{
    // Initialize front and rear
    int r, f;
    static const int MAX_SIZE = 10000;
    // Circular Queue
    // int size;

    uint8_t *arr  = (uint8_t*)malloc(MAX_SIZE);
    unsigned int currSize;

public:
    Queue();
    void add(uint8_t value);
    uint8_t pop();
    void print();
    bool isFull();
    bool isEmpty();
};
