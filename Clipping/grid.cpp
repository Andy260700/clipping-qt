#include "grid.h"
#include <iostream>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <algorithm>
#include<QPoint>

grid::grid(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void grid::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);


    if(showAxes)
    {
        QPen axesPen(Qt::darkGreen, m_gap);
        painter.setPen(axesPen);
        painter.drawLine(0, int(height()/(m_gap*2))*m_gap + m_gap/2, width(), int(height()/(m_gap*2))*m_gap+m_gap/2);
        painter.drawLine(int(width()/(m_gap*2))*m_gap + m_gap/2, 0 , int(width()/(m_gap*2))*m_gap + m_gap/2, height());
    }

    if(showGrid)
    {
        QPen gridpen(Qt::lightGray);
        painter.setPen(gridpen);
        for(int i = 0; i < height(); i += m_gap)
        {
            painter.drawLine(0, i, width(), i);
        }

        for(int i = 0; i < width(); i += m_gap) {
            painter.drawLine(i,0,i, height());
        }
    }

    if(polygon_clip)
    {
       std::list<QPoint> p_list;
       std::list<QPoint> rect_list;
       dda_for_transform(rect_list,QLine(clip_frame.bottomLeft(),clip_frame.topLeft()));
       dda_for_transform(rect_list,QLine(clip_frame.topLeft(),clip_frame.topRight()));
       dda_for_transform(rect_list,QLine(clip_frame.topRight(),clip_frame.bottomRight()));
       dda_for_transform(rect_list,QLine(clip_frame.bottomLeft(),clip_frame.bottomRight()));

       for(QLine l: polygon_lines)
           dda_for_transform(p_list,l);

       for(auto const& p:rect_list)
       {
           setPixel(p.x(), p.y(), Qt::green, painter);
       }

       for(auto const& p:p_list)
       {
           setPixel(p.x(), p.y(), Qt::darkBlue, painter);
       }
    }

    else if(line_clip)
    {
      for(auto const& p:line_clipping_points)
        {
            setPixel(p.x(), p.y(), Qt::darkBlue, painter);
        }


    }

    else if(perform_trans)
    {
        std::list<QPoint> p_list;
        std::list<QPoint> transformed_vertex;

        for(auto const& p:transformations)
        {
            transformed_vertex.push_back(p*transform);
        }

        QPoint a = transformed_vertex.front();
        transformed_vertex.push_back(a);
        int len = transformed_vertex.size();
        for(int i=0;i<len-1;i++)
        {
              QPoint p1 = transformed_vertex.front();
              transformed_vertex.pop_front();
              QPoint p2 = transformed_vertex.front();
              dda_for_transform(p_list,QLine(p1,p2));
        }

        for(auto const& p:p_list)
        {
            setPixel(p.x(), p.y(), Qt::green, painter);
        }

    }

    else
    {
        for(auto const& [p, color]: pointsList)
        {
            setPixel(p.x(), p.y(), color, painter);
        }
    }

    perform_trans = false;
    line_clip = false;
    polygon_clip = false;
}

void grid::dda(QLine const& l)
{
    int dx = l.p2().x() - l.p1().x();
    int dy = l.p2().y() - l.p1().y();

    int steps = std::max(abs(dx), abs(dy));

    double xinc = dx / (double) steps;
    double yinc = dy / (double) steps;


    double x = l.p1().x();
    double y = l.p1().y();
    for (int i = 0; i <= steps; i++)
    {
        pointsList[QPoint(round(x),round(y))] = Qt::green;
        x += xinc;
        y += yinc;
    }
}

void grid::dda_for_transform(std::list<QPoint>& p_list,QLine const& l)
{
    int dx = l.p2().x() - l.p1().x();
    int dy = l.p2().y() - l.p1().y();

    int steps = std::max(abs(dx), abs(dy));

    double xinc = dx / (double) steps;
    double yinc = dy / (double) steps;


    double x = l.p1().x();
    double y = l.p1().y();
    for (int i = 0; i <= steps; i++)
    {
        p_list.push_back(QPoint(x,y));
        x += xinc;
        y += yinc;
    }
}

