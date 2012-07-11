/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#include "sb_tcb_spline2d.h"
#include "sb_assert.h"
#include "luabind/sb_luabind.h"
#include "sb_lua.h"

SB_META_DECLARE_KLASS(Sandbox::TCBSpline2d, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::TCBSpline2d)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(length)
SB_META_METHOD(size)
SB_META_METHOD(point)
SB_META_METHOD(set_point)
SB_META_METHOD(get)
SB_META_METHOD(add)
SB_META_METHOD(clear)
SB_META_METHOD(set_tension)
SB_META_METHOD(set_continuity)
SB_META_METHOD(set_bias)
SB_META_METHOD(angle)
SB_META_METHOD(empty)
SB_META_METHOD(back)
SB_META_METHOD(front)
SB_META_END_KLASS_BIND()


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
        sb_assert(!m_points.empty());
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
  
    void TCBSpline2d::Register( LuaVM* lua ) {
        luabind::Class<TCBSpline2d>(lua->GetVM());
    }
    
}
