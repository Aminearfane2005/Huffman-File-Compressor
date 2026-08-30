#ifndef MINHEAP_H
#define MINHEAP_H


// Minimal array-based heap 
template<typename E>
class Mincompare {
public:
    static bool prior(const E& a, const E& b) { return a < b; }
};

template<typename E, typename Comp = Mincompare<E>>
class heap {
private:
    E* Heap;
    int maxSize;
    int n;
    void swapElems(int i, int j) { E tmp = Heap[i]; Heap[i] = Heap[j]; Heap[j] = tmp; }
    int parent(int pos) const { return (pos - 1) / 2; }
    int leftChild(int pos) const { return 2*pos + 1; }
    int rightChild(int pos) const { return 2*pos + 2; }
    bool isLeaf(int pos) const { return (pos >= n/2) && (pos < n); }
    void siftdown(int pos) {
        while (!isLeaf(pos)) {
            int j = leftChild(pos);
            int r = rightChild(pos);
            if ((r < n) && Comp::prior(Heap[r], Heap[j])) j = r;
            if (!Comp::prior(Heap[j], Heap[pos])) return;
            swapElems(pos, j);
            pos = j;
        }
    }
public:
    heap(int capacity) : maxSize(capacity), n(0) { Heap = new E[maxSize]; }
    heap(E* arr, int num, int capacity) : Heap(arr), maxSize(capacity), n(num) {
        for (int i = n/2 - 1; i >= 0; --i) siftdown(i);
    }
    ~heap() { delete[] Heap; }
    int size() const { return n; }
    bool isEmpty() const { return n == 0; }
// Add new elem at end then float up
    void insert(const E& value) {
        if (n >= maxSize) return;
        Heap[n] = value;
        int curr = n++;
        while (curr > 0 && Comp::prior(Heap[curr], Heap[parent(curr)])) {
            swapElems(curr, parent(curr));
            curr = parent(curr);
        }
    }
    E removeFirst() {
        if (n <= 0) return E();
        swapElems(0, n-1);
        n--;
        if (n > 0) siftdown(0);
        return Heap[n];
    }
};


#endif