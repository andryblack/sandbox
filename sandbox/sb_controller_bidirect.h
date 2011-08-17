/*
 *  sb_controller_bidirect.h
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLER_BIDIRECT_H 
#define SB_CONTROLLER_BIDIRECT_H

#include "sb_controller.h"

namespace Sandbox {

	class ControllerBidirect : public Controller {
	public:
		explicit ControllerBidirect(const ControllerPtr& c);
		~ControllerBidirect();
		void Set(float k);
	private:
		ControllerPtr m_child;
	};
	
}

#endif /*SB_CONTROLLER_BIDIRECT_H*/