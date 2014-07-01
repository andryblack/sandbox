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
#include "luabind/sb_luabind.h"
#include "sb_lua.h"
#include "sb_log.h"


SB_META_DECLARE_KLASS(Sandbox::Chipmunk::Shape, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::Shape)
SB_META_PROPERTY_RW_DEF(Sensor)
SB_META_PROPERTY_RW_DEF(Elasticity)
SB_META_PROPERTY_RW_DEF(Friction)
SB_META_PROPERTY_RW_DEF(CollisionType)
SB_META_PROPERTY_RW_DEF(Layers)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::Body, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::Body)
SB_META_CONSTRUCTOR((float,float))
SB_META_METHOD(Local2World)
SB_META_METHOD(World2Local)
SB_META_METHOD(ResetForces)
SB_META_METHOD(Activate)
SB_META_METHOD(Sleep)
SB_META_METHOD(ApplyForce)
SB_META_METHOD(ApplyImpulse)
SB_META_METHOD(UpdatePosition)
SB_META_PROPERTY_RW_DEF(Mass)
SB_META_PROPERTY_RW_DEF(Moment)
SB_META_PROPERTY_RW_DEF(Pos)
SB_META_PROPERTY_RW_DEF(Vel)
SB_META_PROPERTY_RW_DEF(Force)
SB_META_PROPERTY_RW_DEF(Angle)
SB_META_PROPERTY_RW_DEF(AngVel)
SB_META_PROPERTY_RW_DEF(Torque)
SB_META_PROPERTY_RO(Rot,GetRot)
SB_META_PROPERTY_RW_DEF(VelLimit)
SB_META_PROPERTY_RW_DEF(AngVelLimit)
SB_META_PROPERTY_RO(IsSleeping,IsSleeping)
SB_META_PROPERTY_RO(IsStatic,IsStatic)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::StaticBody, Sandbox::Chipmunk::Body)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::StaticBody)
SB_META_CONSTRUCTOR(())
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::RogueBody, Sandbox::Chipmunk::Body)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::RogueBody)
SB_META_CONSTRUCTOR(())
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::CircleShape, Sandbox::Chipmunk::Shape)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::CircleShape)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::BodyPtr&, float,const Sandbox::Vector2f&))
SB_META_PROPERTY_RO(Radius,GetRadius)
SB_META_PROPERTY_RO(Offset,GetOffset)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::SegmentShape, Sandbox::Chipmunk::Shape)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::SegmentShape)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::BodyPtr&, const Sandbox::Vector2f&,const Sandbox::Vector2f&,float))
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::PolyShape, Sandbox::Chipmunk::Shape)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::PolyShape)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::BodyPtr&,  const std::vector<Vector2f>&,const Sandbox::Vector2f&))
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::BoxShape, Sandbox::Chipmunk::Shape)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::BoxShape)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::BodyPtr&,  float,float))
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::Space)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddBody)
SB_META_METHOD(AddShape)
SB_META_METHOD(AddConstraint)
SB_META_METHOD(AddCollisionHandler)
SB_META_METHOD(RemoveBody)
SB_META_METHOD(RemoveShape)
SB_META_METHOD(RemoveConstraint)
SB_META_METHOD(RemoveCollisionHandler)
SB_META_PROPERTY_RW_DEF(Gravity)
SB_META_PROPERTY_RW_DEF(Iterations)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::TransformAdapter, Sandbox::Container)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::TransformAdapter)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::BodyPtr&))
SB_META_PROPERTY_RW_DEF(ApplyRotate)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::InvertTransformAdapter, Sandbox::Container)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::InvertTransformAdapter)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::BodyPtr&))
SB_META_PROPERTY_RW_DEF(ApplyRotate)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::SpaceDebugDraw)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::SpacePtr&))
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::BodyDebugDraw)
SB_META_CONSTRUCTOR((const Sandbox::Chipmunk::BodyPtr&))
SB_META_PROPERTY_RW_DEF(Color)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::Constraint, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::Constraint)
SB_META_PROPERTY_RW_DEF(MaxForce)
SB_META_PROPERTY_RW_DEF(MaxBias)
SB_META_PROPERTY_RO(A,GetA)
SB_META_PROPERTY_RO(B,GetB)
SB_META_PROPERTY_RO(Impulse,GetImpulse)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::CollisionHandler, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::CollisionHandler)
SB_META_END_KLASS_BIND()


