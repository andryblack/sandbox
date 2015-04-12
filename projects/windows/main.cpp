//
//  main.m
//  YinYang
//
//  Created by Андрей Куницын on 04.09.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "../../src/application.h"
#include <windows.h>
int main(int argc, char** argv) {
	int res = GHL_StartApplication(new Application(), 0, 0);
	return res;
}
int WINAPI WinMain(HINSTANCE /*hInst*/,HINSTANCE /*hPrev*/,LPSTR /*cmdLine*/,int /*showCmd*/) {
	return main(0, 0);
}