void grid::bresenham(QLine const& l)
{
    int x1=l.p1().x();
    int y1=l.p1().y();

    int x2=l.p2().x();
    int y2=l.p2().y();

    int dx=x2-x1;
    int dy=y2-y1;

    int xinc=(dx>0)?1:-1;
    int yinc=(dy>0)?1:-1;

    dx=abs(dx);
    dy=abs(dy);

    if(dx>dy)
    {
        int p=2*(dy)-dx;
        int y=y1;

        for(int x=x1; x!=x2; x+=xinc)
        {
            pointsList[QPoint(x, y)] = Qt::green;
            if(p>=0)
            {
                y+=yinc;
                p-=2*dx;
            }
            p+=2*dy;
        }
    }
    else
    {
        int p=2*(dx)-dy;
        int x=x1;

        for(int y=y1; y!=y2; y+=yinc)
        {
            pointsList[QPoint(x, y)] = Qt::green;
            if(p>=0)
            {
                x+=xinc;
                p-=2*(dy);
            }
            p+=2*(dx);
        }
    }
}

void grid::dda_for_line_clipping(QLine const& l)
{
    int dx = l.p2().x() - l.p1().x();
    int dy = l.p2().y() - l.p1().y();

    int steps = std::max(abs(dx), abs(dy));

    double xinc = dx / (double) steps;
    double yinc = dy / (double) steps;


    double x = l.p1().x();
    double y = l.p1().y();
    for (int i = 0; i <= steps; i++)
    {
        line_clipping_points.push_back(QPoint(x,y));
        x += xinc;
        y += yinc;
    }
}

void delay(int msecs)
{
    QTime dieTime = QTime::currentTime().addMSecs(msecs);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

grid::~grid() {}

int grid::getGap() const
{
    return m_gap;
}
void grid::setGap(int gap)
{
    m_gap = gap;
    update();
}

void grid::setLineAlgo(LineAlgorithm a)
{
    lineAlgo = a;
}

void grid::setPixel(int x, int y, QColor color ,QPainter& painter)
{
    QBrush brush(color);
    int x1 = (int(width()/(m_gap*2))*m_gap  + x * m_gap);
    int y1 = (-y * m_gap + int(height()/(m_gap*2))*m_gap);
    painter.fillRect(x1, y1, m_gap, m_gap, brush);
}

void grid::drawPoint(int x, int y)
{
    pointsList[QPoint(x, y)] = Qt::green;
    update();
}
void grid::clear()
{
    pointsList.clear();
    emit sendTime(0);
    transform = QTransform::fromScale(1, 1);
    line_clipping_points.clear();
    transformations.clear();
    polygon_lines.clear();
    update();
}

void grid::show_axes(bool cond)
{
    showAxes = cond;
    update();
}
void grid::show_grid(bool cond)
{
    showGrid = cond;
    update();
}

void grid::addTransformation(std::list<QPoint> polygon)
{
    transformations = polygon;
}

void grid::bresenham_time(QLine const&l)
{
    QElapsedTimer timer;
    timer.start();
    int x1=l.p1().x();
    int y1=l.p1().y();

    int x2=l.p2().x();
    int y2=l.p2().y();

    int dx=x2-x1;
    int dy=y2-y1;

    int xinc=(dx>0)?1:-1;
    int yinc=(dy>0)?1:-1;

    dx=abs(dx);
    dy=abs(dy);

    if(dx>dy)
    {
        int p=2*(dy)-dx;
        int y=y1;

        for(int x=x1; x!=x2; x+=xinc)
        {
            //            pointsList.push_back(QPoint(x, y));
            if(p>=0)
            {
                y+=yinc;
                p-=2*dx;
            }
            p+=2*dy;
        }
    }
    else
    {
        int p=2*(dx)-dy;
        int x=x1;

        for(int y=y1; y!=y2; y+=yinc)
        {
            //            pointsList.push_back(QPoint(x, y));
            if(p>=0)
            {
                x+=xinc;
                p-=2*(dy);
            }
            p+=2*(dx);
        }
    }

    int time = timer.nsecsElapsed();
    emit sendTime(time);
}

void grid::dda_time(QLine const&l)
{
    QElapsedTimer timer;
    timer.start();
    int dx = l.p2().x() - l.p1().x();
    int dy = l.p2().y() - l.p1().y();

    int steps = std::max(abs(dx), abs(dy));

    double xinc = dx / (double) steps;
    double yinc = dy / (double) steps;


    double x = l.p1().x();
    double y = l.p1().y();
    for (int i = 0; i <= steps; i++)
    {
        //        pointsList.push_back(QPoint(round(x),round(y)));
        x += xinc;
        y += yinc;
    }
    int time = timer.nsecsElapsed();
    emit sendTime(time);
}

QPoint grid::convertCoordinate(QPoint q) const{
    int x = int(q.x())/m_gap - (width()/m_gap)/2;
    int y = -int(q.y())/m_gap + (height()/m_gap)/2;
    return QPoint(x,y);
}

void grid::drawLine(QLine line){
    line_drawn = true;
    switch(lineAlgo)
    {
    case LineAlgorithm::DDA:
        dda(line);
        dda_time(line);
        break;

    case LineAlgorithm::BRESENHAM:
        bresenham(line);
        bresenham_time(line);
    }
    update();
    line_drawn = false;
}

LineAlgorithm grid::getLineAlgorithm()
{
    return  this->lineAlgo;
}

QColor grid::getColor(QPoint& p)
{
    if(pointsList.find(p) == pointsList.end()) return Qt::black;
    return pointsList[p];
}

void grid::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint mouse_pos=ev->pos();
    if(mouse_pos.x()<=this->size().width() && mouse_pos.y()<=this->size().height()){
        if(mouse_pos.x()>0 && mouse_pos.y()>0){
            QPoint mp = convertCoordinate(mouse_pos);
            emit sendMousePosition(mp);
        }
    }
}

