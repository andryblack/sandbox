#ifndef SB_TILEMAP_H_INCLUDED
#define SB_TILEMAP_H_INCLUDED

#include "meta/sb_meta.h"
#include <ghl_types.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>
#include "sb_size.h"

#include <pugixml.hpp>

namespace GHL {
    struct Data;
}

namespace Sandbox {
    
    class FileProvider;
    
    class TileMapLayer : public meta::object {
        SB_META_OBJECT
    private:
        size_t m_width;
        size_t m_height;
        GHL::Data*  m_data;
    public:
        typedef GHL::UInt32 data_t;
        
        explicit TileMapLayer( size_t w, size_t h);
        TileMapLayer( size_t w, size_t h, GHL::Data* data );
        ~TileMapLayer();
        
        size_t GetWidth() const { return m_width; }
        size_t GetHeight() const { return m_height; }
        
        data_t GetAt(size_t x, size_t y) const;
        void SetAt(size_t x, size_t y, data_t d);
    };
    typedef sb::intrusive_ptr<TileMapLayer> TileMapLayerPtr;
    
    class TileMap : public meta::object {
        SB_META_OBJECT
        size_t m_width;
        size_t m_height;
        typedef sb::map<sb::string,TileMapLayerPtr> layers_map_t;
        layers_map_t    m_layers;
        Sizei m_tilesize;
    public:
        explicit TileMap( size_t w, size_t h);
        void SetLayer( const char* name, const TileMapLayerPtr& layer );
        TileMapLayerPtr GetLayer(const char* name) const;
        size_t GetWidth() const { return m_width; }
        size_t GetHeight() const { return m_height; }
        const Sizei& GetTileSize() const { return m_tilesize; }
        void SetTileSize(const Sizei& s) { m_tilesize = s; }
    };
    typedef sb::intrusive_ptr<TileMap> TileMapPtr;
    
    class TileMapTMXLoader {
    private:
        pugi::xml_document m_document;
        TileMapPtr m_map;
    protected:
        void LoadLayer(const pugi::xml_node& node);
        virtual bool ProcessNode(const pugi::xml_node& node);
    public:
        bool Load(GHL::Data* data);
        bool Load(FileProvider* fp, const char* filename);
        bool Parse();
        const TileMapPtr& GetMap() const { return m_map; }
    };
    TileMapPtr LoadTileMapTMX(FileProvider* fp, const char* filename);
}

#endif /*SB_TILEMAP_H_INCLUDED*/