namespace Sandbox {

	namespace Chipmunk {
        
        
        
        static const char* const LuaChipmunkModule = "Sanbox:LuaChipmunk";
        class LuaCollisionHandler : public CollisionHandler {
        public:
            explicit LuaCollisionHandler(luabind::LuaVMHelperWeakPtr ptr,long a,long b) : CollisionHandler(a,b), m_ref(ptr) {}
            ~LuaCollisionHandler() {
            }
            void SetFunction(lua_State* L) {
                m_ref.SetObject(L);
            }
            void Handle( const ShapePtr& a, const ShapePtr& b ) {
                if (m_ref.Valid()) {
                    if ( luabind::LuaVMHelperPtr lua = m_ref.GetHelper()) {
                        lua_State* L = lua->lua;
                        sb_assert(L);
                        m_ref.GetObject(L);
                        sb_assert(lua_isfunction(L,-1));
                        luabind::stack<ShapePtr>::push(L, a);
                        luabind::stack<ShapePtr>::push(L, b);
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
        public:
            static int constructor_func(lua_State* L) {
                if (!lua_isnumber(L, 2)) {
                    luabind::lua_argerror(L, 2, "number",0);
                    return 0;
                }
                long a = lua_tointeger(L, 2);
                if (!lua_isnumber(L, 3)) {
                    luabind::lua_argerror(L, 3, "number",0);
                    return 0;
                }
                long b = lua_tointeger(L, 3);
                if (!lua_isfunction(L,4)) {
                    luabind::lua_argerror(L, 4, "function",0);
                    return 0;
                }
                luabind::LuaVMHelperPtr helper = luabind::GetHelper(L);
                sb_assert(helper);
                LuaCollisionHandler* raw = new LuaCollisionHandler(helper,a,b);
                lua_State* main_state = helper->lua;
                lua_pushvalue(L, 4);
                if (main_state!=L) {
                    lua_xmove(L, main_state, 1);
                }
                raw->SetFunction(main_state);
                luabind::stack<sb::intrusive_ptr<LuaCollisionHandler> >::push(L, sb::intrusive_ptr<LuaCollisionHandler>( raw ));
                return 1;
            }
        private:
            luabind::LuaReference	m_ref;
        };

    }
}

SB_META_DECLARE_KLASS(Sandbox::Chipmunk::LuaCollisionHandler, Sandbox::Chipmunk::CollisionHandler)
SB_META_BEGIN_KLASS_BIND(Sandbox::Chipmunk::LuaCollisionHandler)
bind(constructor(&Sandbox::Chipmunk::LuaCollisionHandler::constructor_func));
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
	namespace Chipmunk {
        
		void Bind( LuaVM* lua ) {
            luabind::Class<Shape>(lua->GetVM());
            luabind::Class<Body>(lua->GetVM());
            luabind::Class<StaticBody>(lua->GetVM());
            luabind::Class<RogueBody>(lua->GetVM());
            luabind::Class<CircleShape>(lua->GetVM());
            luabind::Class<SegmentShape>(lua->GetVM());
            luabind::Class<PolyShape>(lua->GetVM());
            luabind::Class<BoxShape>(lua->GetVM());
            luabind::Class<Space>(lua->GetVM());
            luabind::Class<TransformAdapter>(lua->GetVM());
            luabind::Class<InvertTransformAdapter>(lua->GetVM());
            luabind::Class<SpaceDebugDraw>(lua->GetVM());
            luabind::Class<BodyDebugDraw>(lua->GetVM());
            luabind::Class<Constraint>(lua->GetVM());
            luabind::Class<CollisionHandler>(lua->GetVM());
            luabind::Class<LuaCollisionHandler>(lua->GetVM());
            
            
#if 0
			SB_BIND_BEGIN_BIND
            {
				SB_BIND_BEGIN_SHARED_CLASS( Sandbox::Chipmunk::CollisionHandler )
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
			
			
			
			
			SB_BIND_END_BIND
#endif

		}

	}
	
}