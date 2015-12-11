#include "skeleton_convert.h"
#include "application.h"
#include <sb_data.h>
#include <sb_base64.h>
#include <sbstd/sb_platform.h>

SB_META_DECLARE_OBJECT(SkeletonConvert, Sandbox::meta::object)

SkeletonConvert::SkeletonConvert() : m_image_index(1),m_nodes_count(0) {
    
}

SkeletonConvert::atlas& SkeletonConvert::add_atlas( const sb::string& file, bool premultiplied) {
    m_atlases.push_back(atlas());
    m_atlases.back().filename = file;
    m_atlases.back().premultiplied = premultiplied;
    return m_atlases.back();
}

SkeletonConvert::image& SkeletonConvert::add_image(SkeletonConvert::atlas& atlas,const sb::string& name) {
    m_image_indexes[name]=m_image_index;
    ++m_image_index;
    image& img = atlas.images[name];
    img.hsx = 0;
    img.hsy = 0;
    return img;
}

int SkeletonConvert::get_image_index(const sb::string& name) {
    sb::map<sb::string, int>::const_iterator it = m_image_indexes.find(name);
    if (it!=m_image_indexes.end())
        return it->second;
    return 0;
}
bool SkeletonConvert::get_image(const sb::string& name,image& img) {
    for (sb::vector<atlas>::const_iterator it = m_atlases.begin();it!=m_atlases.end();++it) {
        sb::map<sb::string, image>::const_iterator iit = it->images.find(name);
        if (iit!=it->images.end()) {
            img = iit->second;
            return true;
        }
    }
    return false;
}

SkeletonConvert::animation& SkeletonConvert::add_animation( const sb::string& name, float fps ) {
    m_animations.push_back(animation());
    m_animations.back().name = name;
    m_animations.back().fps = fps;
    sb::map<sb::string, sb::string>::const_iterator it = m_anim_rename.find(name);
    if (it!=m_anim_rename.end()) {
        m_animations.back().name = it->second;
    }
    return m_animations.back();
}

SkeletonConvert::frame& SkeletonConvert::add_frame( SkeletonConvert::animation& a ) {
    a.frames.push_back(frame());
    return a.frames.back();
}

SkeletonConvert::frame::node& SkeletonConvert::add_frame_node(frame& f) {
    f.nodes.push_back(frame::node());
    return f.nodes.back();
}

void SkeletonConvert::write_atlases() {
    pugi::xml_node textures = m_doc.document_element().append_child("textures");
    for (sb::vector<atlas>::const_iterator it = m_atlases.begin();it!=m_atlases.end();++it) {
        pugi::xml_node texture = textures.append_child("texture");
        texture.append_attribute("file").set_value(it->filename.c_str());
        texture.append_attribute("premultiplied").set_value(it->premultiplied);
        for (sb::map<sb::string, image>::const_iterator iit = it->images.begin();iit!=it->images.end();++iit) {
            pugi::xml_node image = texture.append_child("image");
            image.append_attribute("x").set_value(iit->second.x);
            image.append_attribute("y").set_value(iit->second.y);
            image.append_attribute("w").set_value(iit->second.w);
            image.append_attribute("h").set_value(iit->second.h);
            image.append_attribute("r").set_value(iit->second.r);
            image.append_attribute("hsx").set_value(iit->second.hsx);
            image.append_attribute("hsy").set_value(iit->second.hsy);
            image.append_attribute("index").set_value(m_image_indexes[iit->first]);
            image.append_attribute("name").set_value(iit->first.c_str());
            
            
            
        }
    }
}

void SkeletonConvert::add_node(const sb::string& name) {
    ++m_nodes_count;
}

void SkeletonConvert::write_nodes() {
    pugi::xml_node nodes = m_doc.document_element().append_child("nodes");
    nodes.append_attribute("count").set_value(m_nodes_count);
}

void SkeletonConvert::post_scale(float s) {
    for (sb::vector<animation>::iterator anim = m_animations.begin();anim!=m_animations.end();++anim) {
        for (sb::vector<frame>::iterator fit = anim->frames.begin();fit!=anim->frames.end();++fit) {
            for (sb::vector<frame::node>::iterator nit = fit->nodes.begin();nit!=fit->nodes.end();++nit) {
                nit->tr.scale(s);
            }
        }
    }
}

