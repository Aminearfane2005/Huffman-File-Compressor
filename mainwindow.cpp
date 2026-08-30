#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "HuffmanCore.h"
#include "Compression.h"
#include "Decompression.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
    // widgets are owned by Qt's parent-child tree, no manual delete needed
}

void MainWindow::setupUi()
{
    setWindowTitle("Huffman Compressor");
    resize(560, 420);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);

    // --- File selection row ---
    auto *fileRow = new QHBoxLayout();
    filePathEdit = new QLineEdit(central);
    filePathEdit->setReadOnly(true);
    filePathEdit->setPlaceholderText("No file selected...");
    selectFileButton = new QPushButton("Select File", central);
    fileRow->addWidget(filePathEdit);
    fileRow->addWidget(selectFileButton);
    mainLayout->addLayout(fileRow);

    // --- Action buttons row ---
    auto *actionRow = new QHBoxLayout();
    compressButton = new QPushButton("Compress", central);
    decompressButton = new QPushButton("Decompress", central);
    compressButton->setEnabled(false);
    decompressButton->setEnabled(false);
    actionRow->addWidget(compressButton);
    actionRow->addWidget(decompressButton);
    mainLayout->addLayout(actionRow);

    // --- Progress + stats ---
    progressBar = new QProgressBar(central);
    progressBar->setRange(0, 0);   // indeterminate "busy" style
    progressBar->setVisible(false);
    mainLayout->addWidget(progressBar);

    statsLabel = new QLabel("No results yet.", central);
    statsLabel->setWordWrap(true);
    mainLayout->addWidget(statsLabel);

    // --- Log output ---
    logOutput = new QTextEdit(central);
    logOutput->setReadOnly(true);
    mainLayout->addWidget(logOutput, /*stretch=*/1);

    connect(selectFileButton, &QPushButton::clicked, this, &MainWindow::onSelectFileClicked);
    connect(compressButton,   &QPushButton::clicked, this, &MainWindow::onCompressClicked);
    connect(decompressButton, &QPushButton::clicked, this, &MainWindow::onDecompressClicked);
}

void MainWindow::appendLog(const QString &text)
{
    logOutput->append(text);
}

void MainWindow::onSelectFileClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Select a file to compress");
    if (path.isEmpty()) return; // user cancelled

    selectedFilePath = path;
    filePathEdit->setText(path);
    compressButton->setEnabled(true);
    decompressButton->setEnabled(true);
    statsLabel->setText("File selected. Ready to compress or decompress.");
}

void MainWindow::onCompressClicked()
{
    if (selectedFilePath.isEmpty()) return;

    QString outPath = QFileDialog::getSaveFileName(this, "Save compressed file as",
                                                   selectedFilePath + ".huff");
    if (outPath.isEmpty()) return;

    progressBar->setVisible(true);
    compressButton->setEnabled(false);
    decompressButton->setEnabled(false);
    appendLog("--- Compressing: " + selectedFilePath + " ---");

    CompressionResult result;
    runCapturingCout([&]() {
        result = compressFile(selectedFilePath.toStdString(), outPath.toStdString(), /*showTree=*/false);
    });

    progressBar->setVisible(false);
    compressButton->setEnabled(true);
    decompressButton->setEnabled(true);

    if (result.totalWeight == 0) {
        QMessageBox::warning(this, "Compression failed",
                             "Something went wrong during compression. Check the log below.");
        return;
    }

    lastCompressedPath = outPath;

    QFileInfo origInfo(selectedFilePath);
    QFileInfo compInfo(outPath);
    qint64 origSize = origInfo.size();
    qint64 compSize = compInfo.size();
    double ratio = origSize > 0 ? (double)compSize / (double)origSize : 0.0;

    QString stats = QString(
                        "Original: %1 bytes\nCompressed: %2 bytes\nRatio: %3\nAvg code length: %4 bits/char"
                        ).arg(origSize).arg(compSize).arg(ratio, 0, 'f', 2).arg(result.avgCodeLength, 0, 'f', 2);

    statsLabel->setText(stats);
    appendLog("Compression complete: " + outPath);
}

void MainWindow::onDecompressClicked()
{
    QString sourcePath = lastCompressedPath.isEmpty() ? selectedFilePath : lastCompressedPath;

    QString inPath = QFileDialog::getOpenFileName(this, "Select compressed file", sourcePath);
    if (inPath.isEmpty()) return;

    QString outPath = QFileDialog::getSaveFileName(this, "Save decompressed file as");
    if (outPath.isEmpty()) return;

    progressBar->setVisible(true);
    compressButton->setEnabled(false);
    decompressButton->setEnabled(false);
    appendLog("--- Decompressing: " + inPath + " ---");

    runCapturingCout([&]() {
        decompressFile(inPath.toStdString(), outPath.toStdString());
    });

    progressBar->setVisible(false);
    compressButton->setEnabled(true);
    decompressButton->setEnabled(true);

    appendLog("Decompression complete: " + outPath);
    statsLabel->setText("Decompressed file written to:\n" + outPath);
}