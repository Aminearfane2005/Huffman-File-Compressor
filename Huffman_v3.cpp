#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <iomanip>


using namespace std;

const int MAXCODELEN = 100;

#include "KVpair.h"

#include"MinHeap.h"

#include"HuffNode.h"

#include"Hufftree.h"

// comparator for HuffTree pointers
class minTreeComp {
public:
    static bool prior(HuffTree<char>* x, HuffTree<char>* y) {
        if (!x || !y) return false;
        return x->weight() < y->weight();
    }
};

//Code table (array-based) 
//It stores which binary code belongs to which character
template <typename Elem>
class CodeTable {
private:
    Elem* obs;//array of character
    char** codes;// 2D array of code
    int currsize;
    int maxsize;
public:
    CodeTable(int size) : currsize(0), maxsize(size) {
        obs = new Elem[size];
        codes = new char*[size];
        for (int i = 0; i < size; ++i) {
            codes[i] = new char[MAXCODELEN + 1];//each pointer get a string 
            codes[i][0] = '\0'; // initilaize with empty string
        }
    }
    ~CodeTable() {
        for (int i = 0; i < maxsize; ++i) delete[] codes[i];
        delete[] codes;
        delete[] obs;
    }
    void addobject(const Elem& obj) { //add character to the next available slot
         if (currsize < maxsize) 
         obs[currsize++] = obj;
         }
    char* getcode(const Elem& obj) {
    for (int i = 0; i < currsize; ++i) {
        if (obj == obs[i]) return codes[i];
    }
    return nullptr;
}
    void setcode(const Elem& obj, const char* code) {
        for (int i = 0; i < currsize; ++i) {
            if (obj == obs[i]) {
                strncpy(codes[i], code, MAXCODELEN);//Copy the code string to that slot
                codes[i][MAXCODELEN] = '\0';
                return;
            }
        }
    }
};

// Bit writer/reader 
class BitWriter {// use for write bits until complate 8 bits for write 1byte in the file
private:
    ofstream& outFile;//ref to output file
    unsigned char buffer; //8-bit storage for collecting bits
    int bitCount; //bit counter
public:
   BitWriter(ofstream& file) : outFile(file), buffer(0), bitCount(0) {}
    void writeBit(int bit) {
        buffer = (buffer << 1) | (bit & 1);// Add bit to right
        ++bitCount;
        if (bitCount == 8) {
             outFile.put((char)buffer);// Write byte to file
             buffer = 0;
             bitCount = 0; 
            }
    }

    void writeCode(const char* code) {
        for (int i = 0; code[i] != '\0'; ++i)
        {

       
         writeBit(code[i] - '0'); 
        }
    }
    void flush() {//Write Remaining Bits

        if (bitCount > 0) {
             buffer <<= (8 - bitCount);
              outFile.put((char)buffer);
               buffer = 0;
                bitCount = 0;
             }
    }
    ~BitWriter() { flush(); }
};

class BitReader {//BitWriter packed bits into bytes → BitReader unpacks bytes back into bits!
private:
    ifstream& inFile;
    unsigned char buffer;
    int bitCount;
    bool eof;
public:
     BitReader(ifstream& file) : inFile(file), buffer(0), bitCount(8), eof(false) {}
    int readBit() {
        if (bitCount == 8) { // Buffer empty?
            if (!inFile.get((char&)buffer))//read new byte
             {
                 eof = true;  // Failed → end of file
                return -1;
             }
            bitCount = 0;// Reset: 0 bits read from new byte

        }

        int bit = (buffer >> (7 - bitCount)) & 1;// Extract bit ktakhod bit wa7d mn string li f buffer
        ++bitCount; // Mark as read
        return bit; // Return bit (0 or 1)
    }
    bool isEOF() const {
         return eof;
        }
};

//  Delete helpers 
template <typename Elem>
void deleteHuffHelp(HuffNode<Elem>* root) {
    if (!root) return;
    if (!root->isLeaf()) {
        deleteHuffHelp(root->left());
        deleteHuffHelp(root->right());
    }
    delete root;
}

