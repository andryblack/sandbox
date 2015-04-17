/*
 *  sb_controller.h
 *  SR
 *
 *  Created by Андрей Куницын on 15.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLER_H
#define SB_CONTROLLER_H

#include <sbstd/sb_intrusive_ptr.h>
#include "meta/sb_meta.h"

namespace Sandbox {
	
	class Controller : public meta::object{
        SB_META_OBJECT
	public:
		virtual ~Controller() {}
		virtual void Set(float k) = 0;
	};
	typedef sb::intrusive_ptr<Controller> ControllerPtr;
	
	template <class T,class Setter ,class Obj> class LinearController : public Controller {
	public:
		const T& GetBegin() const { return m_begin;}
		void SetBegin(const T& from) { m_begin = from;}
		const T& GetEnd() const { return m_end;}
		void SetEnd(const T& to) { m_end = to;}
		virtual void Set(float k) {
			Setter::SetValue( m_obj, m_begin * (1.0f-k) + m_end * k );
		}
		explicit LinearController(const Obj& obj) : m_obj(obj),m_begin(),m_end() {}
    protected:
        const Obj& GetObject() const { return m_obj; }
	private:
		Obj	m_obj;
		T	m_begin;
		T	m_end;
	};
	
	template <class Setter ,class Obj> class LinearController<float,Setter,Obj> : public Controller {
	public:
		float GetBegin() const { return m_begin;}
		void SetBegin(float from) { m_begin = from;}
		float GetEnd() const { return m_end;}
		void SetEnd(float to) { m_end = to;}
		void Set(float k) {
			Setter::SetValue( m_obj, m_begin * (1.0f-k) + m_end * k );
		}
		explicit LinearController(const Obj& obj) : m_obj(obj),m_begin(),m_end() {
            sb_assert(m_obj);
        }
	private:
		Obj	m_obj;
		float	m_begin;
		float	m_end;
	};
}

#endif /*SB_CONTROLLER_H*/
