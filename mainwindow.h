#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QMenu>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void uploadRawFolder();
    void stitchFolder(QString folderPath, QString savePath, QString fileName);
    void stitchImages(QStringList fileNames, QString savePath, QString fileName);

private:
    Ui::MainWindow *ui;
    QMenu *fileMenu;
    QStringList tifFiles;
    QStringList ch1;
    QStringList ch2;
    QStringList ch3;
    QStringList ch4;
    QStringList overlay;
};

#endif // MAINWINDOW_H
