#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <QGraphicsSceneMouseEvent>
#include <QDesktopServices>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>

using namespace cv;

// namespace(hier==Klasse)::Funktion_aus_Namespace()
MainWindow::MainWindow(QWidget *parent) : // Initialisierungsliste
    QMainWindow(parent),
    ui(new Ui::MainWindow), // "ui = new Ui::MainWindow"
    left_ellipse(nullptr),
    right_ellipse(nullptr),
    pixmap_item(nullptr),
    current_framenr(-1),
    num_frames(0),
    framerate(1)
{
    left_ellipse_pen.setColor(QColor(255, 0, 0));
    right_ellipse_pen.setColor(QColor(0, 255, 0));

    ui->setupUi(this);

    graphics_scene = new QGraphicsScene(this);
    pixmap_item = new QGraphicsPixmapItem();
    graphics_scene->addItem(pixmap_item);

    // install event filter
    graphics_scene->installEventFilter(this);
    // ToDo install event filter/signal for save button


    ui->graphicsView->setScene(graphics_scene); // graphicsView is QGraphicsView widget


    loadVideo("/home/amelie/Uni/Arbeit/PegTransfer.avi");
    readCSV();

    loadFrame(0);
}

MainWindow::~MainWindow() // for every "new" add one "delete"
{
    delete ui;
    delete graphics_scene;
}

void MainWindow::loadVideo(std::string const& filename) {
    // https://amin-ahmadi.com/2018/03/29/how-to-read-process-and-display-videos-using-qt-and-opencv/
    video.open(filename);

    if(!video.isOpened())
    {
        QMessageBox::critical(this,
                              "Camera Error",
                              "Make sure you entered a correct camera index,"
                              "<br>or that the camera is not being accessed by another program!");
        return;
    }

    num_frames = static_cast<int>(video.get(CAP_PROP_FRAME_COUNT));
    ui->frameSpinBox->setMaximum(num_frames-1); // maximum from spin box: begin count 0
}

void MainWindow::setFramerate(int _framerate) {
    framerate = _framerate;
    refreshButtonState();
}

void MainWindow::loadNextFrame() {
    loadFrame(current_framenr + framerate);
}

void MainWindow::loadFrame(int framenumber) {
    // buttons don't react if framenumber gets negative
    if(framenumber < 0 || framenumber >= num_frames) {
        return;
    }
    if(framenumber != current_framenr) { // prevent 2x/double loadNextFrame trigger
        current_framenr = framenumber;
        video.set(CAP_PROP_POS_FRAMES, current_framenr);
        ui->frameSpinBox->setValue(current_framenr);

        Mat frame;
        video >> frame;
        if (!frame.empty())
        {
            QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            pixmap_item->setPixmap(QPixmap::fromImage(qimg.rgbSwapped()));

            InstrumentPair const &pair = instrumentPairs[current_framenr]; //InstrumentPair refers to element in vector
            setLeftInstrumentPos(pair.xLeft, pair.yLeft);
            setRightInstrumentPos(pair.xRight, pair.yRight);
        }
        refreshButtonState();
    }
}

void MainWindow::refreshButtonState() {

    // Previous Button
    int framerate = ui->frameRateSpinBox->value();
    if(current_framenr - framerate < 0) {
        ui->prevFrameButton->setEnabled(false);
    }
    else {
        ui->prevFrameButton->setEnabled(true);
    }

    // Next Frame Button
    if(current_framenr + framerate >= num_frames) {
        ui->nextFrameButton->setEnabled(false);
    }
    else{
        ui->nextFrameButton->setEnabled(true);
    }

    // First Frame Button
    if(current_framenr == 0) {
        ui->firstFrameButton->setEnabled(false);
    }
    else {
        ui->firstFrameButton->setEnabled(true);
    }

    // Last Frame Button
    if(current_framenr + 1 == num_frames) {
        ui->lastFrameButton->setEnabled(false);
    }
    else{
        ui->lastFrameButton->setEnabled(true);
    }
}

void MainWindow::loadPreviousFrame() {
    int frameRate = ui->frameRateSpinBox->value();
    loadFrame(current_framenr-frameRate);
}

void MainWindow::loadFirstFrame() {
    loadFrame(0);
}

void MainWindow::loadLastFrame() {
    loadFrame(num_frames-1);
}