template <typename Elem>
void deleteHuff(HuffTree<Elem>* tree) {
    if (!tree) return;
    deleteHuffHelp(tree->root());
    delete tree;
}

// Build codes & print tree
//It traverses the Huffman tree and creates a binary code (like "010", "1011")
//for each character, based on the path from root to leaf!
void buildcode(HuffNode<char>* root, CodeTable<char>* ct, char* prefix, int level, double& total) {
    if (!root) return;
    if (root->isLeaf()) {
        char value = (((LeafNode<char>*)root)->val())->value();
        // Cast root to LeafNode* to access character
        prefix[level] = '\0';//add null at position level
        ct->setcode(value, prefix);//Store in CodeTable:
        total += level * root->weight();
        // level = code length (e.g., 3 for "010")
        // weight = frequency (e.g., 10 for 'A')
        // total accumulates: 3 × 10 = 30
       // Used later: avgCodeLength = total / totalCharacters
        // Print mapping
        if (value >= 32 && value <= 126) {//Printable characters
            cout << "'" << value << "'\t" << prefix << " (freq=" << root->weight() << ")\n";
        } else {//  No Printable characters
            cout << "char(" << (int)(unsigned char)value << ")\t" << prefix << " (freq=" << root->weight() << ")\n";
        }
    } else {
        if (level >= MAXCODELEN) { cout << "Warning: code length exceeded\n"; return; }
        if (root->left()) {
            prefix[level] = '0';
            buildcode(root->left(), ct, prefix, level + 1, total);
        }
        if (root->right()) {
            prefix[level] = '1';
            buildcode(root->right(), ct, prefix, level + 1, total);
        }
    }
}

void printTree(HuffNode<char>* node, int depth = 0) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) cout << "  ";
    if (node->isLeaf()) {
         // Leaf node: show character and frequency
        char v = (((LeafNode<char>*)node)->val())->value();
        if (v >= 32 && v <= 126) cout << "Leaf '" << v << "' (freq=" << node->weight() << ")\n";
        else cout << "Leaf char(" << (int)(unsigned char)v << ") (freq=" << node->weight() << ")\n";
    } else {
        // Internal node: show total frequency
        cout << "Node (freq=" << node->weight() << ")\n";
        // Recursively print left and right subtrees (indented more)
        printTree(node->left(), depth + 1);
        printTree(node->right(), depth + 1);
    }
}

//  Frequency analysis 
//It counts how many times each character appears in a file.
void countFrequencies(const string& filename, int freq[256]) {
     // 1. Clear all counters (256 possible ASCII values)
    for (int i = 0; i < 256; ++i) freq[i] = 0;

    // 2. Open file in binary mode (reads everything)
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file: " << filename << "\n";
        return;
    }

     // 3. Read character by character, increment counter
    char ch;
    while (file.get(ch)) freq[(unsigned char)ch]++;
    file.close();
}

void printFrequencies(const int freq[256], ostream& os) {
    // printer for the frequency array.
    os << "Character frequencies (non-zero):\n";
    for (int i = 0; i < 256; ++i) {// Only show characters that appear
        if (freq[i] > 0) {
            if (i >= 32 && i <= 126) os << "'" << (char)i << "': " << freq[i] << "\n";// printable
            else os << "char(" << i << "): " << freq[i] << "\n"; // Non-printable (tabs, newlines, etc.)
        }
    }
}

// ----------------- Build Huffman tree using heap (O(n log n)) -----------------
template <typename Elem>
HuffTree<Elem>* buildHuff(heap<HuffTree<Elem>*, minTreeComp>* forest) {
    if (forest->isEmpty()) return nullptr;
    while (forest->size() > 1) {//more then one tree
        HuffTree<Elem>* t1 = forest->removeFirst();//Take the two lightest trees from the heap
        HuffTree<Elem>* t2 = forest->removeFirst();//The heap is a min-heap (smallest at top), so removeFirst() always gives smallest.
        HuffTree<Elem>* merged = new HuffTree<Elem>(t1, t2);//"Create new parent tree with t1 and t2 as children
        // delete wrapper objects (they do not free nodes)
        delete t1;
        delete t2;
        forest->insert(merged);
    }
    HuffTree<Elem>* finalTree = forest->removeFirst();
    return finalTree;
}

