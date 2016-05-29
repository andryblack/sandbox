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
        m_data = GHL_CreateData(m_width * m_height * sizeof(data_t));
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
        m_data->SetData((x+y*m_width)*sizeof(data_t),
                        reinterpret_cast<const GHL::Byte*>(&d), sizeof(d));
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
    
    
    TileMapPtr LoadTileMapTMX(FileProvider* fp, const char* filename) {
        if (!fp || !filename) return  TileMapPtr();
        GHL::DataStream* ds = fp->OpenFile(filename);
        if (!ds) {
            LogError() << "not found file " << filename;
            return TileMapPtr();
        }
        GHL::Data* tmx_data = GHL_ReadAllData( ds );
        ds->Release();
        sb_assert(tmx_data);
        pugi::xml_document doc;
        pugi::xml_parse_result parse_res = doc.load_buffer(tmx_data->GetData(),
                                                           tmx_data->GetSize(),
                                                           pugi::parse_default, pugi::encoding_utf8);
        tmx_data->Release();
        if (!parse_res) {
            LogError() << "failed parse " << filename;
            LogError() << parse_res.description();
            return TileMapPtr();
        }
        pugi::xml_node n = doc.document_element();
        size_t w = n.attribute("width").as_uint();
        size_t h = n.attribute("height").as_uint();
        TileMapPtr map = TileMapPtr(new TileMap(w,h));
        for (pugi::xml_node_iterator it = n.begin();it!=n.end();++it) {
            if (::strcmp(it->name(), "layer")==0) {
                size_t lw = it->attribute("width").as_uint();
                size_t lh = it->attribute("height").as_uint();
                pugi::xml_node data_node = it->child("data");
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
                            continue;
                        }
                        if (!d) continue;
                        if (::strcmp(data_node.attribute("compression").as_string(),"zlib")==0) {
                            GHL::UInt32 size = sizeof(GHL::UInt32)*lw*lh;
                            VectorData<GHL::Byte>* dd = new VectorData<GHL::Byte>();
                            dd->vector().resize(size);
                            
                            if( GHL_UnpackZlib(d, dd->vector().data(), &size ) && size == dd->GetSize() ) {
                                d->Release();
                                d = dd;
                            } else {
                                dd->Release();
                                d->Release();
                                continue;
                            }
                        } else if (data_node.attribute("compression")) {
                            LogError() << "unsupported compression " << data_node.attribute("compression").as_string();
                            d->Release();
                            continue;
                        }
                        layer = TileMapLayerPtr(new TileMapLayer(lw,lh,d));
                    }
                    if (layer) {
                        map->SetLayer(it->attribute("name").as_string(), layer);
                    }
                }
            }
        }
        return map;
    }
    
    
}