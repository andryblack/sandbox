/*
 *  sb_accelerometer.cpp
 *  YinYang
 *
 *  Created by Андрей Куницын on 07.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#include "sb_accelerometer.h"
#include <ghl_system.h>
#include "sb_bind.h"

namespace Sandbox {
	
	static size_t enable_count = 0;
	
	Accelerometer::Accelerometer( GHL::System* system ) : m_system( system ) {
		m_data = Vector3f( 0,0,0);
		bool enable = true;
		m_system->SetDeviceState( GHL::DEVICE_STATE_ACCELEROMETER_ENABLED, &enable );
		enable_count++;
	}
	Accelerometer::~Accelerometer() {
		if (enable_count) {
			enable_count--;
			if (!enable_count) {
				bool enable = false;
				m_system->SetDeviceState( GHL::DEVICE_STATE_ACCELEROMETER_ENABLED, &enable );
			}
		}
	}
	bool Accelerometer::Update( float dt ) {
		float data[3];
		if (m_system->GetDeviceData( GHL::DEVICE_DATA_ACCELEROMETER, data )) {
			m_data.x = data[0];
			m_data.y = data[1];
			m_data.z = data[2];
		}
		return false;
	}
	
	void Accelerometer::Bind( Lua* lua ) {
		SB_BIND_BEGIN_BIND
		SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Accelerometer,Sandbox::Thread)
		SB_BIND_BEGIN_PROPERTYS
		SB_BIND_PROPERTY_RO(Sandbox::Accelerometer,Data,GetData,Sandbox::Vector3f)
		SB_BIND_END_PROPERTYS
		SB_BIND_END_CLASS
		SB_BIND(lua)
		SB_BIND_END_BIND
	}
	
}
