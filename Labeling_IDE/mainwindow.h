#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
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
    void loadVideo(std::string const &filename);
    void readCSV(std::string const &filename); // filename: reference to constant string
    void saveCSV(std::string const &filename);
    bool eventFilter(QObject *target, QEvent *event);
    void setLeftInstrumentPos(int x, int y);
    void setRightInstrumentPos(int x, int y);

    Ui::MainWindow *ui;
    QGraphicsScene *graphics_scene; // Membervariable

    QPen left_ellipse_pen;
    QPen right_ellipse_pen;

    QGraphicsEllipseItem * left_ellipse;
    QGraphicsEllipseItem * right_ellipse;

    QGraphicsPixmapItem * pixmap_item;

    cv::VideoCapture video;
    int current_framenr;
    int num_frames;

    std::vector<InstrumentPair> instrumentPairs; // besseres Array
};

#endif // MAINWINDOW_H
