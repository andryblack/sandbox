//
//  sb_mygui_data_manager.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_mygui_data_manager.h"
#include "../sb_file_provider.h"
#include <ghl_data_stream.h>
#include <sbstd/sb_assert.h>

namespace Sandbox {
    
    namespace mygui {
        
        class DataStream : public MyGUI::IDataStream {
        private:
            GHL::DataStream* m_ds;
            size_t  m_size;
        public:
            explicit DataStream(GHL::DataStream* ds) : m_ds(ds),m_size(0) {
                
            }
            ~DataStream() {
                m_ds->Release();
            }
            
            virtual bool eof() { return m_ds->Eof(); }
            virtual size_t size() {
                if (!m_size) {
                    GHL::UInt32 pos = m_ds->Tell();
                    m_ds->Seek(0, GHL::F_SEEK_END);
                    m_size = m_ds->Tell();
                    m_ds->Seek(pos, GHL::F_SEEK_BEGIN);
                }
                return m_size;
            }
            virtual void readline(std::string& _source, MyGUI::Char _delim ) {
                _source.clear();
                char ch;
                while (!eof()) {
                    read(&ch,1);
                    if (ch == _delim) break;
                    _source += ch;
                }
            }
            virtual size_t read(void* _buf, size_t _count) {
                return m_ds->Read(reinterpret_cast<GHL::Byte*>(_buf), GHL::UInt32(_count));
            }
        };
     
        DataManager::DataManager( FileProvider* file_provider ) : m_file_provider(file_provider) {
            
        }
        
        
        MyGUI::IDataStream* DataManager::getData(const std::string& _name) {
            bool variant = false;
            GHL::DataStream* ds = m_file_provider->OpenFileVariant(_name.c_str(),variant);
            if (!ds) return 0;
            return new DataStream(ds);
        }
        
        void DataManager::freeData(MyGUI::IDataStream* _data) {
            delete static_cast<DataStream*>(_data);
        }
        
        bool DataManager::isDataExist(const std::string& _name) {
            GHL::DataStream* ds = m_file_provider->OpenFile(_name.c_str());
            if (ds) {
                ds->Release();
                return true;
            }
            return false;
        }
        
        const MyGUI::VectorString& DataManager::getDataListNames(const std::string& _pattern) {
            static MyGUI::VectorString empty;
            return empty;
        }
        
        const std::string& DataManager::getDataPath(const std::string& _name) {
            return _name;
        }
        
    }
    
}