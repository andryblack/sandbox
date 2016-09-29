#ifndef SKELETON_CONVERT_H_INCLUDED
#define SKELETON_CONVERT_H_INCLUDED

#include <pugixml.hpp>
#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_vector.h>
#include <sb_transform2d.h>
#include <sb_color.h>

namespace GHL {
    struct Data;
}

class Application;

enum blend_mode {
    blend_mode_normal,
    blend_mode_additive,
    blend_mode_multiply,
    blend_mode_screen
};

class SkeletonConvert : public Sandbox::meta::object {
    SB_META_OBJECT
protected:
    pugi::xml_document   m_doc;
    sb::string  m_dir;
    sb::string  m_out_dir;
    sb::string  m_out_name;
    int m_image_index;
    SkeletonConvert();
    
    

    struct image {
        float x;
        float y;
        float w;
        float h;
        bool r;
        float hsx;
        float hsy;
    };
    struct atlas {
        sb::string filename;
        bool premultiplied;
        sb::map<sb::string, image> images;
    };
    sb::vector<atlas> m_atlases;
    sb::map<sb::string, int> m_image_indexes;
    int get_image_index(const sb::string& name);
    bool get_image(const sb::string& name,image& img);
    
    struct node {
        sb::string name;
        blend_mode blend;
    };
    sb::vector<node> m_nodes;
    sb::map<sb::string, int> m_nodes_indexes;
    
    struct frame {
        struct slot {
            Sandbox::Color clr;
            Sandbox::Transform2d tr;
            int image;
            int node;
        };
        sb::vector<slot> slots;
    };
    struct animation {
        sb::string name;
        float fps;
        sb::vector<frame> frames;
    };
    sb::vector<animation> m_animations;
    
    atlas& add_atlas( const sb::string& file, bool premultiplied);
    image& add_image(atlas& atlas,const sb::string& name);
    animation& add_animation( const sb::string& name, float fps );
    frame& add_frame( animation& a );
    frame::slot& add_frame_slot(frame& f);
    
    node& add_node(const sb::string& name);
    
    void write_atlases();
    void write_nodes();
    void write_animations();
    GHL::Data* get_xml_data();
    bool store_xml(const sb::string& file, Application* app);
    
    void post_scale(float s);
    sb::map<sb::string, sb::string> m_anim_rename;
    bool m_export_files;
    virtual sb::string write_file(const sb::string& file, GHL::Data* data ) { return file;}
    virtual sb::string write_texture(const sb::string& name) { return name; }
public:
    bool RenameAnimation(const char* from, const char* to);
};

#endif /*SKELETON_CONVERT_H_INCLUDED*/
