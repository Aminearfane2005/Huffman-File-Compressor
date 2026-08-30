#ifndef HUFFTREE_H
#define HUFFTREE_H

template <typename E>
//The stand/base that holds the tree
class HuffTree {
private:
    HuffNode<E>* Root;
public:
    HuffTree(const E& val, int freq) {
         Root = new LeafNode<E>(val, freq);//Creates a tree with just ONE character (a leaf).
         }
    HuffTree(HuffTree<E>* l, HuffTree<E>* r) {
         Root = new IntlNode<E>(l->root(), r->root());//combin two smaller trees to one bigger tree
         }
    ~HuffTree() {}
    HuffNode<E>* root() const { 
        return Root; 
    }
   int weight() const {// Returns total frequency count of the tree
    if (Root != nullptr) {
        return Root->weight();
    } else {
        return 0;
    }
}
};




#endif