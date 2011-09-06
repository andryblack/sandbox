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
			{
				SB_BIND_BEGIN_SHARED_CLASS( Sandbox::Chipmunk::Shape )
				SB_BIND_NO_CONSTRUCTOR
				SB_BIND_BEGIN_METHODS
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
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
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::CircleShape,Sandbox::Chipmunk::Shape)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::CircleShape, (Sandbox::Chipmunk::Body,float,Sandbox::Vector2f),(const Sandbox::Chipmunk::BodyPtr&, float,const Sandbox::Vector2f&))
				SB_BIND_BEGIN_METHODS
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::SegmentShape,Sandbox::Chipmunk::Shape)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::SegmentShape, (Sandbox::Chipmunk::Body,Sandbox::Vector2f,Sandbox::Vector2f,float),(const Sandbox::Chipmunk::BodyPtr&, const Sandbox::Vector2f&,const Sandbox::Vector2f&,float))
				SB_BIND_BEGIN_METHODS
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::PolyShape,Sandbox::Chipmunk::Shape)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::PolyShape, (Sandbox::Chipmunk::Body, Sandbox::Vector2fList,Sandbox::Vector2f),(const Sandbox::Chipmunk::BodyPtr&, const std::vector<Vector2f>&,const Sandbox::Vector2f&))
				SB_BIND_BEGIN_METHODS
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
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
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Space, Gravity, GetGravity, SetGravity, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RO( Sandbox::Chipmunk::Space, StaticBody, GetStaticBody, Sandbox::Chipmunk::Body )
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
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
		}
	}
	
}