bool SkeletonConvert::RenameAnimation(const char* from, const char* to) {
    for (sb::vector<animation>::iterator anim = m_animations.begin();anim!=m_animations.end();++anim) {
        if (anim->name == from) {
            anim->name = to;
            return true;
        }
    }
    m_anim_rename[from]=to;
    return true;
}
void SkeletonConvert::write_animations() {
    bool raw = true;
    pugi::xml_node animations = m_doc.document_element().append_child("animations");
    for (sb::vector<animation>::const_iterator anim = m_animations.begin();anim!=m_animations.end();++anim) {
        pugi::xml_node a = animations.append_child("animation");
        a.append_attribute("name").set_value(anim->name.c_str());
        a.append_attribute("fps").set_value(anim->fps);
        a.append_attribute("frames").set_value((unsigned int)anim->frames.size());
        a.append_attribute("duration").set_value(float(anim->frames.size())/anim->fps);
        
        if (raw) {
        
            a.append_attribute("compression").set_value("zlib");
            a.append_attribute("encoding").set_value("base64");
            
            Sandbox::VectorData<float>* data = new Sandbox::VectorData<float>();
            data->vector().resize((4+2+1+1)*anim->frames.size()*m_nodes_count);
            float* dst = data->vector().data();
            
            for (sb::vector<frame>::const_iterator fit = anim->frames.begin();fit!=anim->frames.end();++fit) {
                sb_assert(fit->nodes.size()==m_nodes_count);
                
                for (sb::vector<frame::node>::const_iterator nit = fit->nodes.begin();nit!=fit->nodes.end();++nit) {
                    *dst++ = nit->a;
                    *dst++ = nit->tr.m.matrix[0];
                    *dst++ = nit->tr.m.matrix[1];
                    *dst++ = nit->tr.m.matrix[2];
                    *dst++ = nit->tr.m.matrix[3];
                    *dst++ = nit->tr.v.x;
                    *dst++ = nit->tr.v.y;
                    
                    *reinterpret_cast<GHL::UInt32*>(dst) = nit->image;
                    ++dst;
                }
            }
            
            GHL::Data* compressed = GHL_PackZlib(data);
            a.append_child(pugi::node_pcdata).set_value(Sandbox::Base64Encode(compressed->GetData(), compressed->GetSize()).c_str());
            compressed->Release();
            data->Release();
        } else {
            a.append_attribute("compression").set_value("none");
            a.append_attribute("encoding").set_value("none");
            
            for (sb::vector<frame>::const_iterator fit = anim->frames.begin();fit!=anim->frames.end();++fit) {
                sb_assert(fit->nodes.size()==m_nodes_count);
                pugi::xml_node f = a.append_child("frame");
                
                for (sb::vector<frame::node>::const_iterator nit = fit->nodes.begin();nit!=fit->nodes.end();++nit) {
                    pugi::xml_node n = f.append_child("n");
                    n.append_attribute("a").set_value(nit->a);
                    char buff[128];
                    sb::snprintf(buff, 128, "%f %f %f %f %f %f",
                               nit->tr.m.matrix[0],
                               nit->tr.m.matrix[1],
                               nit->tr.m.matrix[2],
                               nit->tr.m.matrix[3],
                               nit->tr.v.x,
                               nit->tr.v.y);
                    n.append_attribute("m").set_value(buff);
                    n.append_attribute("i").set_value(nit->image);
                   
                }
            }

        }
        
    }
}

struct xml_writer : public pugi::xml_writer {
    Sandbox::VectorData<GHL::Byte>* sdata;
    virtual void write(const void* data, size_t size) {
        size_t idx = sdata->vector().size();
        sdata->vector().resize(idx+size);
        ::memcpy(sdata->vector().data()+idx, data, size);
    }
};

bool SkeletonConvert::store_xml(const sb::string& file,Application* app) {
    xml_writer writer;
    writer.sdata = new Sandbox::VectorData<GHL::Byte>();;
    m_doc.save(writer);
    app->store_file(file, writer.sdata);
    writer.sdata->Release();
    return true;
}

