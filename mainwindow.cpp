#include "mainwindow.h"
#include "ui_grid.h"
#include <iostream>
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gridsize->setValue(ui->widget->getGap());
    connect(ui->widget, SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));
    connect(ui->widget, SIGNAL(mousePressed(QPoint&)),this,SLOT(showMousePressed(QPoint&)));
    connect(ui->widget, SIGNAL(showTime(double&)), this, SLOT(showExTime(double&)));
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
    {
        QString a  = ui->widget->getAlgo();
        if(a=="DDA_Line"){
            ui->widget->dda(lastLine);
        }
        else{
            ui->widget->bresenham(lastLine.p1().x(), lastLine.p1().y(), lastLine.p2().x(), lastLine.p2().y());
        }
    }

}

void MainWindow::showExTime(double& time){
    ui->ex_time->setText(QString::number(time));
}

void MainWindow::showMousePosition(QPoint& pos){
    ui->mouse_movement->setText(QString::number(pos.x()) +", "+QString::number(pos.y()));
}

void MainWindow::showMousePressed(QPoint& pos){
    ui->mouse_pressed->setText(QString::number(pos.x()) +", "+QString::number(pos.y()));
    lastClicked=pos;
    lastLine.setP1(lastLine.p2());
    lastLine.setP2(lastClicked);
    if(isDrawingPolygon)
        {
            if(!poly.empty())
            {
                ui->widget->dda(QLine(pos, poly.front()));
            }
            poly.push_front(pos);
        }
}

void MainWindow::on_gridsize_valueChanged(int gap)
{
    ui->widget->setGap(gap);
}


void MainWindow::on_pushButton_pressed()
{
    ui->widget->clear();
}


void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    ui->widget->setAlgo(arg1);
}



void MainWindow::on_polygon_clicked()
{
    if(!isDrawingPolygon) {
            ui->polygon->setText("End Polygon");
            isDrawingPolygon = true;
        } else {

                    ui->widget->dda(QLine(poly.back(), poly.front()));
                    ui->widget->addPolygon(poly);
                    isDrawingPolygon = false;
                    ui->polygon->setText("Start Polygon");
                    poly.clear();
        }
}




void MainWindow::on_clip_window_clicked()
{
    double w = ui->width->value();
    double h = ui->height->value();
    ui->widget->bresenham(lastClicked.x(),lastClicked.y(), lastClicked.x()+w, lastClicked.y());
    ui->widget->bresenham(lastClicked.x()+w, lastClicked.y(), lastClicked.x()+w, lastClicked.y()+h);
    ui->widget->bresenham(lastClicked.x()+w, lastClicked.y()+h, lastClicked.x(), lastClicked.y()+h);
    ui->widget->bresenham(lastClicked.x(), lastClicked.y()+h, lastClicked.x(), lastClicked.y());

    ui->widget->clip_frame = QRect( QPoint(lastClicked.x(), lastClicked.y()+h), QPoint(lastClicked.x()+w, lastClicked.y()));
}


void MainWindow::on_Line_Clipping_clicked()
{
    ui->widget->clip_lines(clip_lines);
}


void MainWindow::on_Polygon_Clipping_clicked()
{
    ui->widget->polygon_clipping();
}

