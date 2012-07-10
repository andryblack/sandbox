//
//  sb_notcopyable.h
//  YinYang
//
//  Created by Андрей Куницын on 5/13/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_notcopyable_h
#define YinYang_sb_notcopyable_h


namespace Sandbox {
    
    
    class NotCopyable {
    private:
        NotCopyable( const NotCopyable& );
        NotCopyable& operator = (const NotCopyable&);
    protected:
        NotCopyable() {}
    };
    
}

#endif
