#ifndef GRID_H
#define GRID_H

#include <QWidget>
#include <QtGui>
#include <list>
#include<unordered_map>
#include "qpointhasher.h"

using namespace std;

class grid : public QWidget
{
    Q_OBJECT
private:
    int m_gap = 10;
//  list<QPoint> pointsList;
    unordered_map<QPoint, QColor, QPointHasher> pointsList;
    list<QLine> lineList;
    list<QPoint> flood_fill;
    list<list<QPoint>> polygons;
    std::list<QPoint> transformations;
    bool isTransformation = false;
    bool showGrid = false;
    bool showAxes = false;
    bool line_clip = false;
    bool polygon_clip = false;
    QString algo;
    QPoint convertCoordinate(QPoint q) const;
    double ex_time=0;
    std::list<QPoint> line_clipping_points;
    std::list<QLine> polygon_lines;
public:
    QRect clip_frame;
    explicit grid(QWidget *parent = nullptr);
    ~grid();
    void drawPoint(int x, int y, QColor color = QColor(Qt::yellow));
    void clear();
    int getGap() const;
    int getRadius() const;
    int getSminor() const;
    int getSmajor() const;
    void setGap(int);
    void show_axes(bool);
    void show_grid(bool);
    void addLine(QLine l);
    void setAlgo(const QString &arg1);
    void setTime(double t);
    QString getAlgo();
    void addFill(QPoint p);
    void clip_lines(std::list<QLine>);
    void addPolygon(list<QPoint> polygon);
    void dda_for_transform(std::list<QPoint> &,QLine const& l);
    void polygon_clipping();
    void bresenham(int x1, int y1, int x2, int y2 );
    void dda(QLine const& l );
    void dda(QLine const&, list<QPoint>&) const;
    void dda_for_line_clipping(QLine const& l);
    int get_point_code(QPoint);

protected:
    void paintEvent(QPaintEvent *) override;
    void setPixel(int x, int y, QPainter& painter ,QBrush brush=QBrush(Qt::yellow));
    QColor getPixel(int x, int y);
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
    void mousePressed(QPoint&);
    void showTime(double&);
};

#endif // GRID_H
