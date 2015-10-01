#ifndef SKELETON_CONVERT_H_INCLUDED
#define SKELETON_CONVERT_H_INCLUDED

#include <pugixml.hpp>
#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_vector.h>
#include <sb_transform2d.h>

class Application;

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

    int m_nodes_count;
    sb::map<sb::string,int> m_node_indexes;
    
    struct frame {
        struct node {
            float a;
            Sandbox::Transform2d tr;
            int image;
        };
        sb::vector<node> nodes;
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
    frame::node& add_frame_node(frame& f);
    
    void add_node(const sb::string& name);
    
    void write_atlases();
    void write_nodes();
    void write_animations();
    bool store_xml(const sb::string& file, Application* app);
    
    void post_scale(float s);
    sb::map<sb::string, sb::string> m_anim_rename;
public:
    bool RenameAnimation(const char* from, const char* to);
};

#endif /*SKELETON_CONVERT_H_INCLUDED*/