// https://stackoverflow.com/questions/35039946/get-mouse-position-in-child-qgraphicsscene
// https://doc.qt.io/qt-5/eventsandfilters.html -> "Event Filters"
bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    if (target == graphics_scene && event->type() == QEvent::GraphicsSceneMousePress) {
        const QGraphicsSceneMouseEvent* const mouse_event = static_cast<const QGraphicsSceneMouseEvent*>(event);

        // Right Mousebutton was clicked:
        if(mouse_event->button() == Qt::RightButton) {
            const QPointF position = mouse_event->scenePos();
            // std::cout << "Right Mousebutton:" << position.x() << "," << position.y() << std::endl;
            InstrumentPair &pair = instrumentPairs[current_framenr];
            pair.xRight = position.x();
            pair.yRight = position.y();
            pair.annotated = true;
            setRightInstrumentPos(position.x(), position.y());
        }

        //Left Mousebutton was clicked:
        if(mouse_event->button() == Qt::LeftButton) {
            QPointF const position = mouse_event->scenePos();
            // std::cout << "Left Mousebutton:" << position.x() << "," << position.y() << std::endl;
            InstrumentPair &pair = instrumentPairs[current_framenr];
            pair.xLeft = position.x();
            pair.yLeft = position.y();
            pair.annotated = true;
            setLeftInstrumentPos(position.x(), position.y());
        }
    }

    return QMainWindow::eventFilter(target, event);
}

void MainWindow::setLeftInstrumentPos(int x, int y) {
    if(left_ellipse != nullptr) {
        graphics_scene->removeItem(left_ellipse);
        left_ellipse = nullptr;
    }
    if(x != -1 && y != -1) {
        left_ellipse = graphics_scene->addEllipse(x - 5, y - 5, 10, 10, left_ellipse_pen);
    }
}

void MainWindow::setRightInstrumentPos(int x, int y) {
    if(right_ellipse != nullptr) {
        graphics_scene->removeItem(right_ellipse);
        right_ellipse = nullptr;
    }

    if(x != -1 && y != -1) {
        right_ellipse = graphics_scene->addEllipse(x - 5, y - 5, 10, 10, right_ellipse_pen);
    }
}

void MainWindow::readCSV() {
    QString read_csv_file = QFileDialog::getOpenFileName(this,
                                                         "Open Coordinates File", "/home/amelie/Uni/Arbeit/",
                                                         "CSV File (*.csv)");

    if (read_csv_file.isEmpty())
        return;
    else {
        QFile file(read_csv_file);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "Unable to open file",
                                     file.errorString());
            return;
        }

        instrumentPairs.clear(); // delete content of vector

        QTextStream in(&file);
        while(!in.atEnd()) {
            InstrumentPair pair;

            QString line = in.readLine();

            QStringList coordinates_instrument = line.split(";");

            pair.xLeft = coordinates_instrument[0].toInt();
            pair.yLeft = coordinates_instrument[1].toInt();
            pair.xRight = coordinates_instrument[2].toInt();
            pair.yRight = coordinates_instrument[3].toInt();
            if(coordinates_instrument.length() > 4) {
                pair.annotated = coordinates_instrument[4] != "0";
            }

            instrumentPairs.push_back(pair); // move pair element to end of vector ("append")
        }
        file.close();
    }
}

// https://doc.qt.io/qt-5/qtwidgets-tutorials-addressbook-part6-example.html
void MainWindow::saveCSV() {

    QString csv_file_name = QFileDialog::getSaveFileName(this, "Save CSV", "/home/amelie/Uni/Arbeit/", "CSV (*.csv)");

    // https://doc.qt.io/qt-5/qfiledialog.html
    //    QFileDialog dialog(this);
    //    dialog.setFileMode(QFileDialog::AnyFile);
    //    dialog.setNameFilter("CSV File (*.csv)");
    //    dialog.setDefaultSuffix("csv");
    //    QString csv_file_name;
    //    if (dialog.exec())
    //        csv_file_name = dialog.selectFile();


    if (csv_file_name.isEmpty())
        return;
    else {
        QFile file(csv_file_name);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream out(&file);
        for(auto const &pair : instrumentPairs){ // auto find type of pair, iterate over every element in instrumentPairs
            out << pair.xLeft << ";" << pair.yLeft << ";" << pair.xRight << ";" << pair.yRight << ";" << pair.annotated << "\n";

        }
    }
}

//void MainWindow::saveCSV(std::string const &filename) {

//    std::ofstream file(filename, std::ios::trunc); // open new file with filename, overwrites old file! ("truncate")

//    for(auto const &pair : instrumentPairs){ // auto find type of pair, iterate over every element in instrumentPairs
//        file << pair.xLeft << ";" << pair.yLeft << ";" << pair.xRight << ";" << pair.yRight << "\n";
//    }
//}
