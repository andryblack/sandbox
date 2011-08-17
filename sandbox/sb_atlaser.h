/*
 *  sb_atlaser.h
 *  SR
 *
 *  Created by Андрей Куницын on 31.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_ATLASER_H
#define SB_ATLASER_H



namespace Sandbox {
	
	class Resources;
	
	class Atlaser {
	public:
		Atlaser(Resources* resources,int w,int h);
	private:
		Resources* m_resources;
		int	m_w;
		int	m_h;
	};
	
}

#endif /*SB_ATLASER_H*/

