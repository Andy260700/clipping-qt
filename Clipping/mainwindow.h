#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <list>

//using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLine lastLine;
    std::list<QLine> clip_lines;
    QColor lastClickedColor;
    bool isTransformation = false;
    std::list<QPoint> transformation;

public slots:
    void showMousePosition(QPoint& pos);
    void showMousePressed(QPoint& pos, QColor);
    void setTime(int time);
private slots:
    void on_show_axes_clicked();
    void on_show_grid_clicked();
    void on_Draw_clicked();
    void on_gridsize_valueChanged(int arg1);
    void on_pushButton_pressed();
    void on_line_algorithm_currentIndexChanged(int index);

    void on_Transformation_polygon_clicked();  

    void on_Line_Clipping_clicked();
    void on_Polygon_Clipping_clicked();
    void on_draw_rectangle_clicked();
};
#endif // MAINWINDOW_H
