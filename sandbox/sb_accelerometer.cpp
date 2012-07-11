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
#include "luabind/sb_luabind.h"
#include "sb_lua.h"

SB_META_DECLARE_KLASS(Sandbox::Accelerometer, Sandbox::Thread)
SB_META_BEGIN_KLASS_BIND(Sandbox::Accelerometer)
SB_META_PROPERTY_RO(Data,GetData)
SB_META_END_KLASS_BIND()

namespace Sandbox {
	
	static size_t enable_count = 0;
	
	Accelerometer::Accelerometer( GHL::System* system ) : m_system( system ) {
		m_data = Vector3f( 0,0,0);
		bool enable = true;
		m_system->SetDeviceState( GHL::DEVICE_STATE_ACCELEROMETER_ENABLED, &enable );
		enable_count++;
        m_actual = false;
		Update(0);
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
	bool Accelerometer::Update( float ) {
		float data[3];
		if (m_system->GetDeviceData( GHL::DEVICE_DATA_ACCELEROMETER, data )) {
			m_data.x = data[0];
			m_data.y = data[1];
			m_data.z = data[2];
            m_actual = true;
		}
		return false;
	}
	void Accelerometer::Bind( LuaVM* lua ) {
        luabind::Class<Accelerometer>(lua->GetVM());
	}
}
