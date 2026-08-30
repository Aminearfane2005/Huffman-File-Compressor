
#ifndef KVPAIR_H
#define KVPAIR_H


// ----------------- Basic helpers & KVpair -----------------
//Used to store character-frequency pairs in leaf nodes.
template <typename Key, typename Elem>
class KVpair {
private:
  Key k;    // Frequency (int)
    Elem e;   // Character (char)
public:
    KVpair() {}
    KVpair(Key kval, Elem eval){
        k=kval;
        e = eval;
    }
    KVpair(const KVpair& o) {
        k = o.k;
        e= o.e;
    } 
    void operator=(const KVpair& o) { 
        k = o.k; 
        e = o.e; 
    }
    Key key() const { 
        return k;
     }
    void setKey(Key ink) { 
        k = ink; 
    }
    Elem value() const {
         return e;
         }
};




#endif