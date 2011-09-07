/*
 *  sb_chipmunk.h
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#ifndef SB_CHIPMUNK_H_INCLUDED
#define SB_CHIPMUNK_H_INCLUDED

#include "sb_shared_ptr.h"
#include "sb_vector2.h"
#include "sb_thread.h"
#include "sb_container_transform.h"

#include <vector>

struct cpSpace;
struct cpShape;
struct cpBody;

namespace Sandbox {
	
	namespace Chipmunk {
	
	class Shape;
	
	class Body {
	public:
		Body(float mass,float inertia);
		virtual ~Body();
		float GetMass() const;
		void SetMass(float m);
		float GetMoment() const;
		void SetMoment(float i);
		Vector2f GetPos() const;
		void SetPos(const Vector2f& pos);
		Vector2f GetVel() const;
		void SetVel(const Vector2f& vel);
		Vector2f GetForce() const;
		void SetForce(const Vector2f& force);
		float GetAngle() const;
		void SetAngle(float a);
		float GetAngVel() const;
		void SetAngVel(float v);
		float GetTorque() const;
		void SetTorque(float t);
		Vector2f GetRot() const;
		float GetVelLimit() const;
		void SetVelLimit(float v);
		float GetAngVelLimit() const;
		void SetAngVelLimit(float v);
		Vector2f Local2World(const Vector2f& pos) const;
		Vector2f World2Local(const Vector2f& pos) const;
		void ResetForces();
		void ApplyForce(const Vector2f& f, const Vector2f& r);
		void ApplyImpulse(const Vector2f& j, const Vector2f& r);
		bool IsSleeping() const;
		void Activate();
		void Sleep();
		bool IsStatic() const;
	protected:
		explicit Body( cpBody* external ); 
		cpBody* get_body() const { return m_body;}
	private:
		Body(const Body&);
		Body& operator = (const Body&);
		cpBody*	m_body;
		bool	m_hold;
		friend class Space;
		friend class CircleShape;
		friend class PolyShape;
		friend class SegmentShape;
	};
	typedef shared_ptr<Body> BodyPtr;
	
	class Shape {
	public:
		virtual ~Shape();
		float GetFriction() const;
		void SetFriction(float f);
	protected:
		Shape();
		void SetShape( cpShape* shape );
	private:
		Shape(const Shape&);
		Shape& operator = (const Shape&);
		cpShape*	m_shape;
		friend class Space;
	};
	typedef shared_ptr<Shape> ShapePtr;
	
	class Space : public Thread {
	public:
		Space();
		~Space();
		void SetGravity( const Vector2f& g );
		Vector2f GetGravity() const;
		void AddBody(const BodyPtr& body);
		void RemoveBody(const BodyPtr& body);
		void AddShape(const ShapePtr& shape);
		void RemoveShape(const ShapePtr& shape);
		const BodyPtr& GetStaticBody() const {
			return m_static_body;
		}
		void Step( float dt );
		bool Update( float dt );
	private:
		Space(const Space&);
		Space& operator = (const Space&);
		cpSpace*	m_space;
		BodyPtr	m_static_body;
		std::vector<ShapePtr>	m_shapes;
		std::vector<BodyPtr>	m_bodys;
		float	m_time_cum;
	};
	typedef shared_ptr<Space> SpacePtr;
	
	
	
	class CircleShape : public Shape {
	public:
		CircleShape( const BodyPtr& body, float radius, const Vector2f& pos ); 
	};
	
	class SegmentShape : public Shape {
	public:
		SegmentShape( const BodyPtr& body, const Vector2f& a, const Vector2f& b, float radius);
	};
	
	class PolyShape : public Shape {
	public:
		PolyShape( const BodyPtr& body, const std::vector<Vector2f>& points, const Vector2f& offset );
	};
	
	class Constraint {
		
	};
	typedef shared_ptr<Constraint> ConstraintPtr;
		
		
		class TransformAdapter : public Thread {
		public:
			TransformAdapter( const BodyPtr& body, const ContainerTransformPtr& transform);
			void Sync();
			bool Update(float);
		private:
			ContainerTransformPtr	m_transform;
			BodyPtr		m_body;
		};
	
	}
}

#endif /*SB_CHIPMUNK_H_INCLUDED*/