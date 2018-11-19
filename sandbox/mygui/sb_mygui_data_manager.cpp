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
        
        static const size_t BUFFER_SIZE = 1024;
        
        class DataStream : public MyGUI::IDataStream {
        private:
            GHL::DataStream* m_ds;
            GHL::Byte   m_buffer[BUFFER_SIZE];
            size_t  m_buffer_size;
            size_t  m_buffer_offset;
            size_t  m_size;
            bool    m_eof;
        public:
            explicit DataStream(GHL::DataStream* ds) : m_ds(ds),m_size(0),m_buffer_size(0),m_buffer_offset(0),m_eof(false) {
                
            }
            ~DataStream() {
                m_ds->Release();
            }
            
            virtual bool eof() {
                return m_eof && m_buffer_size == 0;
            }
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
                if (m_buffer_size == 0) {
                    m_buffer_offset = 0;
                    m_buffer_size = m_ds->Read(m_buffer, BUFFER_SIZE);
                    m_eof = m_ds->Eof();
                }
                GHL::Byte* out = static_cast<GHL::Byte*>(_buf);
                size_t readed = 0;
                if (m_buffer_size) {
                    size_t copy = (m_buffer_size < _count) ? m_buffer_size : _count;
                    if (copy) {
                        memcpy(out, &m_buffer[m_buffer_offset], GHL::UInt32(copy));
                    }
                    m_buffer_offset += copy;
                    m_buffer_size -= copy;
                    out += copy;
                    _count -= copy;
                    readed += copy;
                }
                if (_count) {
                    readed += m_ds->Read(out, GHL::UInt32(_count));
                    m_eof = m_ds->Eof();
                }
                return readed;
            }
        };
     
        DataManager::DataManager( FileProvider* file_provider ) : m_file_provider(file_provider) {
            
        }
        
        
        MyGUI::IDataStream* DataManager::getData(const std::string& _name) {
            GHL::DataStream* ds = m_file_provider->OpenFile(_name.c_str());
            if (!ds) return 0;
            return new DataStream(ds);
        }
        
        void DataManager::freeData(MyGUI::IDataStream* _data) {
            delete static_cast<DataStream*>(_data);
        }
        
        bool DataManager::isDataExist(const std::string& _name, bool _texture) {
            if (_texture) {
                bool variant = false;
                GHL::DataStream* ds = m_file_provider->OpenFileVariant(_name.c_str(),variant);
                if (ds) {
                    ds->Release();
                    return true;
                }
            } else {
                GHL::DataStream* ds = m_file_provider->OpenFile(_name.c_str());
                if (ds) {
                    ds->Release();
                    return true;
                }
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
