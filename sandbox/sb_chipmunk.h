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
struct cpConstraint;

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

			cpBody* get_body() const { return m_body;}
		protected:
			explicit Body(cpBody* b);
		private:
			Body(const Body&);
			Body& operator = (const Body&);
			cpBody*	m_body;
		};
		typedef shared_ptr<Body> BodyPtr;
		
		class StaticBody : public Body {
		public:
			StaticBody();
		};
			
		class Shape {
		public:
			virtual ~Shape();
			float GetFriction() const;
			void SetFriction(float f);
			bool GetSensor() const;
			void SetSensor(bool s);
			float GetElasticity() const;
			void SetElasticity(float e);
			
			cpShape* get_shape() const { return m_shape;}
		protected:
			Shape();
			void SetShape( cpShape* shape );
		private:
			Shape(const Shape&);
			Shape& operator = (const Shape&);
		protected:
			cpShape*	m_shape;
		};
		typedef shared_ptr<Shape> ShapePtr;
		
		class Constraint {
		private:
			Constraint(const Constraint&);
			Constraint& operator = (const Constraint&);
		public:
			~Constraint();
			Body* GetA() const;
			Body* GetB() const;
			float GetMaxForce() const;
			void SetMaxForce(float f);
			float GetMaxBias() const;
			void SetMaxBias(float b);
			float GetImpulse() const;
			
			cpConstraint* get_constraint() const { return m_constraint;}
		protected:
			explicit Constraint( cpConstraint* c);
			cpConstraint*	m_constraint;
		};
		typedef shared_ptr<Constraint> ConstraintPtr;
		
		class Space : public Thread {
		public:
			Space();
			~Space();
			void SetGravity( const Vector2f& g );
			Vector2f GetGravity() const;
			int GetIterations() const;
			void SetIterations(int i);
			void AddBody(const BodyPtr& body);
			void RemoveBody(const BodyPtr& body);
			void AddShape(const ShapePtr& shape);
			void RemoveShape(const ShapePtr& shape);
			void AddConstraint(const ConstraintPtr& constraint);
			void RemoveConstraint(const ConstraintPtr& constraint);
			void Step( float dt );
			bool Update( float dt );
			
			cpSpace* get_space() const { return m_space;}
		private:
			Space(const Space&);
			Space& operator = (const Space&);
			cpSpace*	m_space;
			float	m_time_cum;
		};
		typedef shared_ptr<Space> SpacePtr;
		
		
		
		class CircleShape : public Shape {
		public:
			CircleShape( const BodyPtr& body, float radius, const Vector2f& pos ); 
			Vector2f GetOffset() const;
			float	GetRadius() const;
		};
		
		class SegmentShape : public Shape {
		public:
			SegmentShape( const BodyPtr& body, const Vector2f& a, const Vector2f& b, float radius);
		};
		
		class PolyShape : public Shape {
		public:
			PolyShape( const BodyPtr& body, const std::vector<Vector2f>& points, const Vector2f& offset );
		};
		class BoxShape : public Shape {
		public:
			BoxShape( const BodyPtr& body, float w, float h);
		};
		
		
		
		class PinJoint : public Constraint {
		public:
			PinJoint(const BodyPtr& a,const BodyPtr& b,
					 const Vector2f& anchr1, const Vector2f& anchr2 );
			Vector2f GetAnchr1() const;
			void SetAnchr1(const Vector2f& v);
			Vector2f GetAnchr2() const;
			void SetAnchr2(const Vector2f& v);
			float GetDist() const;
			void SetDist(float d);
		};
		
		class SlideJoint : public Constraint {
		public:
			SlideJoint(const BodyPtr& a,const BodyPtr& b,const Vector2f& anchr1, 
					   const Vector2f& anchr2, float min,float max );
			Vector2f GetAnchr1() const;
			void SetAnchr1(const Vector2f& v);
			Vector2f GetAnchr2() const;
			void SetAnchr2(const Vector2f& v);
			float GetMin() const;
			void SetMin(float d);
			float GetMax() const;
			void SetMax(float d);
		};
		
		class PivotJoint : public Constraint {
		public:
			PivotJoint(const BodyPtr& a,const BodyPtr& b,
					   const Vector2f& anchr1, const Vector2f& anchr2 );
			Vector2f GetAnchr1() const;
			void SetAnchr1(const Vector2f& v);
			Vector2f GetAnchr2() const;
			void SetAnchr2(const Vector2f& v);
		};
		
		class GrooveJoint : public Constraint {
		public:
			GrooveJoint(const BodyPtr& a,const BodyPtr& b,
						const Vector2f& grove_a, const Vector2f& grove_b, 
						const Vector2f& anchr2 );
			Vector2f GetGrooveA() const;
			void SetGrooveA(const Vector2f& v);
			Vector2f GetGrooveB() const;
			void SetGrooveB(const Vector2f& v);
			Vector2f GetAnchr2() const;
			void SetAnchr2(const Vector2f& v);
		};
		
		class DampedSpring : public Constraint {
		public:
			DampedSpring(const BodyPtr& a,const BodyPtr& b,
						 const Vector2f& anchr1, const Vector2f& anchr2,
						 float restLength, float stiffness, float damping);
			Vector2f GetAnchr1() const;
			void SetAnchr1(const Vector2f& v);
			Vector2f GetAnchr2() const;
			void SetAnchr2(const Vector2f& v);
			float GetRestLength() const;
			void SetRestLength(float l);
			float GetStiffness() const;
			void SetStiffness(float s);
			float GetDamping() const;
			void SetDamping(float d);
		};
		
		class DampedRotarySpring : public Constraint {
		public:
			DampedRotarySpring(const BodyPtr& a,const BodyPtr& b,
						 float restAngle, float stiffness, float damping);
			float GetRestAngle() const;
			void SetRestAngle(float l);
			float GetStiffness() const;
			void SetStiffness(float s);
			float GetDamping() const;
			void SetDamping(float d);
		};
		
		class RotaryLimitJoint : public Constraint {
		public:
			RotaryLimitJoint(const BodyPtr& a,const BodyPtr& b,
							   float min, float max);
			float GetMin() const;
			void SetMin(float m);
			float GetMax() const;
			void SetMax(float m);
		};
		
		class RatchetJoint : public Constraint {
		public:
			RatchetJoint(const BodyPtr& a,const BodyPtr& b,
							 float phase, float ratchet);
			float GetAngle() const;
			void SetAngle(float m);
			float GetPhase() const;
			void SetPhase(float m);
			float GetRatchet() const;
			void SetRatchet(float m);
		};
		
		class GearJoint : public Constraint {
		public:
			GearJoint(const BodyPtr& a,const BodyPtr& b,
						 float phase, float ratio);
			float GetPhase() const;
			void SetPhase(float m);
			float GetRatio() const;
			void SetRatio(float m);
		};
		
		class SimpleMotor : public Constraint {
		public:
			SimpleMotor(const BodyPtr& a,const BodyPtr& b,
					  float rate);
			float GetRate() const;
			void SetRate(float r);
		};
		
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