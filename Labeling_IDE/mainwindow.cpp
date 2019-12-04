#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

using namespace std;

// namespace(hier==Klasse)::Funktion_aus_Namespace()
MainWindow::MainWindow(QWidget *parent) : // Initialisierungsliste
    QMainWindow(parent),
    ui(new Ui::MainWindow) // "ui = new Ui::MainWindow"
{

    ui->setupUi(this);


    // add image to graphics_scene & show image in graphicsView widget
    graphics_scene = new QGraphicsScene(this);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap("/home/amelie/Uni/NCT_Arbeit/instrument.png"));
    graphics_scene->addItem(item);

    // install event filter
    graphics_scene->installEventFilter(this);
    // install event filter for button

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
            }

            //Left Mousebutton was clicked:
            if(mouse_event->button() == Qt::LeftButton) {
                const QPointF pos_right_click = mouse_event->scenePos();
                cout << "Left Mousebutton:" << pos_right_click.x() << "," << pos_right_click.y() << endl;
            }

        }


    return QMainWindow::eventFilter(target, event);
}



MainWindow::~MainWindow()
{
    delete ui;
}

