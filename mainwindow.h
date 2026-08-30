#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QProgressBar>
#include <sstream>
#include <iostream>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectFileClicked();
    void onCompressClicked();
    void onDecompressClicked();

private:
    // Widgets built in code (no .ui file dependency)
    QLineEdit    *filePathEdit;
    QPushButton  *selectFileButton;
    QPushButton  *compressButton;
    QPushButton  *decompressButton;
    QTextEdit    *logOutput;
    QLabel       *statsLabel;
    QProgressBar *progressBar;

    QString selectedFilePath;
    QString lastCompressedPath;

    void setupUi();          // builds and lays out the widgets above
    void appendLog(const QString &text);

    // Runs `fn` while capturing anything it writes to std::cout,
    // then appends that captured text to the log. Used because
    // buildcode()/printTree() in your Huffman code write to cout,
    // which would otherwise vanish in a GUI app.
    // (Template implementation must live in the header.)
    template <typename Fn>
    void runCapturingCout(Fn fn) {
        std::ostringstream captured;
        std::streambuf *oldBuf = std::cout.rdbuf(captured.rdbuf());
        fn();
        std::cout.rdbuf(oldBuf);
        QString text = QString::fromStdString(captured.str());
        if (!text.isEmpty()) appendLog(text);
    }
};
#endif // MAINWINDOW_H