// ----------------- CompressionResult -----------------
struct CompressionResult {
    double avgCodeLength;
    int totalWeight;
    CompressionResult(){
        avgCodeLength = 0.0;
        totalWeight = 0;
    }

    CompressionResult(double avg, int weight){
        avgCodeLength = avg;
        totalWeight = weight;

    } 
};

// ----------------- Compression statistics -----------------
//for compating origenal and compressed HUffman code
void printCompressionStats(const string& originalFile,
                           const string& compressedFile,
                           double avgCodeLength,
                           ostream& os) {

//ios::binary: Read as raw bytes 
//ios::ate: Open at end (so tellg() gives file size), for give pos = size ,open at end|Opens file, moves to end immediately:
//tellg(): "Tell get position" = current position = file size
//static_cast<long long>: Convert to 64-bit integer (large files!)                           
    ifstream original(originalFile, ios::binary | ios::ate);
    if (!original) { os << "Error: Cannot find original file!\n"; return; }
    long long originalSize = static_cast<long long>(original.tellg());
    original.close();

    ifstream compressed(compressedFile, ios::binary | ios::ate);
    if (!compressed) { os << "Error: Cannot find compressed file!\n"; return; }
    long long compressedSize = static_cast<long long>(compressed.tellg());
    compressed.close();

    if (originalSize == 0) { os << "Original file empty\n"; return; }

    double ratio = static_cast<double>(compressedSize) / static_cast<double>(originalSize);

    os << "\n=== COMPRESSION REPORT ===\n";
    os << "Original file: " << originalSize << " bytes\n";
    os << "Compressed file: " << compressedSize << " bytes\n";
    os << fixed << setprecision(2);
    os << "Report compression ratio: " << ratio << "\n";
    if (ratio < 1.0) {//good compression
        os << "Saved: " << (1.0 - ratio) * 100.0 << "%\n";
    } else if (ratio > 1.0) {//bad compression
        os << "File larger by: " << (ratio - 1.0) * 100.0 << "%\n";
    } else {
        os << "Same size\n";
    }
    os << "Average code length: " << avgCodeLength << " bits/char\n";
    os.unsetf(std::ios_base::floatfield);
}


#include"Compression.h"
#include"Decompression.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " inputFile compressedFile [decompressedOutput] [reportFile] [--show-tree]\n";
        return 1;
    }
    string inputFile = argv[1];
    string compressedFile = argv[2];
    string decompressedFile = (argc > 3) ? argv[3] : "";
    string reportFile = (argc > 4) ? argv[4] : "outputfile.txt";

    bool showTree = false;
    for (int i = 1; i < argc; ++i) if (string(argv[i]) == "--show-tree") showTree = true;

    cout << "Compressing: " << inputFile << " -> " << compressedFile << "\n";
    CompressionResult res = compressFile(inputFile, compressedFile, showTree);

    // write stats to report file
    ofstream report(reportFile);
    if (!report) {
        cerr << "Warning: Cannot open report file '" << reportFile << "'. Printing report to stdout.\n";
        printCompressionStats(inputFile, compressedFile, res.avgCodeLength, cout);
    } else {
        printCompressionStats(inputFile, compressedFile, res.avgCodeLength, report);
        report.close();
        cout << "Report written to: " << reportFile << "\n";
    }

    // If decompressed output requested, run decompression and write to that file
    if (!decompressedFile.empty()) {
        cout << "Decompressing: " << compressedFile << " -> " << decompressedFile << "\n";
        decompressFile(compressedFile, decompressedFile);
    }

    return 0;
}
