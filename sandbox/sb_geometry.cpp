//
//  sb_geometry.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 06/02/14.
//
//

#include "sb_geometry.h"

namespace Sandbox {
    
    static void build_line_segment(){
        
    }
    
    struct BuildContext {
        GeometryData* buffer;
        GHL::UInt32 color;
        float z;
        GHL::UInt16 ibase;
        void add_vertex(const Sandbox::Vector2f& p,float tx,float ty) {
            size_t pos = buffer->vertexes.size();
            buffer->vertexes.resize(pos+1);
            GHL::Vertex* vertex = &buffer->vertexes[pos];
            *reinterpret_cast<GHL::UInt32*>(&(vertex->color)) = color;
            vertex->x = p.x;
            vertex->y = p.y;
            vertex->z = z;
            vertex->tx = tx;
            vertex->ty = ty;
        }
        void add_triangle(GHL::UInt16 i1,GHL::UInt16 i2,GHL::UInt16 i3) {
            size_t pos = buffer->indexes.size();
            buffer->indexes.resize(pos+3);
            GHL::UInt16* indices = &buffer->indexes[pos];
            *indices++=ibase+i1;
            *indices++=ibase+i2;
            *indices++=ibase+i3;
        }

    };
    struct BuildLineContext : BuildContext {
        float W;
        float tx;
        float ty;
        float th;
    };
    
