
# Huffman File Compressor

A complete lossless file compression tool built from scratch in C++, using a custom implementation of the Huffman Coding algorithm — no external compression libraries, just hand-written binary trees, a min-heap priority queue, and bit-level file I/O.

Originally built as a C++ / Data Structures course project, then extended with a Qt-based desktop GUI so it's actually usable as a real tool, not just a console demo.

## Features

- **Full compression/decompression pipeline** — reads any file, analyzes character frequencies, builds an optimal Huffman tree, and writes a smaller compressed file that decompresses back to the original, byte for byte.
- **Custom data structures** — a template-based min-heap for O(n log n) tree construction, a hand-rolled Huffman tree with leaf/internal node classes, and a parallel-array code table.
- **Bit-level I/O** — custom `BitWriter`/`BitReader` classes to pack variable-length Huffman codes into byte-aligned output.
- **Compression stats** — reports average code length, compression ratio, and space savings for every run.
- **Desktop GUI (Qt)** — select a file, compress or decompress it, and see live stats and a character/code breakdown, without touching a terminal.

## Screenshots

<img width="1366" height="768" alt="Weixin Image_20260831105957_36_50" src="https://github.com/user-attachments/assets/e3badb8b-d9c7-430d-979d-d3ceb1ea167f" /><img width="976" height="1091" alt="Gemini_Generated_Image_igomjtigomjtigom" src="https://github.com/user-attachments/assets/3eb4ff3c-0b84-4971-9347-9dfe75bfa843" />
<img width="1366" height="768" alt="Weixin Image_20260831110005_37_50" src="https://github.com/user-attachments/assets/8c9f76d5-cb9c-4e93-9b8c-a4b51810459a" />

<img width="515" height="601" alt="Screenshot 2026-08-31 110611" src="https://github.com/user-attachments/assets/27c693d4-cad1-4289-9ae9-d0cd7a18225d" />


## Getting Started (Linux — GUI)

The GUI is currently packaged as a portable **AppImage**, so there's nothing to install.

```bash
# make it executable (one-time)
chmod +x CompressFiles-x86_64.AppImage

# run it
./CompressFiles-x86_64.AppImage
```

Then just click **Select File**, choose anything you want to compress, and hit **Compress** or **Decompress**. Stats and the generated Huffman code table show up right in the window.

> **Note:** Huffman coding compresses best on files with skewed byte frequencies — plain text, logs, source code, CSVs. Already-dense binary formats (executables, images, zip files, videos) won't shrink much, since there's little frequency imbalance left for Huffman to exploit. That's expected, not a bug.

**Windows GUI build is on the way.** In the meantime, Windows (and any other platform) can use the command-line version below.

## Command-Line Usage (any platform)

The core compressor is a single, dependency-free C++ file you can compile anywhere with a C++17-capable compiler.

**Build:**

```bash
g++ -std=c++17 Huffman_v3.cpp -o huffman
```

On Windows, the same command works from a Developer Command Prompt (MinGW or MSVC), or via WSL.

**Usage:**

```bash
./huffman <inputFile> <compressedFile> [decompressedOutput] [reportFile] [--show-tree]
```

| Argument | Required | Description |
|---|---|---|
| `inputFile` | Yes | File to compress |
| `compressedFile` | Yes | Where to write the compressed output |
| `decompressedOutput` | No | If given, decompresses `compressedFile` back to this path right after compressing |
| `reportFile` | No | Where to write the compression stats report (defaults to `outputfile.txt`) |
| `--show-tree` | No | Prints the full Huffman tree structure to the console |

**Example — compress, decompress, and verify in one go:**

```bash
./huffman input.txt compressed.huff restored.txt report.txt --show-tree
diff input.txt restored.txt   # no output = byte-for-byte identical
```

## Project Structure

```
HuffNode.h        Abstract node base class + LeafNode / IntlNode
HuffTree.h        Huffman tree container (build, merge, weight)
KVpair.h          Simple key-value pair for frequency/character storage
MinHeap.h         Template-based min-heap for O(n log n) tree building
Compression.h     compressFile(), frequency analysis, code generation
Decompression.h   decompressFile(), header parsing, tree reconstruction
Huffman_v3.cpp    Standalone CLI entry point (compiles with no other deps)
mainwindow.h/.cpp Qt GUI wiring around the same core compression logic
```

## Roadmap

- [x] Core Huffman compression/decompression engine
- [x] Compression ratio & stats reporting
- [x] Desktop GUI (Qt) for Linux
- [ ] Windows GUI build
- [ ] Tree visualization inside the GUI
- [ ] Drag-and-drop file support

## Acknowledgments

Built as a C++ / Data Structures course project — the push to add a GUI on top came from my instructor's feedback, and turned this from a class assignment into something I'd actually use.

Thanks for checking it out — feedback and issues are always welcome.
