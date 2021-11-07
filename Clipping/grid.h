#ifndef GRID_H
#define GRID_H

#include <QWidget>
#include <QtGui>
#include <unordered_map>
#include "qpointhasher.h"
#include<QPoint>


enum class LineAlgorithm
{
    DDA = 0,
    BRESENHAM = 1
};

void delay(int msecs);

class grid : public QWidget
{
    Q_OBJECT
private:
    int m_gap = 10;
    int delayms = 10;

    std::unordered_map<QPoint, QColor, QPointHasher> pointsList;
    std::list<QPoint> transformations;
    std::list<QPoint> line_clipping_points;
    std::list<QLine> polygon_lines;
    //std::list<QPoint> rectangle_points; //to_do*/

    bool perform_trans = false;
    bool showGrid = true;
    bool showAxes = true;
    bool line_clip = false;
    bool polygon_clip = false;

    QTransform transform = QTransform::fromScale(1, 1);
    QPoint convertCoordinate(QPoint q) const;
    LineAlgorithm lineAlgo = LineAlgorithm::DDA;

public:
    bool line_drawn = false;
    QRect clip_frame; //Rectangle for line clipping.
    explicit grid(QWidget *parent = nullptr);
    ~grid();
    void drawPoint(int x, int y);
    void clear();
    int getGap() const;
    void setGap(int);
    void show_axes(bool);
    void show_grid(bool);
    void drawLine(QLine l);
    void setLineAlgo(LineAlgorithm);
    void clip_lines(std::list<QLine>);
    QColor getColor(QPoint& p);
    void addTransformation(std::list<QPoint> polygon);
    void dda_for_transform(std::list<QPoint> &,QLine const& l);
    void polygon_clipping();
    LineAlgorithm getLineAlgorithm();
    void draw_rectangle(QPoint,int,int);

protected:
    void paintEvent(QPaintEvent *) override;
    void setPixel(int x, int y, QColor color,QPainter& painter);
    void dda(QLine const&);
    void dda_time(QLine const&);
    void bresenham(QLine const& l);
    void bresenham_time(QLine const&l);
    void dda_for_line_clipping(QLine const& l);
    int get_point_code(QPoint);
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    std::pair<bool,QLine> get_clipped_points(QLine);
    void generate_polygon_lines();
    void left_clip();
    void right_clip();
    void top_clip();
    void bottom_clip();
signals:
    void sendMousePosition(QPoint&);
    void mousePressed(QPoint&, QColor);
    void sendTime(int time);
};

#endif // GRID_H
