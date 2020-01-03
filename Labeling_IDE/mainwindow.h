#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <opencv2/opencv.hpp>
#include <QPushButton>
#include <string>
#include <vector>

namespace Ui {
class MainWindow;
}

struct InstrumentPair {
    int xLeft;
    int yLeft;
    int xRight;
    int yRight;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0); // Konstruktor, hat nie Rückgabewert
    ~MainWindow(); // Destruktor

// function is called by emitted signal by PushButton "nextFrameButton"
public slots:
    void setFrame(int framenumber);
    void loadNextFrame();
    void loadPreviousFrame();
    void loadFirstFrame();
    void loadLastFrame();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *graphics_scene; // Membervariable

    // QPointF positions für rechts / linksklick hier erstellen

    bool eventFilter(QObject *target, QEvent *event);

    QGraphicsEllipseItem * left_ellipse;
    QPen left_ellipse_pen;

    QGraphicsEllipseItem * right_ellipse;
    QPen right_ellipse_pen;

    QGraphicsPixmapItem * item;

    cv::VideoCapture video;

    int current_framenr;
    int num_frames;

    void readCSV(std::string const &filename); // filename: reference to constant string
    std::vector<InstrumentPair> instrumentPairs; // besseres Array
};

#endif // MAINWINDOW_H
