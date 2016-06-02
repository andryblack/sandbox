//
//  main.m
//  YinYang
//
//  Created by Андрей Куницын on 04.09.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include <windows.h>
int StartApplication(int argc, char** argv);
int main(int argc, char** argv) {
	return StartApplication(argc,argv);
}
int WINAPI WinMain(HINSTANCE /*hInst*/,HINSTANCE /*hPrev*/,LPSTR /*cmdLine*/,int /*showCmd*/) {
	return main(0, 0);
}
