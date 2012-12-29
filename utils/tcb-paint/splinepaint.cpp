/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#include "splinepaint.h"
#include <QPainter>
#include <QPaintEvent>

SplinePaint::SplinePaint(QWidget *parent) :
    QWidget(parent)
{
    m_selected_point = -1;
    m_paint_mode = false;
    m_paint = false;
    m_dash = false;
}



void SplinePaint::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(),QColor(0,0,0));
    painter.setRenderHint(QPainter::Antialiasing,true);
    if (m_spline.size()>1) {
        painter.setPen(QPen(QBrush(QColor(255,255,255)),1.0f));
        sandbox::Vector2d prev = m_spline.get(0.0f);
        if (m_paint_mode)  {
             {
                for (float t=2.0f;t<=m_spline.length();t+=2.0f) {
                    sandbox::Vector2d pos = m_spline.get(t);
                    painter.drawLine(prev.x,prev.y,pos.x,pos.y);
                    prev = pos;
                }

            }
        } else {
            for (float t=5.0f;t<=m_spline.length();t+=5.0f) {
                sandbox::Vector2d pos = m_spline.get(t);
                painter.drawLine(prev.x,prev.y,pos.x,pos.y);
                prev = pos;
            }
        }
        if (m_dash) {
            painter.setPen(QPen(QBrush(QColor(255,0,0)),1.0f));
            for (float t=7.0f;t<=m_spline.length();t+=14.0f) {
                sandbox::Vector2d pos = m_spline.get(t);
                sandbox::Vector2d dir = sandbox::Vector2d(0,-1).rotate(m_spline.angle(t));
                prev = pos - dir * 5.0f;
                pos = pos + dir*5.0f;
                painter.drawLine(prev.x,prev.y,pos.x,pos.y);
             }
        }
    }
    if (!m_paint_mode) {
        painter.setPen(QPen(QBrush(QColor(255,0,255)),5.0f));
        for (size_t i=0;i<m_spline.size();i++) {
            if (int(i)!=m_selected_point) {
                sandbox::Vector2d pos = m_spline.point(i);
                painter.drawPoint(pos.x,pos.y);
            }
        }
        if (m_selected_point>=0) {
            painter.setPen(QPen(QBrush(QColor(255,0,0)),5.0f));
            sandbox::Vector2d pos = m_spline.point(m_selected_point);
            painter.drawPoint(pos.x,pos.y);
        }
    }
}

void SplinePaint::mousePressEvent(QMouseEvent * event) {
    sandbox::Vector2d pos = sandbox::Vector2d(event->localPos().x(),event->localPos().y());
    if (m_paint_mode) {
        m_spline.clear();
        m_spline.add(pos);
        m_paint = true;
    } else {
        for (size_t i=0;i<m_spline.size();i++) {
            if ((pos-m_spline.point(i)).length()<10.0f) {
                m_selected_point = i;
                break;
            }
        }
        if (m_selected_point<0) {
            m_spline.add(pos);
            m_selected_point = m_spline.size()-1;
        }
    }
    repaint();
}

void SplinePaint::mouseReleaseEvent(QMouseEvent *){

    m_selected_point=-1;
    m_paint = false;
    repaint();
}

void  SplinePaint::mouseMoveEvent(QMouseEvent * event) {
    sandbox::Vector2d pos = sandbox::Vector2d(event->localPos().x(),event->localPos().y());
    if (m_paint_mode) {
        m_spline.add(pos);
        repaint();
    } else {
        if (m_selected_point>=0) {
            m_spline.set_point(m_selected_point,pos);
            repaint();
        }
    }
}

void SplinePaint::clear() {
    m_spline.clear();
    m_paint = false;
    m_selected_point = -1;
    repaint();
}

void SplinePaint::setTension(float val) {
    m_spline.set_tension(val);
     repaint();
}

void SplinePaint::setContinuity(float val) {
    m_spline.set_continuity(val);
    repaint();
}

void SplinePaint::setBias(float val) {
    m_spline.set_bias(val);
    repaint();
}