void grid::mousePressEvent(QMouseEvent *ev)
{
    QPoint mouse_pos=ev->pos();
    if(mouse_pos.x()<=this->size().width() && mouse_pos.y()<=this->size().height()){
        if(mouse_pos.x()>0 && mouse_pos.y()>0){
            QPoint mp = convertCoordinate(mouse_pos);
            QColor color = getColor(mp);
            drawPoint(mp.x(),mp.y());
            emit mousePressed(mp, color);
        }
    }
}

int grid::get_point_code(QPoint p)
{
    const int INSIDE = 0; // 0000
    const int LEFT = 1; // 0001
    const int RIGHT = 2; // 0010
    const int BOTTOM = 4; // 0100
    const int TOP = 8; // 1000

    int x = p.x();
    int y = p.y();
    int x_min = clip_frame.bottomLeft().x();
    int y_min = clip_frame.bottomLeft().y();
    int x_max = clip_frame.topRight().x();
    int y_max = clip_frame.topRight().y();

    int code = INSIDE;

    if (x < x_min) // to the left of rectangle
        code |= LEFT;
    else if (x > x_max) // to the right of rectangle
        code |= RIGHT;
    if (y < y_min) // below the rectangle
        code |= BOTTOM;
    else if (y > y_max) // above the rectangle
        code |= TOP;
    return code;
}

std::pair<bool,QLine> grid::get_clipped_points(QLine l)
{
    const int INSIDE = 0; // 0000
    const int LEFT = 1; // 0001
    const int RIGHT = 2; // 0010
    const int BOTTOM = 4; // 0100
    const int TOP = 8; // 1000

    int x_min = clip_frame.bottomLeft().x();
    int y_min = clip_frame.bottomLeft().y();
    int x_max = clip_frame.topRight().x();
    int y_max = clip_frame.topRight().y();

    int x1 = l.p1().x();
    int x2 = l.p2().x();
    int y1 = l.p1().y();
    int y2 = l.p2().y();

    // Compute region codes for P1, P2
    int code1 = get_point_code(l.p1());
    int code2 = get_point_code(l.p2());

    // Initialize line as outside the rectangular window
    bool accept = false;

    while (true) {
        if ((code1 == 0) && (code2 == 0)) {
            // If both endpoints lie within rectangle
            accept = true;
            qDebug()<<x1<<y1<<x2<<y2;
            break;
        }
        else if (code1 & code2) {
            // If both endpoints are outside rectangle,
            // in same region
            qDebug()<<x1<<y1<<x2<<y2;
            break;
        }
        else
        {
            // Some segment of line lies within the
            // rectangle
            int code_out;
            double x, y;

            // At least one endpoint is outside the
            // rectangle, pick it.
            if (code1 != 0)
                code_out = code1;
            else
                code_out = code2;

            // Find intersection point;
            // using formulas y = y1 + slope * (x - x1),
            // x = x1 + (1 / slope) * (y - y1)
            if (code_out & TOP) {
                // point is above the clip rectangle
                x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1);
                y = y_max;
            }
            else if (code_out & BOTTOM) {
                // point is below the rectangle
                x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1);
                y = y_min;
            }
            else if (code_out & RIGHT) {
                // point is to the right of rectangle
                y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
                x = x_max;
            }
            else if (code_out & LEFT) {
                // point is to the left of rectangle
                y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
                x = x_min;
            }

            // Now intersection point x, y is found
            // We replace point outside rectangle
            // by intersection point
            if (code_out == code1) {
                x1 = x;
                y1 = y;
                code1 = get_point_code(QPoint(x1,y1));
            }
            else {
                x2 = x;
                y2 = y;
                code2 = get_point_code(QPoint(x2, y2));
            }
            qDebug()<<x1<<y1<<x2<<y2;
        }
    }
    qDebug()<<"final: "<<x1<<y1<<x2<<y2;
    std::pair<bool, QLine> p = std::make_pair(accept,QLine(x1,y1,x2,y2));

    return p;
}

