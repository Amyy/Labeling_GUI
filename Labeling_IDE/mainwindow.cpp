#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <QGraphicsSceneMouseEvent>
#include <QDesktopServices>
#include <QDataStream>
#include <QFileDialog>

using namespace cv;

// namespace(hier==Klasse)::Funktion_aus_Namespace()
MainWindow::MainWindow(QWidget *parent) : // Initialisierungsliste
    QMainWindow(parent),
    ui(new Ui::MainWindow), // "ui = new Ui::MainWindow"
    left_ellipse(nullptr),
    right_ellipse(nullptr),
    pixmap_item(nullptr),
    current_framenr(0),
    num_frames(0)
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
    readCSV("/home/amelie/Uni/Arbeit/positions.csv");

    loadNextFrame();
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

void MainWindow::loadNextFrame() {
    Mat frame;
    current_framenr = static_cast<int>(video.get(CAP_PROP_POS_FRAMES));
    ui->frameSpinBox->setValue(current_framenr);

    video >> frame;
    if (!frame.empty())
    {
        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        pixmap_item->setPixmap(QPixmap::fromImage(qimg.rgbSwapped()));

        InstrumentPair const &pair = instrumentPairs[current_framenr]; //InstrumentPair refers to element in vector
        setLeftInstrumentPos(pair.xLeft, pair.yLeft);
        setRightInstrumentPos(pair.xRight, pair.yRight);
    }
}

void MainWindow::setFrame(int framenumber) {
    if(framenumber != current_framenr) { // prevent 2x/double loadNextFrame trigger
        video.set(CAP_PROP_POS_FRAMES, framenumber);
        loadNextFrame();
    }
}

void MainWindow::loadPreviousFrame() {
    setFrame(current_framenr-1); // current_framenr == -1 doesn't matter lol
}

void MainWindow::loadFirstFrame() {
    setFrame(0);
}

void MainWindow::loadLastFrame() {
    setFrame(num_frames-1);
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
            setRightInstrumentPos(position.x(), position.y());
        }

        //Left Mousebutton was clicked:
        if(mouse_event->button() == Qt::LeftButton) {
            QPointF const position = mouse_event->scenePos();
            // std::cout << "Left Mousebutton:" << position.x() << "," << position.y() << std::endl;
            InstrumentPair &pair = instrumentPairs[current_framenr];
            pair.xLeft = position.x();
            pair.yLeft = position.y();
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

void MainWindow::readCSV(std::string const &filename) {
    std::ifstream file(filename);

    instrumentPairs.clear(); // delete content of vector

    std::string line;
    while(std::getline(file, line)) { // get line out of file -> write in variable line
        InstrumentPair pair;

        std::stringstream ss(line); // fill stringstream with content of line
        ss >> pair.xLeft; // read line until non integer character (== ";") because xLeft == int
        ss.ignore(1); // ignore next single/one character
        ss >> pair.yLeft;
        ss.ignore(1);
        ss >> pair.xRight;
        ss.ignore(1);
        ss >> pair.yRight;

        instrumentPairs.push_back(pair); // move pair element to end of vector ("append")
    }
}

// https://doc.qt.io/qt-5/qtwidgets-tutorials-addressbook-part6-example.html
void MainWindow::saveCSV() {

    QString csv_file_name = QFileDialog::getSaveFileName(this, "Save CSV", "", "CSV (*.csv);;All Files (*)");

    if (csv_file_name.isEmpty())
           return;
       else {
           QFile file(csv_file_name);
           if (!file.open(QIODevice::WriteOnly)) {
               QMessageBox::information(this, tr("Unable to open file"),
                   file.errorString());
               return;
           }
       QDataStream out(&file);
       for(auto const &pair : instrumentPairs){ // auto find type of pair, iterate over every element in instrumentPairs
           std::cout << pair.xLeft << std::endl;
           out << pair.xLeft << ";" << pair.yLeft << ";" << pair.xRight << ";" << pair.yRight << "\n";

       }
     }
}

//void MainWindow::saveCSV(std::string const &filename) {

//    std::ofstream file(filename, std::ios::trunc); // open new file with filename, overwrites old file! ("truncate")

//    for(auto const &pair : instrumentPairs){ // auto find type of pair, iterate over every element in instrumentPairs
//        file << pair.xLeft << ";" << pair.yLeft << ";" << pair.xRight << ";" << pair.yRight << "\n";
//    }
//}
