//
//  main.m
//  YinYang
//
//  Created by Андрей Куницын on 04.09.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "application.h"

void register_vk_auth(lua_State* L) {}
int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = GHL_StartApplication(new Application(),argc,argv);
    [pool release];
    return retVal;
}
