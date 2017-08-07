#include "sb_tilemap.h"
#include "sb_data.h"
#include <ghl_data_stream.h>
#include <sbstd/sb_assert.h>
#include "sb_file_provider.h"
#include "sb_log.h"
#include <pugixml.hpp>
#include "utils/sb_base64.h"

SB_META_DECLARE_OBJECT(Sandbox::TileMapLayer, Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::TileMap, Sandbox::meta::object)

namespace Sandbox {
    
    TileMapLayer::TileMapLayer( size_t w, size_t h) : m_width(w),m_height(h),m_data(0) {
        m_data = GHL_CreateData(GHL::UInt32(m_width * m_height * sizeof(data_t)));
    }
    
    TileMapLayer::TileMapLayer( size_t w, size_t h, GHL::Data* data ) : m_width(w),m_height(h),m_data(data) {
        if (m_data) {
            m_data->AddRef();
        }
    }
    TileMapLayer::~TileMapLayer() {
        if (m_data) {
            m_data->Release();
        }
    }
    
    TileMapLayer::data_t TileMapLayer::GetAt(size_t x, size_t y) const {
        if (!m_data)
            return 0;
        sb_assert(x<m_width);
        sb_assert(y<m_height);
        return *(reinterpret_cast<const data_t*>(m_data->GetData()) + x + y * m_width);
    }
    void TileMapLayer::SetAt(size_t x, size_t y, data_t d) {
        if (!m_data)
            return;
        sb_assert(x<m_width);
        sb_assert(y<m_height);
        ::memcpy(m_data->GetDataPtr()+(x+y*m_width)*sizeof(data_t),reinterpret_cast<const GHL::Byte*>(&d), sizeof(d));
    }
    
    
    
    TileMap::TileMap(size_t w, size_t h) : m_width(w),m_height(h) {
        
    }
    
    void TileMap::SetLayer(const char *name, const TileMapLayerPtr &layer) {
        m_layers[name] = layer;
    }
    
    TileMapLayerPtr TileMap::GetLayer(const char* name) const {
        layers_map_t::const_iterator it = m_layers.find(name);
        if (it!=m_layers.end()) {
            return it->second;
        }
        return TileMapLayerPtr();
    }
    
    bool TileMapTMXLoader::Load(GHL::Data* data) {
        pugi::xml_parse_result parse_res = m_document.load_buffer(data->GetData(),
                                                           data->GetSize(),
                                                           pugi::parse_default, pugi::encoding_utf8);
        if (!parse_res) {
            LogError() << "failed parse";
            LogError() << parse_res.description();
            return false;
        }
        
        return true;
    }
    
    
    bool TileMapTMXLoader::ProcessNode(const pugi::xml_node& node) {
        if (::strcmp(node.name(), "layer")==0) {
            LoadLayer(node);
        }
        return true;
    }
    
    bool TileMapTMXLoader::Parse() {
    
    
    
        pugi::xml_node n = m_document.document_element();
        size_t w = n.attribute("width").as_uint();
        size_t h = n.attribute("height").as_uint();
        
        m_map = TileMapPtr(new TileMap(w,h));
        m_map->SetTileSize(Sizei(n.attribute("tilewidth").as_int(),n.attribute("tileheight").as_int()));
        for (pugi::xml_node_iterator it = n.begin();it!=n.end();++it) {
            if (!ProcessNode(*it))
                return false;
        }
        return true;
        
    }
    void TileMapTMXLoader::LoadLayer(const pugi::xml_node &node) {
        size_t lw = node.attribute("width").as_uint();
        size_t lh = node.attribute("height").as_uint();
        pugi::xml_node data_node = node.child("data");
        if (data_node) {
            TileMapLayerPtr layer;
            if (!data_node.attribute("compression") &&
                !data_node.attribute("encoding")) {
                size_t x = 0;
                size_t y = 0;
                layer = TileMapLayerPtr(new TileMapLayer(lw,lh));
                for (pugi::xml_node_iterator ii=data_node.begin(); ii!=data_node.end(); ++ii) {
                    if (::strcmp(ii->name(), "tile")==0) {
                        TileMapLayer::data_t d = ii->attribute("gid").as_uint();
                        layer->SetAt(x, y, d);
                        ++x;
                        if (x>=lw) {
                            ++y;
                            x = 0;
                        }
                    }
                }
            }
            if (!layer) {
                GHL::Data* d = 0;
                if (::strcmp(data_node.attribute("encoding").as_string(),"base64")==0) {
                    d = Base64DecodeData(data_node.child_value());
                } else {
                    return;
                }
                if (!d) return;
                if (::strcmp(data_node.attribute("compression").as_string(),"zlib")==0) {
                    GHL::UInt32 size = GHL::UInt32(sizeof(GHL::UInt32)*lw*lh);
                    VectorData<GHL::Byte>* dd = new VectorData<GHL::Byte>();
                    dd->vector().resize(size);
                    
                    if( GHL_UnpackZlib(d, dd->vector().data(), &size ) && size == dd->GetSize() ) {
                        d->Release();
                        d = dd;
                    } else {
                        dd->Release();
                        d->Release();
                        return;
                    }
                } else if (data_node.attribute("compression")) {
                    LogError() << "unsupported compression " << data_node.attribute("compression").as_string();
                    d->Release();
                    return;
                }
                layer = TileMapLayerPtr(new TileMapLayer(lw,lh,d));
            }
            if (layer) {
                m_map->SetLayer(node.attribute("name").as_string(), layer);
            }
        }
    }
    
    bool TileMapTMXLoader::Load(FileProvider* fp, const char* filename) {
        GHL::DataStream* ds = fp->OpenFile(filename);
        if (!ds) {
            LogError() << "not found file " << filename;
            return TileMapPtr();
        }
        GHL::Data* tmx_data = GHL_ReadAllData( ds );
        ds->Release();
        sb_assert(tmx_data);
        bool res = Load(tmx_data);
        tmx_data->Release();
        return res;
    }
    
    TileMapPtr LoadTileMapTMX(FileProvider* fp, const char* filename) {
        if (!fp || !filename) return  TileMapPtr();
        
        TileMapTMXLoader loader;
        if (loader.Load(fp,filename)) {
            if (loader.Parse())
                return loader.GetMap();
        }
        return TileMapPtr();
    }
    
    
}
