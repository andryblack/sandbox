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
			
            
/*
         out_l out_m_l
  -----------1--3--4 outer_pnt
             |  |\/|
             |  |/\|
        mid_l*--2--5 out_m_r
             | /|p |
             |/ |  |
  -----------06-*--7 out_r
    inner_pnt|mid_r|
          */

            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
            buffer->add_vertex(out_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(p,buffer->tx,buffer->ty);
            buffer->add_vertex(out_m_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(outer_pnt,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(out_m_r,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
            buffer->add_vertex(out_r,buffer->tx,buffer->ty-buffer->th*cw);
            
            buffer->add_triangle(0,1,2);
            buffer->add_triangle(2,1,3);
            buffer->add_triangle(3,4,2);
            buffer->add_triangle(2,4,5);
            buffer->add_triangle(2,5,7);
            buffer->add_triangle(6,2,7);
			
        } else {
			
/*
    inner_pnt|mid_r|
  -----------17-*--6 out_r
             |  |\/|
             |  |/\|
        mid_l*--2--5 out_m_r
             | /|\ |
             |/ | \|
  -----------0--3--4 outer_pnt
         out_l  out_m_l
 
             */

            buffer->add_vertex(out_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
            buffer->add_vertex(p,buffer->tx,buffer->ty);
            buffer->add_vertex(out_m_l,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(outer_pnt,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(out_m_r,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(out_r,buffer->tx,buffer->ty-buffer->th*cw);
            buffer->add_vertex(inner_pnt,buffer->tx,buffer->ty+buffer->th*cw);
            
            buffer->add_triangle(0,1,2);
            buffer->add_triangle(0,2,3);
            buffer->add_triangle(3,2,4);
            buffer->add_triangle(2,5,4);
            buffer->add_triangle(2,6,5);
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
        return (points.front()-points.back()).unit();
    }
    
    void GeometryBuilder::BuildLine(const sb::vector<Vector2f>& points,const Image& img) {
        if (points.size()<2) return;
        
        buffer.primitives = GHL::PRIMITIVE_TYPE_TRIANGLES;
        buffer.texture = img.GetTexture();
        BuildLineContext ctx;
        ctx.buffer = &buffer;
        ctx.color = color.hw_premul();
        ctx.ibase = buffer.vertexes.size();
        ctx.z = 0;
        
        size_t vtcs = 8 + (points.size()-2)*corner_vertexes;
		size_t indxs = 6 * 3 + (points.size()-2)*(corner_indexes+2*3);
        size_t start_vertexes = buffer.vertexes.size();
        size_t start_indexes = buffer.indexes.size();
        
		buffer.vertexes.reserve(start_vertexes+vtcs);
		buffer.indexes.reserve(start_indexes+indxs);
        
        float width = img.GetHeight();
        width*=0.5f;
        ctx.W = width;
        
        float tx = img.GetTexture() ? (img.GetTextureX() / img.GetTexture()->GetWidth()) : 0.0f;
        float ty = img.GetTexture() ? (img.GetTextureY() / img.GetTexture()->GetHeight()) : 0.0f;
        float tw = img.GetTexture() ? (img.GetTextureW() / img.GetTexture()->GetWidth()) : 1.0f;
        float th = img.GetTexture() ? (img.GetTextureH() / img.GetTexture()->GetHeight()) : 1.0f;
        
        
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
        ctx.add_triangle(1,3,0);
        ctx.add_triangle(0,3,2);
		
        /// first segment
        ctx.add_triangle(2,3,5);
        ctx.add_triangle(2,5,4);
		
        ctx.ibase+=4;
		
        for (size_t i=1;i<(points.size()-1);i++) {
			Sandbox::Vector2f dir_r = get_dir_next(points,i);
            build_corner(points[i],dir_l,dir_r,&ctx);
            ctx.ibase+=corner_vertexes-2;
            ctx.add_triangle(1,3,0);
            ctx.add_triangle(0,3,2);
            ctx.ibase+=2;
            dir_l = dir_r;
        }
        //dir_l = get_dir_next(points,points.size()-1);
        
        //ctx.add_vertex(points[0]+dir_l.normal()*width,tx+tw*0.5f,ty+th);
        //ctx.add_vertex(points[0]-dir_l.normal()*width,tx+tw*0.5f,ty);
		
        ctx.add_vertex(points.back()+dir_l.normal()*width,tx+tw*0.5f,ty+th);
        ctx.add_vertex(points.back()-dir_l.normal()*width,tx+tw*0.5f,ty);
        //buffer.ibase-=2;
        ctx.add_vertex(points.back()+dir_l*img.GetWidth()*0.5f+dir_l.normal()*width,tx+tw,ty+th);
        ctx.add_vertex(points.back()+dir_l*img.GetWidth()*0.5f-dir_l.normal()*width,tx+tw,ty);
        /// right dot
        ctx.add_triangle(0,1,3);
        ctx.add_triangle(0,3,2);
        
        sb_assert(buffer.vertexes.size()==vtcs+start_vertexes);
        sb_assert(buffer.indexes.size()==indxs+start_indexes);
    }
    
    void GeometryBuilder::BuildContour(const sb::vector<Vector2f>& points,const Image& img) {
        if (points.size()<3) return;
        
        buffer.primitives = GHL::PRIMITIVE_TYPE_TRIANGLES;
        buffer.texture = img.GetTexture();
        BuildLineContext ctx;
        ctx.buffer = &buffer;
        ctx.color = color.hw_premul();
        ctx.ibase = buffer.vertexes.size();
        ctx.z = 0;
        
        size_t start_vertexes = buffer.vertexes.size();
        size_t start_indexes = buffer.indexes.size();
        
        
        size_t vtcs = (points.size())*corner_vertexes+2;
		size_t indxs = (points.size())*(corner_indexes+2*3);
		
        buffer.vertexes.reserve(start_vertexes+vtcs);
        buffer.indexes.reserve(start_indexes+indxs);
        
        float width = img.GetHeight();
        width*=0.5f;
        ctx.W = width;
        
        float tx = img.GetTexture() ? (img.GetTextureX() / img.GetTexture()->GetWidth()) : 0.0f;
        float ty = img.GetTexture() ? (img.GetTextureY() / img.GetTexture()->GetHeight()) : 0.0f;
        float tw = img.GetTexture() ? (img.GetTextureW() / img.GetTexture()->GetWidth()) : 1.0f;
        float th = img.GetTexture() ? (img.GetTextureH() / img.GetTexture()->GetHeight()) : 1.0f;
        
        
        ctx.tx = tx + tw*0.5f;
        ctx.ty = ty + th*0.5f;
        ctx.th = th*0.5f;
        
        sb_assert(points.size()>=2);
		Sandbox::Vector2f dir_l = (points[0]-points.back()).unit();
        
        //     (1)---(3)
        //      |    /|
        //      |   / |
        //      |  /  |
        //      | /   |
        //      |/    |
        //     (0)---(2)
		
        for (size_t i=0;i<points.size();i++) {
			Sandbox::Vector2f dir_r = get_dir_next(points,i);
            build_corner(points[i],dir_l,dir_r,&ctx);
            ctx.ibase+=corner_vertexes-2;
            ctx.add_triangle(1,3,0);
            ctx.add_triangle(0,3,2);
            ctx.ibase+=2;
            dir_l = dir_r;
        }
        dir_l = get_dir_next(points,points.size()-1);
        
        buffer.vertexes.push_back(buffer.vertexes[start_vertexes]);
        buffer.vertexes.push_back(buffer.vertexes[start_vertexes+1]);
        
        sb_assert(buffer.vertexes.size()==vtcs+start_vertexes);
        sb_assert(buffer.indexes.size()==indxs+start_indexes);
       
    }
    
    struct BuildFillContext : BuildContext {
        Transform2d transform;
        void add_vertex(const Vector2f& v) {
            Vector2f tex = transform.transform(v);
            BuildContext::add_vertex(v, tex.x, tex.y);
        }
    };
    
    void GeometryBuilder::BuildFill(const sb::vector<Vector2f>& points,const Image& img,const Transform2d& tr) {
        if (points.size()<3) return;
        buffer.primitives = GHL::PRIMITIVE_TYPE_TRIANGLES;
        buffer.texture = img.GetTexture();
        BuildFillContext ctx;
        ctx.buffer = &buffer;
        ctx.color = color.hw_premul();
        ctx.ibase = buffer.vertexes.size();
        ctx.z = 0;
        ctx.transform = tr;
        if (buffer.texture) {
            ctx.transform.scale(1.0f/buffer.texture->GetWidth(), 1.0f/buffer.texture->GetHeight());
        }
        size_t idx_top = 0;
        size_t idx_bottom = points.size()-1;
        ctx.add_vertex(points[idx_top]);
        ctx.add_vertex(points[idx_bottom]);
        while (idx_top!=idx_bottom) {
            ++idx_top;
            ctx.add_vertex(points[idx_top]);
            ctx.add_triangle(1, 0, 2);
            ctx.ibase+=1;
            if (idx_top==idx_bottom)
                break;
            --idx_bottom;
            ctx.add_vertex(points[idx_bottom]);
            ctx.add_triangle(0, 1, 2);
            ctx.ibase+=1;
        }
    }
    
}
