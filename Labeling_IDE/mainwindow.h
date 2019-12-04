#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0); // Konstruktor, hat nie Rückgabewert
    ~MainWindow(); // Destruktor

private:
    Ui::MainWindow *ui;
    QGraphicsScene *graphics_scene; // Membervariable
    // Button zum speichern
    // QPointF positions für rechts / linksklick hier erstellen

    bool eventFilter(QObject *target, QEvent *event);
};

#endif // MAINWINDOW_H
