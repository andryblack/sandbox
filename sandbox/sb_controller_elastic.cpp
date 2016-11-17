/*
 *  sb_controller_elastic.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 28.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_controller_elastic.h"
#include "sb_math.h"

namespace Sandbox {

	
	ControllerElastic::ControllerElastic(const ControllerPtr& child) : m_child(child),
		m_hard(8.0f),m_end(1.0f) , m_phases(5){
	}
	
	ControllerElastic::~ControllerElastic() {
	}
	

	void ControllerElastic::Set(float k) {
		if (k<=0.0f) {
			m_child->Set(0.0f);
		} else if (k>=1.0f) {
			m_child->Set(m_end);
		} else {
			float d = asin(m_end);
			float modulator = pow(2,-m_hard*k);
			m_child->Set(m_end+modulator*sin(k*(float(M_PI*m_phases)+d)-d));
		}
		/*float Elastic::easeOut(float t,float b , float c, float d) {
			if (t==0) return b;  if ((t/=d)==1) return b+c;
			float p=d*.3f;
			float a=c;
			float s=p/4;
			return (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c + b);
        }*/
	}
}
