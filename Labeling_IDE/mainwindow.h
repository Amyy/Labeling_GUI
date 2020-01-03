#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <opencv2/opencv.hpp>
#include <QPushButton>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0); // Konstruktor, hat nie Rückgabewert
    ~MainWindow(); // Destruktor

// function is called by emitted signal by PushButton "nextFrameButton"
public slots:
    bool loadNextFrame();
    bool loadPreviousFrame();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *graphics_scene; // Membervariable

    // Button zum speichern
    //QPushButton nextFrameButton;

    // QPointF positions für rechts / linksklick hier erstellen

    bool eventFilter(QObject *target, QEvent *event);

    QGraphicsEllipseItem * left_ellipse;
    QPen left_ellipse_pen;

    QGraphicsEllipseItem * right_ellipse;
    QPen right_ellipse_pen;

    QGraphicsPixmapItem * item;

    cv::VideoCapture video;

    double current_framenr;

};

#endif // MAINWINDOW_H
