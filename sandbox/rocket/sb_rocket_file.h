//
//  sb_rocket_file.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_rocket_file__
#define __sr_osx__sb_rocket_file__

#include <Rocket/Core/FileInterface.h>

namespace Sandbox {
    
    class Resources;
    
    class RocketFileInterface : public Rocket::Core::FileInterface {
    public:
        explicit RocketFileInterface(Resources* res);
        
        /// Opens a file.
        /// @param file The file handle to write to.
        /// @return A valid file handle, or NULL on failure
        virtual Rocket::Core::FileHandle Open(const Rocket::Core::String& path);
        /// Closes a previously opened file.
        /// @param file The file handle previously opened through Open().
        virtual void Close(Rocket::Core::FileHandle file);
        
        /// Reads data from a previously opened file.
        /// @param buffer The buffer to be read into.
        /// @param size The number of bytes to read into the buffer.
        /// @param file The handle of the file.
        /// @return The total number of bytes read into the buffer.
        virtual size_t Read(void* buffer, size_t size, Rocket::Core::FileHandle file);
        /// Seeks to a point in a previously opened file.
        /// @param file The handle of the file to seek.
        /// @param offset The number of bytes to seek.
        /// @param origin One of either SEEK_SET (seek from the beginning of the file), SEEK_END (seek from the end of the file) or SEEK_CUR (seek from the current file position).
        /// @return True if the operation completed successfully, false otherwise.
        virtual bool Seek(Rocket::Core::FileHandle file, long offset, int origin);
        /// Returns the current position of the file pointer.
        /// @param file The handle of the file to be queried.
        /// @return The number of bytes from the origin of the file.
        virtual size_t Tell(Rocket::Core::FileHandle file);
    private:
        Resources*  m_resources;
    };
    
}

#endif /* defined(__sr_osx__sb_rocket_file__) */
