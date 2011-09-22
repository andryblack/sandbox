/*
 *  sb_chipmunk_bind.cpp
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#include "sb_chipmunk_bind.h"
#include "sb_chipmunk.h"
#include "sb_bind.h"

namespace Sandbox {

	namespace Chipmunk {
		void Bind( Lua* lua ) {
			SB_BIND_BEGIN_BIND
			{
				SB_BIND_BEGIN_SHARED_CLASS( Sandbox::Chipmunk::Shape )
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Shape, Sensor, GetSensor, SetSensor, bool )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Shape, Elasticity, GetElasticity, SetElasticity, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Shape, Friction, GetFriction, SetFriction, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_CLASS( Sandbox::Chipmunk::Body )
				SB_BIND_SHARED_CONSTRUCTOR( Sandbox::Chipmunk::Body,(float,float) )
				SB_BIND_BEGIN_METHODS
				SB_BIND_METHOD( Sandbox::Chipmunk::Body, Local2World, Sandbox::Vector2f(Sandbox::Vector2f) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Body, World2Local, Sandbox::Vector2f(Sandbox::Vector2f) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Body, ResetForces, void() )
				SB_BIND_METHOD( Sandbox::Chipmunk::Body, Activate, void() )
				SB_BIND_METHOD( Sandbox::Chipmunk::Body, Sleep, void() )
				SB_BIND_METHOD( Sandbox::Chipmunk::Body, ApplyForce, void(Sandbox::Vector2f,Sandbox::Vector2f) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Body, ApplyImpulse, void(Sandbox::Vector2f,Sandbox::Vector2f) )
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, Mass, GetMass, SetMass, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, Moment, GetMoment, SetMoment, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, Pos, GetPos, SetPos, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, Vel, GetVel, SetVel, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, Force, GetForce, SetForce, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, Angle, GetAngle, SetAngle, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, AngVel, GetAngVel, SetAngVel, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, Torque, GetTorque, SetTorque, float )
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::Body, Rot, GetRot, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, VelLimit, GetVelLimit, SetVelLimit, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Body, AngVelLimit, GetAngVelLimit, SetAngVelLimit, float )
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::Body, IsSleeping, IsSleeping, bool )
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::Body, IsStatic, IsStatic, bool )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::StaticBody,Sandbox::Chipmunk::Body )
				SB_BIND_SHARED_CONSTRUCTOR( Sandbox::Chipmunk::StaticBody,() )
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::CircleShape,Sandbox::Chipmunk::Shape)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::CircleShape, (Sandbox::Chipmunk::Body,float,Sandbox::Vector2f),(const Sandbox::Chipmunk::BodyPtr&, float,const Sandbox::Vector2f&))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::CircleShape, Radius, GetRadius, float )
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::CircleShape, Offset, GetOffset, Vector2f )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::SegmentShape,Sandbox::Chipmunk::Shape)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::SegmentShape, (Sandbox::Chipmunk::Body,Sandbox::Vector2f,Sandbox::Vector2f,float),(const Sandbox::Chipmunk::BodyPtr&, const Sandbox::Vector2f&,const Sandbox::Vector2f&,float))
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::PolyShape,Sandbox::Chipmunk::Shape)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::PolyShape, (Sandbox::Chipmunk::Body, Sandbox::Vector2fList,Sandbox::Vector2f),(const Sandbox::Chipmunk::BodyPtr&, const std::vector<Vector2f>&,const Sandbox::Vector2f&))
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::BoxShape,Sandbox::Chipmunk::Shape)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::BoxShape, (Sandbox::Chipmunk::Body,float,float),(const Sandbox::Chipmunk::BodyPtr&, float,float))
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_CLASS( Sandbox::Chipmunk::Constraint )
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::Constraint, A, GetA, Sandbox::Chipmnk::Body )
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::Constraint, B, GetB, Sandbox::Chipmnk::Body )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Constraint, MaxForce, GetMaxForce, SetMaxForce, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Constraint, MaxBias, GetMaxBias, SetMaxBias, float )
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::Constraint, Impulse, GetImpulse, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::PinJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::PinJoint, (Sandbox::Chipmunk::Body,
																		   Sandbox::Chipmunk::Body,
																		   Vector2f,Vector2f
																		   ),(const Sandbox::Chipmunk::BodyPtr&,
																			  const Sandbox::Chipmunk::BodyPtr&,
																			  const Vector2f&,const Vector2f&))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PinJoint, Anchr1, GetAnchr1,SetAnchr1, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PinJoint, Anchr2, GetAnchr2,SetAnchr2, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PinJoint, Dist, GetDist,SetDist, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::SlideJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::SlideJoint, (Sandbox::Chipmunk::Body,
																		   Sandbox::Chipmunk::Body,
																		   Vector2f,Vector2f,float,float
																		   ),(const Sandbox::Chipmunk::BodyPtr&,
																			  const Sandbox::Chipmunk::BodyPtr&,
																			  const Vector2f&,const Vector2f&,float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::SlideJoint, Anchr1, GetAnchr1,SetAnchr1, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::SlideJoint, Anchr2, GetAnchr2,SetAnchr2, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::SlideJoint, Min, GetMin,SetMin, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::SlideJoint, MAx, GetMax,SetMax, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::PivotJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::PivotJoint, (Sandbox::Chipmunk::Body,
																		   Sandbox::Chipmunk::Body,
																		   Vector2f,Vector2f
																		   ),(const Sandbox::Chipmunk::BodyPtr&,
																			  const Sandbox::Chipmunk::BodyPtr&,
																			  const Vector2f&,const Vector2f&))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PivotJoint, Anchr1, GetAnchr1,SetAnchr1, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PivotJoint, Anchr2, GetAnchr2,SetAnchr2, Vector2f )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::GrooveJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::GrooveJoint, (Sandbox::Chipmunk::Body,
																			 Sandbox::Chipmunk::Body,
																			 Vector2f,Vector2f,Vector2f
																			 ),(const Sandbox::Chipmunk::BodyPtr&,
																				const Sandbox::Chipmunk::BodyPtr&,
																				const Vector2f&,const Vector2f&,
																				const Vector2f&))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GrooveJoint, GrooveA, GetGrooveA,SetGrooveA, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GrooveJoint, GrooveB, GetGrooveB,SetGrooveB, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GrooveJoint, Anchr2, GetAnchr2,SetAnchr2, Vector2f )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::DampedSpring,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::DampedSpring, (Sandbox::Chipmunk::Body,
																			 Sandbox::Chipmunk::Body,
																			 Vector2f,Vector2f,
																			 float,float,float
																			 ),(const Sandbox::Chipmunk::BodyPtr&,
																				const Sandbox::Chipmunk::BodyPtr&,
																				const Vector2f&,const Vector2f&,
																				float,float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedSpring, Anchr1, GetAnchr1,SetAnchr1, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedSpring, Anchr2, GetAnchr2,SetAnchr2, Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedSpring, RestLength, GetRestLength,SetRestLength, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedSpring, Stiffness, GetStiffness,SetStiffness, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedSpring, Damping, GetDamping,SetDamping, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::DampedRotarySpring,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::DampedRotarySpring, (Sandbox::Chipmunk::Body,
																			   Sandbox::Chipmunk::Body,
																			   float,float,float
																			   ),(const Sandbox::Chipmunk::BodyPtr&,
																				  const Sandbox::Chipmunk::BodyPtr&,
																				  float,float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedRotarySpring, RestAngle, GetRestAngle,SetRestAngle, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedRotarySpring, Stiffness, GetStiffness,SetStiffness, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedRotarySpring, Damping, GetDamping,SetDamping, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::RotaryLimitJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::RotaryLimitJoint, (Sandbox::Chipmunk::Body,
																					 Sandbox::Chipmunk::Body,
																					 float,float
																					 ),(const Sandbox::Chipmunk::BodyPtr&,
																						const Sandbox::Chipmunk::BodyPtr&,
																						float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::RotaryLimitJoint, Min, GetMin,SetMin, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::RotaryLimitJoint, Max, GetMax,SetMax, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::RatchetJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::RatchetJoint, (Sandbox::Chipmunk::Body,
																				   Sandbox::Chipmunk::Body,
																				   float,float
																				   ),(const Sandbox::Chipmunk::BodyPtr&,
																					  const Sandbox::Chipmunk::BodyPtr&,
																					  float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::RatchetJoint, Angle, GetAngle,SetAngle, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::RatchetJoint, Phase, GetPhase,SetPhase, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::RatchetJoint, Ratchet, GetRatchet,SetRatchet, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::GearJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::GearJoint, (Sandbox::Chipmunk::Body,
																			   Sandbox::Chipmunk::Body,
																			   float,float
																			   ),(const Sandbox::Chipmunk::BodyPtr&,
																				  const Sandbox::Chipmunk::BodyPtr&,
																				  float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GearJoint, Phase, GetPhase,SetPhase, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GearJoint, Ratio, GetRatio,SetRatio, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::SimpleMotor,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::SimpleMotor, (Sandbox::Chipmunk::Body,
																			Sandbox::Chipmunk::Body,
																			float
																			),(const Sandbox::Chipmunk::BodyPtr&,
																			   const Sandbox::Chipmunk::BodyPtr&,
																			   float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::SimpleMotor, Rate, GetRate,SetRate, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			
			
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::Space, Sandbox::Thread )
				SB_BIND_SHARED_CONSTRUCTOR( Sandbox::Chipmunk::Space, () )
				SB_BIND_BEGIN_METHODS
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, AddBody, void(Sandbox::Chipmunk::Body) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, AddShape, void(Sandbox::Chipmunk::Shape) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, AddConstraint, void(Sandbox::Chipmunk::Constraint) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, RemoveBody, void(Sandbox::Chipmunk::Body) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, RemoveShape, void(Sandbox::Chipmunk::Shape) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, RemoveConstraint, void(Sandbox::Chipmunk::Constraint) )
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Space, Gravity, GetGravity, SetGravity, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Space, Iterations, GetIterations, SetIterations, int )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::TransformAdapter, Sandbox::Thread )
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::TransformAdapter, (Sandbox::Chipmunk::Body,Sandbox::ContainerTransform),(const Sandbox::Chipmunk::BodyPtr&,const Sandbox::ContainerTransformPtr&) )
				SB_BIND_BEGIN_METHODS
				SB_BIND_METHOD( Sandbox::Chipmunk::TransformAdapter, Sync, void() )
				SB_BIND_END_METHODS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			SB_BIND_END_BIND
		}
	}
	
}