/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#ifndef SB_TCB_SPLINE2D_H
#define SB_TCB_SPLINE2D_H

#include "sb_vector2.h"
#include <sbtl/sb_shared_ptr.h>
#include <sbtl/sb_vector.h>


namespace Sandbox {

    class LuaVM;

    class TCBSpline2d
    {
    private:
        struct Point {
            Vector2f pos;
            float   len;
            Vector2f ra;
            Vector2f rb;
        };
        sb::vector<Point> m_points;
        float m_length;
        void calc_derivatives(size_t indx);
        float m_tension;
        float m_continuity;
        float m_bias;
        Vector2f interpolate(const Point& p1,const Point& p2,float t) const;
        void recalc();
    public:
        TCBSpline2d();

        float length() const { return m_length;}
        size_t size() const { return m_points.size();}

        const Vector2f& point(size_t i) const { return m_points[i].pos;}
        float poin_length(size_t i) const { return m_points[i].len; }
        void set_point(size_t i,const Vector2f& pos);

        Vector2f get(float t) const;
        float add(const Vector2f& point);
        void   erase_last();

        void clear();

        void set_tension(float value) { m_tension=value;recalc();}
        void set_continuity(float value) { m_continuity=value;recalc();}
        void set_bias(float value) { m_bias = value;recalc();}

        float angle(float t) const;
		
		bool empty() const { return m_points.empty();}
		const Vector2f& back() const { return m_points.back().pos;}
		const Vector2f& front() const { return m_points.front().pos;}
        
        static void Register( LuaVM* lua );
    };
    
    typedef sb::shared_ptr<TCBSpline2d> TCBSpline2dPtr;
}
#endif // SB_TCB_SPLINE2D_H
