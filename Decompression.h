#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H


// ----------------- Decompression -----------------
void decompressFile(const string& compressedFile, const string& outputFile) {
    //Open the Compressed File
    ifstream in(compressedFile, ios::binary);
    if (!in) { cerr << "Error: Cannot open compressed file: " << compressedFile << "\n"; return; }

    int uniqueChars = 0;
    in.read(reinterpret_cast<char*>(&uniqueChars), sizeof(uniqueChars));
    if (!in) { cerr << "Error: Failed to read header (uniqueChars)\n"; in.close(); return; }

    int freq[256];
    //Read each character + its count
    for (int i = 0; i < 256; ++i) freq[i] = 0;
    for (int i = 0; i < uniqueChars; ++i) {
        unsigned char ch;
        int f;
        in.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        in.read(reinterpret_cast<char*>(&f), sizeof(f));
        if (!in) { cerr << "Error: Failed to read header entry\n"; in.close(); return; }
        freq[ch] = f;
    }
    //Read total character count
    int totalChars = 0;
    in.read(reinterpret_cast<char*>(&totalChars), sizeof(totalChars));
    if (!in) { cerr << "Error: Failed to read totalChars\n"; in.close(); return; }

    // Rebuild the Huffman Tree
    const int MAX_TREES = 256;
    heap<HuffTree<char>*, minTreeComp> forest(MAX_TREES);
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            HuffTree<char>* t = new HuffTree<char>(static_cast<char>(i), freq[i]);
            forest.insert(t);
        }
    }

    // If only one symbol
    if (forest.size() == 1) {
        HuffTree<char>* only = forest.removeFirst();
        HuffTree<char>* dummy = new HuffTree<char>('\0', 0);
        HuffTree<char>* merged = new HuffTree<char>(only, dummy);
        delete only;
        delete dummy;
        forest.insert(merged);
    }

    HuffTree<char>* huffTree = buildHuff<char>(&forest);
    if (!huffTree) { cerr << "Error: Failed to rebuild Huffman tree\n"; in.close(); return; }

    ofstream out(outputFile, ios::binary);
    if (!out) { cerr << "Error: Cannot open output file: " << outputFile << "\n"; deleteHuff(huffTree); in.close(); return; }

    // If root is a leaf (single symbol), write it repeated
    HuffNode<char>* root = huffTree->root();
    if (root->isLeaf()) {
        char value = (((LeafNode<char>*)root)->val())->value();
        for (int i = 0; i < totalChars; ++i) out.put(value);
        out.close();
        in.close();
        deleteHuff(huffTree);
        cout << "Decompression complete (single-symbol file). Output: " << outputFile << "\n";
        return;
    }
   //Reading Bits
    BitReader bitReader(in);
    int decoded = 0;
    while (decoded < totalChars) {
        HuffNode<char>* node = huffTree->root();// Start at tree root
        // Walk down the tree
        while (!node->isLeaf()) {
            int bit = bitReader.readBit();
            if (bit == -1) {
                cerr << "Error: Unexpected end of compressed data\n";
                out.close();
                in.close();
                deleteHuff(huffTree);
                return;
            }
            node = (bit == 0) ? node->left() : node->right();
        }
        // Found a leaf! Get the character
        char value = (((LeafNode<char>*)node)->val())->value();
        out.put(value);
        ++decoded;
    }

    out.close();
    in.close();
    deleteHuff(huffTree);
    cout << "Decompression complete. Output: " << outputFile << "\n";
}




#endif