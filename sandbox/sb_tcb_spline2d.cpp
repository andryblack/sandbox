/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#include "sb_tcb_spline2d.h"
#include "sb_bind.h"
#include <cassert>

namespace Sandbox {
    
    TCBSpline2d::TCBSpline2d()
    {
        m_length = 0.0f;
        m_tension = 0.0f;
        m_continuity = 0.0f;
        m_bias = 0.0f;
    }


    void TCBSpline2d::set_point(size_t i,const Vector2f& pos) {
        float len = i!=0 ? (pos - m_points[i-1].pos).length() : 0.0f;
        m_length -= m_points[i].len;
        m_points[i].pos = pos;
        m_points[i].len = len;
        m_length+=len;
        if ((i+2)==m_points.size())
            calc_derivatives(i+1);
        calc_derivatives(i);
        if (i==1)
            calc_derivatives(0);
    }

    Vector2f TCBSpline2d::get(float t) const {
        assert(!m_points.empty());
		if (m_points.size()==1) return m_points.front().pos;
        for (size_t i=1;i<m_points.size();i++) {
            if (t<m_points[i].len) {
                float k = t / m_points[i].len;
                return interpolate(m_points[i-1],m_points[i],k);
            }
            t-=m_points[i].len;
        }
        return m_points.back().pos;
    }

    void TCBSpline2d::calc_derivatives(size_t indx) {
        if (m_points.size()<2) return;
        const Vector2f& cur = m_points[indx].pos;
        if (indx>0) {
            const Vector2f& prev = m_points[indx-1].pos;
            if (indx<(m_points.size()-1)) {
                const Vector2f& next = m_points[indx+1].pos;
                const Vector2f g1 = (cur - prev) * (1.0f + m_bias);
                const Vector2f g2 = (next - cur) * (1.0f - m_bias);
                const Vector2f g3 = g2 - g1;
                m_points[indx].ra = (g1 + g3 * ( 0.5f * (1.0f + m_continuity))) * (1.0f - m_tension);
                m_points[indx].rb = (g1 + g3 * ( 0.5f * (1.0f - m_continuity))) * (1.0f - m_tension);
            } else {
                m_points[indx].ra = ((cur - prev)*1.5f - m_points[indx-1].rb*0.5f) * (1.0f - m_tension);
                m_points[indx].rb = cur - prev;
            }
        } else {
            const Vector2f& next = m_points[indx+1].pos;
            m_points[indx].ra = next - cur;
            m_points[indx].rb = ( (next - cur)*1.5f - m_points[indx+1].ra*0.5f) * (1.0f - m_tension);
        }
    }

    Vector2f TCBSpline2d::interpolate(const Point& p1,const Point& p2,float t) const {
        return p1.pos * (2.0f*t*t*t - 3.0f*t*t + 1.0f) + p1.rb * (t*t*t - 2.0f*t*t + t) +
                p2.pos * (-2.0f*t*t*t + 3.0f*t*t) + p2.ra * (t*t*t - t*t);
    }

    float TCBSpline2d::add(const Vector2f& point) {
        float len = m_points.empty() ? 0.0f : (m_points.back().pos-point).length();
        m_points.resize(m_points.size()+1);
        m_points.back().pos = point;
        m_points.back().len = len;
        if (m_points.size()>1)
            calc_derivatives(m_points.size()-2);
        calc_derivatives(m_points.size()-1);
        if (m_points.size()<=2)
            calc_derivatives(0);
        m_length+=len;
		return len;
    } 
    
    void   TCBSpline2d::erase_last() {
        if (m_points.size() < 3) return;
        m_length -= m_points.back().len;
        m_points.pop_back();
        calc_derivatives(m_points.size()-1);
    }

    void TCBSpline2d::clear() {
        m_points.clear();
        m_length = 0.0f;
    }

    void TCBSpline2d::recalc() {
        if (m_points.size()>1) {
            for (size_t i=0;i<m_points.size();i++)
                calc_derivatives(i);
            calc_derivatives(0);
        }
    }

    float TCBSpline2d::angle(float t) const {
        return (get(t+2.5f)-get(t-2.5f)).unit().dir();
    }
    
    void TCBSpline2d::Register( Lua* lua ) {
        SB_BIND_BEGIN_BIND
        {
			SB_BIND_BEGIN_SHARED_CLASS(Sandbox::TCBSpline2d)
            SB_BIND_BEGIN_METHODS
			SB_BIND_METHOD(Sandbox::TCBSpline2d,length,float())
            SB_BIND_METHOD(Sandbox::TCBSpline2d,size,size_t())
            SB_BIND_METHOD(Sandbox::TCBSpline2d,point,Vector2f(size_t))
            SB_BIND_METHOD(Sandbox::TCBSpline2d,set_point,void(size_t,Vector2f))
            SB_BIND_METHOD(Sandbox::TCBSpline2d,get,Vector2f(float))
            SB_BIND_METHOD(Sandbox::TCBSpline2d,add,float(Vector2f))
            SB_BIND_METHOD(Sandbox::TCBSpline2d,clear,void())
            SB_BIND_METHOD(Sandbox::TCBSpline2d,set_tension,void(float))
			SB_BIND_METHOD(Sandbox::TCBSpline2d,set_continuity,void(float))
			SB_BIND_METHOD(Sandbox::TCBSpline2d,set_bias,void(float))
            SB_BIND_METHOD(Sandbox::TCBSpline2d,angle,float(float))
            SB_BIND_METHOD(Sandbox::TCBSpline2d,empty,bool())
            SB_BIND_METHOD(Sandbox::TCBSpline2d,back,Vector2f())
            SB_BIND_METHOD(Sandbox::TCBSpline2d,front,Vector2f())
			SB_BIND_END_METHODS
			SB_BIND_END_CLASS
			SB_BIND(lua)
		}
        SB_BIND_END_BIND
    }
}
