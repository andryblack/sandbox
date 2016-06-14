//
//  main.m
//  YinYang
//
//  Created by Андрей Куницын on 04.09.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

int StartApplication( int argc, char **argv );

int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = StartApplication(argc,argv);
    [pool release];
    return retVal;
}
