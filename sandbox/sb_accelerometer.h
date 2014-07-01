/*
 *  sb_accelerometer.h
 *  YinYang
 *
 *  Created by Андрей Куницын on 07.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#ifndef SB_ACCELEROMETER_H_INCLUDED
#define SB_ACCELEROMETER_H_INCLUDED

#include "sb_thread.h"
#include "sb_vector3.h"

namespace GHL {
	struct System;
}

namespace Sandbox {
	 
	class LuaVM;
	
	class Accelerometer : public Thread {
	public:
		explicit Accelerometer( GHL::System* system );
		~Accelerometer();
		bool Update( float dt );
		const Vector3f& GetData() const { return m_data;}
        bool GetSupported() const { return m_supported; }
		static void Bind( LuaVM* lua );
        bool    Supported();
    protected:
        bool IsActual() const { return m_actual;} 
	private:
		GHL::System*	m_system;
		Vector3f		m_data;
        bool            m_actual;
        bool            m_supported;
	};
	typedef sb::intrusive_ptr<Accelerometer> AccelerometerPtr;
}

#endif /*SB_ACCELEROMETER_H_INCLUDED*/