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
#include "sb_log.h"

extern "C" {
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
}


namespace Sandbox {

	namespace Chipmunk {
        
        
        
        
        static const char* const LuaChipmunkModule = "Sanbox:LuaChipmunk";
        class LuaCollisionHandler : public CollisionHandler {
        public:
            explicit LuaCollisionHandler(LuaHelperWeakPtr ptr,int a,int b) : CollisionHandler(a,b), m_ref(ptr) {}
            ~LuaCollisionHandler() {
            }
            void SetFunction(lua_State* L) {
                m_ref.SetObject(L);
            }
            void Handle( const ShapePtr& a, const ShapePtr& b ) {
                if (m_ref.Valid()) {
                    if ( LuaHelperPtr lua = m_ref.GetHelper()) {
                        lua_State* L = lua->lua->GetVM();
                        sb_assert(L);
                        m_ref.GetObject(L);
                        sb_assert(lua_isfunction(L,-1));
                        Bind::StackHelper stck_a(L,0,"Sandbox::Chipmunk::Shape");
                        stck_a.PushValue(a);
                        Bind::StackHelper stck_b(L,0,"Sandbox::Chipmunk::Shape");
                        stck_a.PushValue(b);
                        int res = lua_pcall(L, 2, 0, 0);
                        if (res) {
                            LogError(LuaChipmunkModule) << " Failed script Handle  " ;
                            LogError(LuaChipmunkModule) << lua_tostring(L, -1) ;
                        }
                    } else {
                        LogError(LuaChipmunkModule) <<" call Handle on released script" ;
                    }
                }            
            }
            static void constructor_func(const Bind::StackHelper* hpr) {
                lua_State* L = hpr->GetState();
                if (!lua_isnumber(L, 2)) {
                    luaL_argerror(L, 2, "a");
                    return;
                }
                int a = lua_tointeger(L, 2);
                if (!lua_isnumber(L, 3)) {
                    luaL_argerror(L, 3, "b");
                    return;
                }
                int b = lua_tointeger(L, 3);
                if (!lua_isfunction(L,4)) {
                    char buf[128];
                    ::snprintf(buf,127,"function expected, got %s",luaL_typename(L, 2));
                    luaL_argerror(L, 4, buf);
                    return;
                }
                Lua* lua = Lua::GetPtr(L);
                LuaCollisionHandler* raw = new LuaCollisionHandler(lua->GetHelper(),a,b);
                lua_State* main_state = lua->GetVM();
                lua_pushvalue(L, 4);
                if (main_state!=L) {
                    lua_xmove(L, main_state, 1);
                }
                raw->SetFunction(main_state);
                new (hpr->new_object_shared_ptr() ) shared_ptr<LuaCollisionHandler>( raw );
                return;
            }
        private:
            LuaReference	m_ref;
        };
        
        
		void Bind( Lua* lua ) {
			SB_BIND_BEGIN_BIND
            {
				SB_BIND_BEGIN_SHARED_CLASS( Sandbox::Chipmunk::CollisionHandler )
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_CLASS( Sandbox::Chipmunk::Shape )
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Shape, Sensor, GetSensor, SetSensor, bool )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Shape, Elasticity, GetElasticity, SetElasticity, float )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Shape, Friction, GetFriction, SetFriction, float )
                SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Shape, CollisionType, GetCollisionType, SetCollisionType, int )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
            {
                SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::Chipmunk::LuaCollisionHandler,Sandbox::Chipmunk::CollisionHandler )
                SB_BIND_SHARED_CUSTOM_CONSTRUCTOR(Sandbox::Chipmunk::LuaCollisionHandler,&LuaCollisionHandler::constructor_func)
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
																		   Sandbox::Vector2f,
																		   Sandbox::Vector2f
																		   ),(const Sandbox::Chipmunk::BodyPtr&,
																			  const Sandbox::Chipmunk::BodyPtr&,
																			  const Vector2f&,const Vector2f&))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PinJoint, Anchr1, GetAnchr1,SetAnchr1, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PinJoint, Anchr2, GetAnchr2,SetAnchr2, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PinJoint, Dist, GetDist,SetDist, float )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::SlideJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::SlideJoint, (Sandbox::Chipmunk::Body,
																		   Sandbox::Chipmunk::Body,
																		   Sandbox::Vector2f,
																			 Sandbox::Vector2f,float,float
																		   ),(const Sandbox::Chipmunk::BodyPtr&,
																			  const Sandbox::Chipmunk::BodyPtr&,
																			  const Vector2f&,const Vector2f&,float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::SlideJoint, Anchr1, GetAnchr1,SetAnchr1, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::SlideJoint, Anchr2, GetAnchr2,SetAnchr2, Sandbox::Vector2f )
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
																		   Sandbox::Vector2f,
																			 Sandbox::Vector2f
																		   ),(const Sandbox::Chipmunk::BodyPtr&,
																			  const Sandbox::Chipmunk::BodyPtr&,
																			  const Vector2f&,const Vector2f&))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PivotJoint, Anchr1, GetAnchr1,SetAnchr1, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::PivotJoint, Anchr2, GetAnchr2,SetAnchr2, Sandbox::Vector2f )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::GrooveJoint,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::GrooveJoint, (Sandbox::Chipmunk::Body,
																			 Sandbox::Chipmunk::Body,
																			 Sandbox::Vector2f,
																			  Sandbox::Vector2f,
																			  Sandbox::Vector2f
																			 ),(const Sandbox::Chipmunk::BodyPtr&,
																				const Sandbox::Chipmunk::BodyPtr&,
																				const Vector2f&,const Vector2f&,
																				const Vector2f&))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GrooveJoint, GrooveA, GetGrooveA,SetGrooveA, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GrooveJoint, GrooveB, GetGrooveB,SetGrooveB, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::GrooveJoint, Anchr2, GetAnchr2,SetAnchr2, Sandbox::Vector2f )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::DampedSpring,Sandbox::Chipmunk::Constraint)
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::DampedSpring, (Sandbox::Chipmunk::Body,
																			 Sandbox::Chipmunk::Body,
																			 Sandbox::Vector2f,
																			   Sandbox::Vector2f,
																			 float,float,float
																			 ),(const Sandbox::Chipmunk::BodyPtr&,
																				const Sandbox::Chipmunk::BodyPtr&,
																				const Vector2f&,const Vector2f&,
																				float,float,float))
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedSpring, Anchr1, GetAnchr1,SetAnchr1, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::DampedSpring, Anchr2, GetAnchr2,SetAnchr2, Sandbox::Vector2f )
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
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, AddCollisionHandler, void(Sandbox::Chipmunk::CollisionHandler) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, RemoveBody, void(Sandbox::Chipmunk::Body) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, RemoveShape, void(Sandbox::Chipmunk::Shape) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, RemoveConstraint, void(Sandbox::Chipmunk::Constraint) )
				SB_BIND_METHOD( Sandbox::Chipmunk::Space, RemoveCollisionHandler, void(Sandbox::Chipmunk::CollisionHandler) )
				SB_BIND_END_METHODS
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Space, Gravity, GetGravity, SetGravity, Sandbox::Vector2f )
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::Space, Iterations, GetIterations, SetIterations, int )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::TransformAdapter, Sandbox::ContainerTransform )
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::TransformAdapter, (Sandbox::Chipmunk::Body),(const Sandbox::Chipmunk::BodyPtr&) )
				SB_BIND_BEGIN_METHODS
				SB_BIND_END_METHODS
                SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::TransformAdapter, ApplyRotate, GetApplyRotate, SetApplyRotate, bool )
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::SpaceDebugDraw, Sandbox::Object )
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::SpaceDebugDraw, (Sandbox::Chipmunk::Space),(const Sandbox::Chipmunk::SpacePtr&) )
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			{
				SB_BIND_BEGIN_SHARED_SUBCLASS( Sandbox::Chipmunk::BodyDebugDraw, Sandbox::Object )
				SB_BIND_SHARED_CONSTRUCTOR_( Sandbox::Chipmunk::BodyDebugDraw, (Sandbox::Chipmunk::Body),(const Sandbox::Chipmunk::BodyPtr&) )
				SB_BIND_BEGIN_PROPERTYS
				SB_BIND_PROPERTY_RW( Sandbox::Chipmunk::BodyDebugDraw, Color, GetColor,SetColor,Sandbox::Color)
				SB_BIND_END_PROPERTYS
				SB_BIND_END_CLASS
				SB_BIND( lua );
			}
			SB_BIND_END_BIND
		}
	}
	
}