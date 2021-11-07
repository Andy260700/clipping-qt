#include "mainwindow.h"
#include "ui_grid.h"
#include <iostream>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    lastClickedColor = Qt::black;
    ui->setupUi(this);
    ui->gridsize->setValue(ui->widget->getGap());
    connect(ui->widget, SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));
    connect(ui->widget, SIGNAL(mousePressed(QPoint&, QColor)),this,SLOT(showMousePressed(QPoint&, QColor)));
    connect(ui->widget, SIGNAL(sendTime(int)), SLOT(setTime(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_show_axes_clicked(){
    ui->widget->show_axes(ui->show_axes->isChecked());
}

void MainWindow::on_show_grid_clicked(){
    ui->widget->show_grid(ui->show_grid->isChecked());
}

void MainWindow::on_Draw_clicked(){
    if(!lastLine.isNull())
    {   clip_lines.push_back(lastLine);
        ui->widget->drawLine(lastLine);
    }

}

void MainWindow::showMousePosition(QPoint& pos){
    ui->mouse_movement->setText(QString::number(pos.x()) +", "+QString::number(pos.y()));
}

void MainWindow::showMousePressed(QPoint& pos, QColor color)
{
    ui->mouse_pressed->setText(QString::number(pos.x()) +", "+QString::number(pos.y()));
    lastClickedColor = color;
    lastLine.setP1(lastLine.p2());
    lastLine.setP2(pos);

   if(isTransformation)
    {
        if(!transformation.empty())
        {
            ui->widget->drawLine(QLine(pos, transformation.front()));
        }
        transformation.push_front(pos);
    }
}

void MainWindow::on_gridsize_valueChanged(int gap)
{
    ui->widget->setGap(gap);
}


void MainWindow::on_pushButton_pressed()
{
    lastLine.setLine(0, 0, 0, 0);
    lastClickedColor = Qt::black;

    ui->DDA_time->setText("0");
    ui->Bresenham_time->setText("0");
    this->transformation.clear();
    clip_lines.clear();
    ui->widget->clear();
}

void MainWindow::setTime(int time)
{
    if (ui->widget->line_drawn)
    {
        LineAlgorithm la = ui->widget->getLineAlgorithm();
        switch(la)
        {
        case LineAlgorithm::DDA:
            ui->DDA_time->setText(QString::number(time));
            break;

        case LineAlgorithm::BRESENHAM:
            ui->Bresenham_time->setText(QString::number(time));
            break;

        }
    }
}


void MainWindow::on_line_algorithm_currentIndexChanged(int index)
{
    ui->widget->setLineAlgo(LineAlgorithm(index));
}

void MainWindow::on_Transformation_polygon_clicked()
{
    if(!isTransformation) {
            ui->Transformation_polygon->setText("End Polygon");
            isTransformation = true;
        } else {
            ui->widget->drawLine(QLine(transformation.back(), transformation.front()));
            ui->widget->addTransformation(transformation);
            isTransformation = false;
            ui->Transformation_polygon->setText("Start Polygon");
            transformation.clear();
//            ui->widget->scanline(Qt::darkBlue);
        }
}






void MainWindow::on_Line_Clipping_clicked()
{
    ui->widget->clip_lines(clip_lines);
}


void MainWindow::on_Polygon_Clipping_clicked()
{
    ui->widget->polygon_clipping();
}


void MainWindow::on_draw_rectangle_clicked()
{
    double w = ui->width->value();
    double h = ui->height->value();
    QPoint lp = lastLine.p2(); //lower-left point.

//    qDebug()<<lp;

    ui->widget->clip_frame = QRect( QPoint(lp.x(), lp.y()+h), QPoint(lp.x()+w, lp.y()));
    ui->widget->draw_rectangle(lp,h,w);

}

