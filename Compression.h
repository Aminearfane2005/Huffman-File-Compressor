#ifndef COMPRESSION_H
#define COMPRESSION_H

// ----------------- Compression -----------------
CompressionResult compressFile(const string& inputFile, const string& outputFile, bool showTree = false) {
    int freq[256];
    countFrequencies(inputFile, freq);//Counting how many times each letter appears in the file

    int totalChars = 0;
    for (int i = 0; i < 256; ++i) totalChars += freq[i];// check if file empty
    if (totalChars == 0) { 
        cerr << "Error: Input file is empty.\n"; 
        return CompressionResult();
     }

    // Build initial forest
    const int MAX_TREES = 256;
    heap<HuffTree<char>*, minTreeComp> forest(MAX_TREES);
    CodeTable<char> codeTable(256);
//Make Mini Trees for Each Character
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            char c = static_cast<char>(i);
            HuffTree<char>* t = new HuffTree<char>(c, freq[i]);
            forest.insert(t);
            codeTable.addobject(c);
        }
    }

    // If only one symbol, create dummy leaf to ensure non-empty codes
    if (forest.size() == 1) {
        HuffTree<char>* only = forest.removeFirst();
        HuffTree<char>* dummy = new HuffTree<char>('\0', 0);//two "things" to build Huffman tree. 
        HuffTree<char>* merged = new HuffTree<char>(only, dummy);
        delete only;
        delete dummy;
        forest.insert(merged);
    }

    HuffTree<char>* huffTree = buildHuff<char>(&forest);//build big tree from two small trees
    if (!huffTree) { 
        cerr << "Error: Failed to build Huffman tree.\n";
         return CompressionResult();
         }

    //"Yes":draw Huf tree
    if (showTree) {
        cout << "Huffman tree structure:\n";
        printTree(huffTree->root());
    }

    // Generate codes and display
    char prefix[MAXCODELEN + 1];
    memset(prefix, 0, sizeof(prefix));
    double weightedBits = 0.0;
    cout << "\nHuffman Codes (symbol -> code) and frequencies:\n";
    buildcode(huffTree->root(), &codeTable, prefix, 0, weightedBits);
    double avgCodeLength = weightedBits / static_cast<double>(huffTree->weight());
    cout << "\nAverage code length: " << avgCodeLength << " bits\n";

    // Write compressed file header + bitstream
    ofstream out(outputFile, ios::binary);
    if (!out) { cerr << "Error: Cannot open output file: " << outputFile << "\n"; deleteHuff(huffTree); return CompressionResult(); }

    int uniqueChars = 0;
    for (int i = 0; i < 256; ++i) if (freq[i] > 0) ++uniqueChars;
    out.write(reinterpret_cast<const char*>(&uniqueChars), sizeof(uniqueChars));
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            unsigned char ch = static_cast<unsigned char>(i);
            int f = freq[i];
            out.write(reinterpret_cast<const char*>(&ch), sizeof(ch));
            out.write(reinterpret_cast<const char*>(&f), sizeof(f));
        }
    }
    out.write(reinterpret_cast<const char*>(&totalChars), sizeof(totalChars));

   // Write body
BitWriter bitWriter(out);
ifstream input(inputFile, ios::binary);
char c;
bool failed = false;
while (input.get(c)) {
    unsigned char uc = static_cast<unsigned char>(c);
    char key = static_cast<char>(uc); // matches how you stored keys in codeTable
    char* code = codeTable.getcode(key);
    if (!code) {
        cerr << "No code found for byte " << static_cast<int>(uc) << "\n";
        failed = true;
        break;
    }
    if (code[0] == '\0') {
        cerr << "Empty code for byte " << static_cast<int>(uc) << "\n";
        failed = true;
        break;
    }
    bitWriter.writeCode(code);
}
bitWriter.flush();
input.close();
out.close();
    if (failed) {
        remove(outputFile.c_str());
        deleteHuff(huffTree);
        cerr << "Error: failed during encoding, partial output removed.\n";
        return CompressionResult();
    }

    deleteHuff(huffTree);
    return CompressionResult(avgCodeLength, totalChars);
}


#endif