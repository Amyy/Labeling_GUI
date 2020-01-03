#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <QMessageBox>

using namespace std;
using namespace cv;

// namespace(hier==Klasse)::Funktion_aus_Namespace()
MainWindow::MainWindow(QWidget *parent) : // Initialisierungsliste
    QMainWindow(parent),
    ui(new Ui::MainWindow), // "ui = new Ui::MainWindow"
    left_ellipse(NULL),
    right_ellipse(NULL)

{

    ui->setupUi(this);

    graphics_scene = new QGraphicsScene(this);

    //TODO: BeispielImage ersetzen durch wirkliche Frames
    //QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap("/home/amelie/Uni/NCT_Arbeit/instrument.png"));

    VideoCapture video("/home/amelie/Uni/Arbeit/PegTransfer.avi");

    if(!video.isOpened())
    {
        QMessageBox::critical(this,
                              "Camera Error",
                              "Make sure you entered a correct camera index,"
                              "<br>or that the camera is not being accessed by another program!");
        return;
    }

    Mat frame;

    video >> frame;
    if (!frame.empty())
    {
        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        //pixmap.setPixmap( QPixmap::fromImage(qimg.rgbSwapped()));

        QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(qimg.rgbSwapped()));
        graphics_scene->addItem(item);

    }




    // install event filter
    graphics_scene->installEventFilter(this);
    // install event filter for button
    // ...

    left_ellipse_pen.setColor(QColor(255, 0, 0));
    right_ellipse_pen.setColor(QColor(0, 255, 0));

    ui->graphicsView->setScene(graphics_scene); // graphicsView is QGraphicsView widget


}

// https://stackoverflow.com/questions/35039946/get-mouse-position-in-child-qgraphicsscene
// https://doc.qt.io/qt-5/eventsandfilters.html -> "Event Filters"
bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    if (target == graphics_scene && event->type() == QEvent::GraphicsSceneMousePress) {
        const QGraphicsSceneMouseEvent* const mouse_event = static_cast<const QGraphicsSceneMouseEvent*>(event);

        // Right Mousebutton was clicked:
        if(mouse_event->button() == Qt::RightButton) {


            const QPointF position = mouse_event->scenePos();
            cout << "Right Mousebutton:" << position.x() << "," << position.y() << endl;

            if(left_ellipse != NULL) {
                graphics_scene->removeItem(left_ellipse);
            }

            //Draw circle on clicked position:
            left_ellipse = graphics_scene->addEllipse(position.x() - 5, position.y() - 5, 10, 10, left_ellipse_pen);
        }

        //Left Mousebutton was clicked:
        if(mouse_event->button() == Qt::LeftButton) {

            const QPointF pos_right_click = mouse_event->scenePos();
            cout << "Left Mousebutton:" << pos_right_click.x() << "," << pos_right_click.y() << endl;

            if(right_ellipse != NULL) {
                graphics_scene->removeItem(right_ellipse);
            }

            // Draw circle on left-clicked position
            right_ellipse = graphics_scene->addEllipse(pos_right_click.x() - 5, pos_right_click.y() - 5, 10, 10, right_ellipse_pen);
        }
    }

    return QMainWindow::eventFilter(target, event);
}



MainWindow::~MainWindow()
{
    delete ui;
}

