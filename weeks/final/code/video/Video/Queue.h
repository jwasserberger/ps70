class Queue
{
    // Initialize front and rear
    int r, f;
    static const int MAX_SIZE = 16000;
    // Circular Queue
    // int size;

    int8_t *arr  = (int8_t*)malloc(MAX_SIZE);
    int8_t *delayArr  = (int8_t*)malloc(MAX_SIZE);
    unsigned int currSize;

public:
    Queue();
    void add(int8_t value);
    uint8_t pop();
    void delay(int8_t currSound);
    void print();
    bool isFull();
    bool isEmpty();
};