void grid::clip_lines(std::list<QLine> clip_lines)
{
    int len = clip_lines.size();

    for(int i=0;i<len;i++)
    {
        QLine l = clip_lines.front();
        clip_lines.pop_front();
        std::pair<bool,QLine> p = get_clipped_points(l);
        if (p.first)
        {
            clip_lines.push_back(p.second);
        }
    }

    pointsList.clear();
    for(QLine l:clip_lines)
    {
        dda_for_line_clipping(l);
        dda_for_line_clipping(QLine(clip_frame.bottomLeft(),clip_frame.bottomRight()));
        dda_for_line_clipping(QLine(clip_frame.bottomLeft(),clip_frame.topLeft()));
        dda_for_line_clipping(QLine(clip_frame.topLeft(),clip_frame.topRight()));
        dda_for_line_clipping(QLine(clip_frame.topRight(),clip_frame.bottomRight()));
    }
    line_clip = true;
    update();
}

void grid::generate_polygon_lines()
{
    QPoint a = transformations.front();
    transformations.push_back(a);
    int len = transformations.size();

    for(int i=0;i<len-1;i++)
    {
          QPoint p1 = transformations.front();
          transformations.pop_front();
          QPoint p2 = transformations.front();
          polygon_lines.push_back(QLine(p1,p2));
//          transformations.push_back(p1);
    }
    transformations.pop_front();
}

void grid::left_clip()
{

    int len = polygon_lines.size();
    for(int i=0;i<len;i++)
    {
        QLine l = polygon_lines.front();
        polygon_lines.pop_front();
        int x1 = l.p1().x();
        int x2 = l.p2().x();
        int xmin = clip_frame.bottomLeft().x();
        qDebug()<<"line: "<<l<<"\n";

        if(x1 >= xmin && x2 >= xmin)
        {
//            transformations.push_back(l.p1());
            transformations.push_back(l.p2());
        }

        else if(x1 < xmin && x2>=xmin)
        {
           int x = xmin;
           int y = round(l.p1().y()+((l.dy()/(double)l.dx())*(xmin-x1)));

           qDebug()<<"out-in: "<<x<<" "<<y<<"\n";
           transformations.push_back(QPoint(x,y));
           transformations.push_back(l.p2());
        }

        else if(x1 >= xmin && x2 < xmin)
        {
           int x = xmin;
           int y = round(l.p2().y()+((l.dy()/(double)l.dx())*(xmin-x2)));

//           transformations.push_back(l.p1());
           transformations.push_back(QPoint(x,y));
           qDebug()<<"in-out: "<<x<<" "<<y<<"\n";
        }
    }

//    qDebug()<<"after left clip: \n";
//    for (QPoint p:transformations)
//    {
//        qDebug()<<p<<"\n";
//    }
}

