/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

private slots:
    void on_checkBox_2_toggled(bool checked);
    void on_checkBox_toggled(bool checked);
    void on_pushButtonClear_clicked();
    void on_horizontalSliderBias_valueChanged(int value);
    void on_horizontalSliderContinuity_valueChanged(int value);
    void on_horizontalSliderTension_valueChanged(int value);
};

#endif // MAINWINDOW_H
