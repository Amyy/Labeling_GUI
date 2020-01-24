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

    bool annotated{false};
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0); // Konstruktor, hat nie Rückgabewert
    ~MainWindow(); // Destruktor

// function is called by emitted signal by Buttons
public slots:
    void loadFrame(int framenumber); // gets called by frameSpinBox
    void loadNextFrame();
    void loadPreviousFrame();
    void loadFirstFrame();
    void loadLastFrame();
    void saveCSV();
    void readCSV();
    void setFramerate(int framerate);


private:
    void loadVideo(std::string const &filename);
    bool eventFilter(QObject *target, QEvent *event);
    void setLeftInstrumentPos(int x, int y);
    void setRightInstrumentPos(int x, int y);
    void refreshButtonState();

    Ui::MainWindow *ui;
    QGraphicsScene *graphics_scene;

    QPen left_ellipse_pen;
    QPen right_ellipse_pen;

    QGraphicsEllipseItem * left_ellipse;
    QGraphicsEllipseItem * right_ellipse;

    QGraphicsPixmapItem * pixmap_item;

    cv::VideoCapture video;
    int current_framenr;
    int num_frames;
    int framerate;

    std::vector<InstrumentPair> instrumentPairs;
};

#endif // MAINWINDOW_H