void grid::top_clip()
{

    int len = polygon_lines.size();
    for(int i=0;i<len;i++)
    {
        QLine l = polygon_lines.front();
        polygon_lines.pop_front();
        int y1 = l.p1().y();
        int y2 = l.p2().y();
        int ymax = clip_frame.topLeft().y();

        if(y1 <= ymax && y2 <= ymax) //in-in
        {
//            transformations.push_back(l.p1());
            transformations.push_back(l.p2());
        }

        else if(y1 > ymax && y2 <= ymax) //out-in
        {
           int y = ymax;
           int x = round(l.p1().x()+((l.dx()/(double) l.dy()))*(ymax-y1));

           transformations.push_back(QPoint(x,y));
           transformations.push_back(l.p2());
        }

        else if(y1 <= ymax && y2 > ymax) //in-out
        {
           int y = ymax;
           int x = round(l.p2().x()+((double) (l.dx()/(double) l.dy()))*(ymax-y2));

//           transformations.push_back(l.p1());
           transformations.push_back(QPoint(x,y));
        }
    }
}

void grid::right_clip()
{

    int len = polygon_lines.size();
    for(int i=0;i<len;i++)
    {
        QLine l = polygon_lines.front();
        polygon_lines.pop_front();
        int x1 = l.p1().x();
        int x2 = l.p2().x();
        int xmax = clip_frame.bottomRight().x();

        if(x1 <= xmax && x2 <= xmax)
        {
//            transformations.push_back(l.p1());
            transformations.push_back(l.p2());
        }

        else if(x1 > xmax && x2<=xmax)
        {
           int x = xmax;
           int y = round(l.p1().y()+((double) (l.dy()/(double) l.dx()))*(xmax-x1));

           transformations.push_back(QPoint(x,y));
           transformations.push_back(l.p2());
        }

        else if(x1 <= xmax && x2 > xmax)
        {
           int x = xmax;
           int y = round(l.p2().y()+((double) (l.dy()/(double) l.dx()))*(xmax-x2));

//           transformations.push_back(l.p1());
           transformations.push_back(QPoint(x,y));
        }
    }
}

void grid::bottom_clip()
{

    int len = polygon_lines.size();
    for(int i=0;i<len;i++)
    {
        QLine l = polygon_lines.front();
        polygon_lines.pop_front();
        int y1 = l.p1().y();
        int y2 = l.p2().y();
        int ymin = clip_frame.bottomLeft().y();

        if(y1 >= ymin && y2 >= ymin)
        {
//            transformations.push_back(l.p1());
            transformations.push_back(l.p2());
        }

        else if(y1 < ymin && y2 >= ymin)
        {
           int y = ymin;
           int x = round(l.p1().x()+((double) (l.dx()/(double) l.dy()))*(ymin-y1));

           transformations.push_back(QPoint(x,y));
           transformations.push_back(l.p2());
        }

        else if(y1 >= ymin && y2 < ymin)
        {
           int y = ymin;
           int x = round(l.p2().x()+((double) (l.dx()/(double) l.dy()))*(ymin-y2));

//           transformations.push_back(l.p1());
           transformations.push_back(QPoint(x,y));
        }
    }
}


void grid::draw_rectangle(QPoint lp,int h,int w)
{
    drawLine(QLine(lp.x(),lp.y(),lp.x()+w,lp.y()) ); //lower_horizontal
    drawLine(QLine(lp.x(),lp.y(),lp.x(),lp.y()+h) ); //left-vertical
    drawLine(QLine(lp.x(),lp.y()+h,lp.x()+w,lp.y()+h) ); //upper horizontal
    drawLine(QLine(lp.x()+w,lp.y(),lp.x()+w,lp.y()+h) ); //right vertical
}

void grid::polygon_clipping()
{
//    qDebug()<<"Initially: \n";
//    for (QPoint p:transformations)
//    {
//        qDebug()<<p<<"\n";
//    }
    generate_polygon_lines();
//    qDebug()<<"Initially: \n";
//    for (QLine p:polygon_lines)
//    {
//        qDebug()<<p<<"\n";
//    }
    left_clip();
    generate_polygon_lines();
//    qDebug()<<"After left clip : \n";
//    for (QLine p:polygon_lines)
//    {
//        qDebug()<<p<<"\n";
//    }
    top_clip();
    generate_polygon_lines();
    right_clip();
    generate_polygon_lines();
    bottom_clip();
    generate_polygon_lines();
    polygon_clip = true;
    update();

//    clip_lines(polygon_lines);
}
