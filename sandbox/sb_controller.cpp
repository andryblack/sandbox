//
//  sb_controller.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 4/17/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_controller.h"
#include "sb_math.h"

SB_META_DECLARE_OBJECT(Sandbox::Controller, Sandbox::meta::object)

namespace Sandbox {
    Vector2f MoveValue(const Vector2f& begin,const Vector2f& end, const Vector2f& dd, float k) {
        float d = ::sinf( k * M_PI );
        Vector2f r = begin * (1.0f-k) + end * k + dd * d;
        return r;
    }
}