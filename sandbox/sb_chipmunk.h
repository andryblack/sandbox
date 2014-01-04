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

#include <sbstd/sb_shared_ptr.h>
#include "sb_vector2.h"
#include "sb_thread.h"
#include "sb_container.h"
#include "sb_scene_object.h"
#include "sb_color.h"
#include "sb_draw_modificator.h"

#include <vector>

struct cpSpace;
struct cpShape;
struct cpBody;
struct cpConstraint;
struct cpArbiter;

namespace Sandbox {
	
	namespace Chipmunk {
	
		class Shape;
		class Space;
		
		class Body : public sb::enable_shared_from_this<Body>{
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
            void UpdatePosition(float dt);

			cpBody* get_body() const { return m_body;}
		protected:
			explicit Body(cpBody* b);
		private:
			Body(const Body&);
			Body& operator = (const Body&);
			cpBody*	m_body;
		};
		typedef sb::shared_ptr<Body> BodyPtr;
		
		class StaticBody : public Body {
		public:
			StaticBody();
		};
        
        class RogueBody : public Body {
		public:
			RogueBody();
		};
			
		class Shape : public sb::enable_shared_from_this<Shape>{
		public:
			virtual ~Shape();
			float GetFriction() const;
			void SetFriction(float f);
			bool GetSensor() const;
			void SetSensor(bool s);
			float GetElasticity() const;
			void SetElasticity(float e);
            long GetCollisionType() const;
            void SetCollisionType( long type );
            //unsigned int GetLayers() const;
            //void SetLayers(unsigned int);
			
			void set_space( Space* space) { m_space = space;}
			Space* get_space() const { return m_space;}
			cpShape* get_shape() const { return m_shape;}
		protected:
			Shape();
			void SetShape( cpShape* shape );
		private:
			Shape(const Shape&);
			Shape& operator = (const Shape&);
       protected:
			cpShape*	m_shape;
			Space* m_space;
		};
		typedef sb::shared_ptr<Shape> ShapePtr;
		
		class Constraint : public sb::enable_shared_from_this<Constraint>{
		private:
			Constraint(const Constraint&);
			Constraint& operator = (const Constraint&);
		public:
			~Constraint();
			BodyPtr GetA() const;
			BodyPtr GetB() const;
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
		typedef sb::shared_ptr<Constraint> ConstraintPtr;
		
        class CollisionHandler : public sb::enable_shared_from_this<CollisionHandler>{
        public:
            CollisionHandler( long a, long b );
            long GetCollisionTypeA() const { return m_collision_a; }
            long GetCollisionTypeB() const { return m_collision_b; }
            virtual void Handle( const ShapePtr& a, const ShapePtr& b ) = 0;
        private:
            long m_collision_a;
            long m_collision_b;
        };
        typedef sb::shared_ptr<CollisionHandler> CollisionHandlerPtr;
        
		class Space : public Thread, public sb::enable_shared_from_this<Space> {
            SB_META_OBJECT
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
            void AddCollisionHandler( const CollisionHandlerPtr& handler );
            void RemoveCollisionHandler( const CollisionHandlerPtr& handler );
			
			cpSpace* get_space() const { return m_space;}
		private:
			Space(const Space&);
			Space& operator = (const Space&);
			cpSpace*	m_space;
			float	m_time_cum;
			sb::weak_ptr<Space>	m_this;
			std::vector<BodyPtr>	m_bodies;
			std::vector<ShapePtr>	m_shapes;
			std::vector<ConstraintPtr>	m_constraints;
            static int collision_begin(cpArbiter *arb, struct cpSpace *space, void *data);
            struct collision {
                CollisionHandlerPtr handler;
                ShapePtr a;
                ShapePtr b;
                collision(const CollisionHandlerPtr& _handler, 
                          const ShapePtr& _a,
                          const ShapePtr& _b) : handler(_handler),a(_a),b(_b) {}
            };
            std::vector<collision>  m_postprocess_collisions;
            void add_collision_postprocess( const collision& c ) {
                m_postprocess_collisions.push_back( c );
            }
            void process_collision( const collision& c );
            std::vector<CollisionHandlerPtr> m_collision_handlers;
       };
		typedef sb::shared_ptr<Space> SpacePtr;
		
		
		
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
		
		class TransformAdapter : public Container {
		public:
			explicit TransformAdapter( const BodyPtr& body );
			void Update(float dt);
            
            void SetApplyRotate( bool apply ) { m_apply_rotate = apply; }
            bool GetApplyRotate() const { return m_apply_rotate;}
		private:
			BodyPtr		m_body;
            bool    m_apply_rotate;
            TransformModificatorPtr m_transform;
		};
        
        class InvertTransformAdapter : public Container {
		public:
			explicit InvertTransformAdapter( const BodyPtr& body );
            void Draw(Graphics& g) const;
            
            void SetApplyRotate( bool apply ) { m_apply_rotate = apply; }
            bool GetApplyRotate() const { return m_apply_rotate;}
		private:
			BodyPtr		m_body;
            bool    m_apply_rotate;
		};
		
		class DebugDrawImpl;
		
		class SpaceDebugDraw	: public SceneObject {
            SB_META_OBJECT
		public:
			explicit SpaceDebugDraw( const SpacePtr& space );
			~SpaceDebugDraw();
			void Draw( Graphics& g) const;
		private:
			DebugDrawImpl*	m_impl;
			SpacePtr	m_space;
		};
		
		class BodyDebugDraw	: public SceneObject {
            SB_META_OBJECT
		public:
			explicit BodyDebugDraw( const BodyPtr& body );
			~BodyDebugDraw();
			void Draw( Graphics& g) const;
			void SetColor( const Color& c ) { m_color = c;}
			const Color& GetColor() const { return m_color;}
		private:
			DebugDrawImpl*	m_impl;
			BodyPtr	m_body;
			Color	m_color;
		};
	
	}
}

#endif /*SB_CHIPMUNK_H_INCLUDED*/