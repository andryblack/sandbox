/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#ifndef SPLINEPAINT_H
#define SPLINEPAINT_H

#include <QWidget>

#include "sb_tcb_spline2d.h"

class SplinePaint : public QWidget
{
Q_OBJECT
public:
    explicit SplinePaint(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    sandbox::TCBSpline2d m_spline;
    int m_selected_point;
    bool m_paint_mode;
    bool m_paint;
    bool m_dash;
signals:

public slots:
    void clear();
    void setTension(float val);
    void setContinuity(float val);
    void setBias(float val);
    void setPaintMode(bool mode) { m_paint_mode = mode;m_paint=false;}
    void setDashMode(bool mode) { m_dash = mode;}
};

#endif // SPLINEPAINT_H
