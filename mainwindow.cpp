#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the File menu
    fileMenu = menuBar()->addMenu(tr("File"));

    // Add the select images button to the File menu
    QAction *selectImagesAction = new QAction(QIcon(":/icons/open.png"), tr("Stitch Images"), this);
    selectImagesAction->setShortcut(QKeySequence::Open);
    connect(selectImagesAction, &QAction::triggered, this, &MainWindow::uploadRawFolder);
    fileMenu->addAction(selectImagesAction);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * This function allows users to upload a folder of subfolders containing images to be stitched
 * and then saves the stitched images to another user selected folder. It assumes that each subfolder with
 * images that need to be stitched begins with the naming convention "XY".
 *
 * It does so by:
 *      1. Prompting the user to choose a folder containing the subfolders with images they want stitched.
 *      2. Prompting the user to choose a folder to save stitched images to.
 *      3. Calling on the stitchFolder helper function on each subfolder in the selected folder.
 *
 * @author Kai Jun Zhuang
 */
void MainWindow::uploadRawFolder()
{
    // Show a file dialog to select the folder containing the images to stitch
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder Containing XY Subfolders"));
    if (folderPath.isEmpty()) {
        qDebug() << "No folder selected. Please choose a folder containing the images you want stitched.";
        return;
    }

    // Show a file dialog to select the folder to save stitched images to
    QString savePath = QFileDialog::getExistingDirectory(this, tr("Select Where to Save Stitched Images"));
    if (savePath.isEmpty()) {
        qDebug() << "No folder selected. Please choose a folder to save all the stitched images to.";
        return;
    }

    // Get all subfolders in folderPath
    QDir folder(folderPath);
    QStringList subfolders = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Filter through each subfolder and only call stitchFolder on the ones including XY
    foreach (QString subfolder, subfolders)
    {
        if (subfolder.contains("XY")) {
            QString path = folderPath + "/" + subfolder;

            stitchFolder(path, savePath, subfolder.replace(0, 2, "A"));
        }
    }
}

/**
 * This function is a helper function for uploadRawFolder. It takes in a subfolder, sorts images into
 * appropriate lists, and then calls the stitchImage helper function on the images. It assumes that the
 * subfolder passed to it contains 4 channels with naming conventions CH1, CH2, CH3, and CH4, and that
 * there exists an overlay of all channels with naming convention Overlay.
 *
 * @author Kai Jun Zhuang
 * @param folderPath The path to the subfolder passed in uploadRawFolder.
 * @param savePath The path to save stitched images to.
 * @param fileName The name of the subfolder to be used in naming the stitched images.
 */
void MainWindow::stitchFolder(QString folderPath, QString savePath, QString fileName)
{
    // Get a list of all .tif files in the selected folder
    QDir folder(folderPath);
    QStringList filters;
    filters << "*.tif";
    folder.setNameFilters(filters);
    QStringList tifFiles = folder.entryList(QDir::Files);

    // Initialize lists
    QStringList ch1;
    QStringList ch2;
    QStringList ch3;
    QStringList ch4;
    QStringList overlay;

    // Add images for channels and overlay into respective lists
    foreach (QString file, tifFiles) {
        if (file.contains("CH1")) {
            ch1.append(folderPath + "/" + file);
        } else if (file.contains("CH2")) {
            ch2.append(folderPath + "/" + file);
        } else if (file.contains("CH3")) {
            ch3.append(folderPath + "/" + file);
        } else if (file.contains("CH4")) {
            ch4.append(folderPath + "/" + file);
        } else if (file.contains("Overlay")) {
            overlay.append(folderPath + "/" + file);
        }
    }

    // Stitch images in each of the lists
    stitchImages(ch1, savePath, fileName + "_CH1");
    stitchImages(ch2, savePath, fileName + "_CH2");
    stitchImages(ch3, savePath, fileName + "_CH3");
    stitchImages(ch4, savePath, fileName + "_CH4");
    stitchImages(overlay, savePath, fileName + "_Overlay");
}

/**
 * This function is a helper function for the stitchFolder function. It takes a list of images,
 * ensures that there are 9 images, stitches the 9 images together, and then saves the images to
 * the savePath.
 *
 * @author Kai Jun Zhuang
 * @param fileNames A list of the file paths to each image.
 * @param savePath The path to save stitched images to.
 * @param fileName The name for the saved image.
 */
void MainWindow::stitchImages(QStringList fileNames, QString savePath, QString fileName)
{
    // Check if there are 9 images in the fileNames list, if not return an error to console
    if (fileNames.size() == 9) {
        // Load the images
        QImage image1(fileNames[0]);
        QImage image2(fileNames[1]);
        QImage image3(fileNames[2]);
        // Account for how files are ordered
        QImage image4(fileNames[5]);
        QImage image5(fileNames[4]);
        // Account for how files are ordered
        QImage image6(fileNames[3]);
        QImage image7(fileNames[6]);
        QImage image8(fileNames[7]);
        QImage image9(fileNames[8]);

        // Create a QPixmap to paint on
        QPixmap pixmap(image1.width() * 3 - 578, image1.height() * 3 - 432);

        // Create a QPainter to draw on the pixmap
        QPainter painter(&pixmap);

        // Draw the images onto the pixmap
        painter.drawImage(0, 0, image1);
        painter.drawImage(image1.width() - 289, 0, image2);
        painter.drawImage(image1.width() * 2 - 578, 0, image3);
        painter.drawImage(0, image1.height() - 216, image4);
        painter.drawImage(image1.width() - 289, image1.height() - 216, image5);
        painter.drawImage(image1.width() * 2 - 578, image1.height() - 216, image6);
        painter.drawImage(0, image1.height() * 2 - 432, image7);
        painter.drawImage(image1.width() - 289, image1.height() * 2 - 432, image8);
        painter.drawImage(image1.width() * 2 - 578, image1.height() * 2 - 432, image9);

        // Convert the pixmap to an image
        QImage image = pixmap.toImage();

        // Show a file dialog to select the output file
        QString filePath = savePath + "/" + fileName + ".png";

        // Save the image to the selected file
        image.save(filePath);
    } else {
        // There were fewer/more than 9 images for the respective channel so log an error message
        qDebug() << "Missing images for " + fileName + " please ensure there are exactly 9 images to complete the stitch.";
        return;
    }
}

