/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_horizontalSliderTension_valueChanged(int value)
{
    ui->lineEditTension->setText(QString().number(0.1f*value));
    ui->widget->setTension(0.1f*value);
}

void MainWindow::on_horizontalSliderContinuity_valueChanged(int value)
{
    ui->lineEditContinuity->setText(QString().number(0.1f*value));
    ui->widget->setContinuity(0.1f*value);
}

void MainWindow::on_horizontalSliderBias_valueChanged(int value)
{
    ui->lineEditBias->setText(QString().number(0.1f*value));
    ui->widget->setBias(0.1f*value);
}

void MainWindow::on_pushButtonClear_clicked()
{
    ui->widget->clear();
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    ui->widget->setPaintMode(checked);
}

void MainWindow::on_checkBox_2_toggled(bool checked)
{
    ui->widget->setDashMode(checked);
}
