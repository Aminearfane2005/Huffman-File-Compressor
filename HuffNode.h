
#ifndef HUFFNODE_H
#define HUFFNODE_H

//  Huffman node & tree types 
template <typename E>
//Rules every node must follow
class HuffNode {
public:
    virtual ~HuffNode() {}
    virtual int weight() const = 0;
    virtual bool isLeaf() const = 0;
    virtual HuffNode<E>* left() const = 0;
    virtual HuffNode<E>* right() const = 0;
};

template <typename Elem>

// Stores actual data (character + frequency)
class LeafNode : public HuffNode<Elem> {
private:
    KVpair<int, Elem>* it;
public:
    LeafNode(const Elem& val, int freq) { 
        it = new KVpair<int, Elem>(freq, val);
     }
    ~LeafNode() { 
        delete it;
     }
    int weight() const override {
         return it->key();
         }
    KVpair<int, Elem>* val() const {
         return it;
         }
    bool isLeaf() const override {
         return true;
         }
    HuffNode<Elem>* left() const override {
         return nullptr;
         }
    HuffNode<Elem>* right() const override {
         return nullptr;
         }
};

template <typename E>
//Connects two nodes, calculates total
class IntlNode : public HuffNode<E> {
private:
    HuffNode<E>* lc;
    HuffNode<E>* rc;
    int wgt;
public:
    IntlNode(HuffNode<E>* l, HuffNode<E>* r) : lc(l), rc(r) {
        wgt = 0;
        if (l) wgt += l->weight();
        if (r) wgt += r->weight();
    }
    ~IntlNode() {}
    int weight() const override {
         return wgt;
         }
    bool isLeaf() const override { 
        return false;
     }
    HuffNode<E>* left() const override {
         return lc; 
        }
    HuffNode<E>* right() const override {
         return rc; 
        }
    void setLeft(HuffNode<E>* b) {
         lc = b;
         }
    void setRight(HuffNode<E>* b) {
         rc = b; 
        }
};




#endif