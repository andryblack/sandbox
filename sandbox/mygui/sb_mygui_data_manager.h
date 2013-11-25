//
//  sb_mygui_data_manager.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_mygui_data_manager__
#define __sr_osx__sb_mygui_data_manager__

#include "MyGUI_DataManager.h"

namespace Sandbox {
    
    class FileProvider;
    
    namespace mygui {
        
        class DataManager : public MyGUI::DataManager {
        public:
            explicit DataManager( FileProvider* file_provider );
            
            /** Get data stream from specified resource name.
             @param _name Resource name (usually file name).
             */
            virtual MyGUI::IDataStream* getData(const std::string& _name);
            
            /** Free data stream.
             @param _data Data stream.
             */
            virtual void freeData(MyGUI::IDataStream* _data);
            
            /** Is data with specified name exist.
             @param _name Resource name.
             */
            virtual bool isDataExist(const std::string& _name);
            
            /** Get all data names with names that matches pattern.
             @param _pattern Pattern to match (for example "*.layout").
             */
            virtual const MyGUI::VectorString& getDataListNames(const std::string& _pattern);
            
            /** Get full path to data.
             @param _name Resource name.
             @return Return full path to specified data.
             For example getDataPath("My.layout") might return "C:\path\to\project\data\My.layout"
             */
            virtual const std::string& getDataPath(const std::string& _name);
            
        private:
            FileProvider*  m_file_provider;
        };
        
    }
}

#endif /* defined(__sr_osx__sb_mygui_data_manager__) */