    static void build_corner( const Sandbox::Vector2f& p,
							 const Sandbox::Vector2f& dir_l,
							 const Sandbox::Vector2f& dir_r,
							 BuildLineContext* buffer) {
        float median_len = buffer->W / std::sqrt( ( 1.0f - (-dir_l).dot(dir_r) ) / 2.0f );
        Sandbox::Vector2f median_dir = (dir_r-dir_l).unit();
        Sandbox::Vector2f median = median_dir * median_len;
        float offset = median.dot(-dir_l);
		
        Sandbox::Vector2f inner_pnt = p + median;
        Sandbox::Vector2f outer_pnt = p - median_dir * buffer->W;
		
        Sandbox::Vector2f mid_l = p - dir_l * offset;
        Sandbox::Vector2f mid_r = p + dir_r * offset;
		
        float cw = (dir_r.normal().dot(-dir_l) > 0 ? 1.0f : -1.0f);
	    
        Sandbox::Vector2f normal_l = dir_l.normal()*buffer->W;
        Sandbox::Vector2f normal_r = dir_r.normal()*buffer->W;
		
        Sandbox::Vector2f out_m_l = p - normal_l * cw;
        Sandbox::Vector2f out_m_r = p - normal_r * cw;
		
        Sandbox::Vector2f out_l = mid_l - normal_l * cw;
        Sandbox::Vector2f out_r = mid_r - normal_r * cw;
		
        if (cw > 0) {
			
            
            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
            buffer->add_vertex(out_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(p,buffer->tx,buffer->ty);
            buffer->add_vertex(out_m_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(outer_pnt,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(out_m_r,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(out_r,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
			
            buffer->add_triangle(0,1,2);
            buffer->add_triangle(2,1,3);
            buffer->add_triangle(3,4,2);
            buffer->add_triangle(2,4,5);
            buffer->add_triangle(5,6,2);
            buffer->add_triangle(6,7,2);
			
        } else {
			
            buffer->add_vertex(out_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
            buffer->add_vertex(p,buffer->tx,buffer->ty);
            buffer->add_vertex(out_m_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(outer_pnt,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(out_m_r,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
            buffer->add_vertex(out_r,buffer->tx,buffer->ty-buffer->th*cw);
			
            buffer->add_triangle(1,0,2);
            buffer->add_triangle(2,0,3);
            buffer->add_triangle(3,4,2);
            buffer->add_triangle(2,4,5);
            buffer->add_triangle(5,7,2);
            buffer->add_triangle(7,6,2);
        }
    }
    static const size_t corner_vertexes = 8;
    static const size_t corner_indexes = 6 * 3;
	
    static Sandbox::Vector2f get_dir_next(const std::vector<Sandbox::Vector2f>& points,size_t i)  {
        const size_t size = points.size();
        if (i<=(size-2)) {
            return (points[i+1]-points[i]).unit();
        }
        return (points[size-1]-points[size-2]).unit();
    }
    
    void GeometryBuilder::BuildLine(GeometryData& buffer,const sb::vector<Vector2f>& points,const Image& img,const Color& color) {
        if (points.size()<2) return;
        
        buffer.primitives = GHL::PRIMITIVE_TYPE_TRIANGLES;
        buffer.texture = img.GetTexture();
        BuildLineContext ctx;
        ctx.buffer = &buffer;
        ctx.color = color.hw();
        ctx.ibase = 0;
        ctx.z = 0;
        
        size_t vtcs = 8 + (points.size()-1)*corner_vertexes;
		size_t indxs = 6 * 3 + (points.size()-1)*(corner_indexes+2*3);
		buffer.vertexes.reserve(vtcs);
		buffer.indexes.reserve(indxs);
        
        float width = img.GetHeight();
        width*=0.5f;
        ctx.W = width;
        
        float tx = img.GetTextureX() / img.GetTexture()->GetWidth();
        float ty = img.GetTextureY() / img.GetTexture()->GetHeight();
        float tw = img.GetTextureW() / img.GetTexture()->GetWidth();
        float th = img.GetTextureH() / img.GetTexture()->GetHeight();
        
        
        ctx.tx = tx + tw*0.5f;
        ctx.ty = ty + th*0.5f;
        ctx.th = th*0.5f;
        
        sb_assert(points.size()>=2);
		Sandbox::Vector2f dir_l = (points[1]-points[0]).unit();
        
        ctx.add_vertex(points[0]-dir_l*img.GetWidth()*0.5f+dir_l.normal()*width,tx,ty+th);  // 0
        ctx.add_vertex(points[0]-dir_l*img.GetWidth()*0.5f-dir_l.normal()*width,tx,ty);     // 1
        
        ctx.add_vertex(points[0]+dir_l.normal()*width,tx+tw*0.5f,ty+th);                    // 2
        ctx.add_vertex(points[0]-dir_l.normal()*width,tx+tw*0.5f,ty);                       // 3
		
//     (1)---(3)---(5)--
//      |    /|    /|
//      |   / |p0 / |
//      |  /  *  /  |
//      | /   | /   |
//      |/    |/    |
//     (0)---(2)---(4)--
//        
        /// left dot
        ctx.add_triangle(0,3,1);
        ctx.add_triangle(0,2,3);
		
        /// first segment
        ctx.add_triangle(2,5,3);
        ctx.add_triangle(2,4,5);
		
        ctx.ibase+=4;
		
        for (size_t i=1;i<(points.size()-1);i++) {
			Sandbox::Vector2f dir_r = get_dir_next(points,i);
            build_corner(points[i],dir_l,dir_r,&ctx);
            ctx.ibase+=corner_vertexes-2;
            ctx.add_triangle(0,2,1);
            ctx.add_triangle(0,2,3);
            ctx.ibase+=2;
            dir_l = dir_r;
        }
        dir_l = get_dir_next(points,points.size()-1);
        
        //ctx.add_vertex(points[0]+dir_l.normal()*width,tx+tw*0.5f,ty+th);
        //ctx.add_vertex(points[0]-dir_l.normal()*width,tx+tw*0.5f,ty);
		
        ctx.add_vertex(points.back()+dir_l.normal()*width,tx+tw*0.5f,ty+th);
        ctx.add_vertex(points.back()-dir_l.normal()*width,tx+tw*0.5f,ty);
        //buffer.ibase-=2;
        ctx.add_vertex(points.back()+dir_l*img.GetWidth()*0.5f+dir_l.normal()*width,tx+tw,ty+th);
        ctx.add_vertex(points.back()+dir_l*img.GetWidth()*0.5f-dir_l.normal()*width,tx+tw,ty);
        /// right dot
        ctx.add_triangle(0,3,1);
        ctx.add_triangle(0,2,3);
    }
    
    void GeometryBuilder::BuildContour(GeometryData& buffer,const sb::vector<Vector2f>& points,const Image& img,const Color& color) {
        BuildLine(buffer, points, img, color);
    }
    
}
