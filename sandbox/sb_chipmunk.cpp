/*
 *  sb_chipmunk.cpp
 *  YinYang
 *
 *  Created by Андрей Куницын on 04.09.11.
 *  Copyright 2011 AndryBlack. All rights reserved.
 *
 */

#include "sb_chipmunk.h"
#include "../chipmunk/include/chipmunk/chipmunk.h"
#include <algorithm>
#include "sb_graphics.h"
#include "sb_log.h"

namespace Sandbox {

    static const char* MODULE = "Sandbox:Chipmunk";
	
	namespace Chipmunk {
		
		template <class T> 
		inline bool add( std::vector<T>& objects, const T& object ) {
			typename std::vector<T>::iterator i = std::find(objects.begin(),objects.end(),object);
			if (i==objects.end()) {
				objects.push_back(object);
				return true;
			}
			return false;
		}
		template <class T> 
		inline bool remove( std::vector<T>& objects, const T& object ) {
			typename std::vector<T>::iterator i = std::find(objects.begin(),objects.end(),object);
			if (i!=objects.end()) {
				objects.erase(i);
				return true;
			}
			return false;
		}
		template <class T>
		struct compare_object {
			explicit compare_object(const T* raw_) : raw(raw_) {}
			bool operator() ( const sb::shared_ptr<T>& ptr) {
				return ptr.get() == raw;
			}
			const T* raw;
		};
		template <class T> 
		inline bool remove( std::vector<sb::shared_ptr<T> >& objects, const T* object ) {
			typename std::vector<sb::shared_ptr<T> >::iterator i = std::find_if(objects.begin(),objects.end(),compare_object<T>(object));
			if (i!=objects.end()) {
				objects.erase(i);
				return true;
			}
			return false;
		}
		
		
		static inline cpVect vect( const Vector2f& v) {
			return cpv(v.x,v.y);
		}
		
		static inline Vector2f vect( const cpVect& v) {
			return Vector2f(float(v.x),float(v.y));
		}
	
		

		Space::Space() {
			m_time_cum = 0.0f;
			m_space = cpSpaceNew();
            cpSpaceSetUserData(m_space, this);
		}
		
		Space::~Space() {
            m_postprocess_collisions.clear();
			for (size_t i=0;i<m_bodies.size();i++) {
				cpSpaceRemoveBody(m_space, m_bodies[i]->get_body());
			}
            m_bodies.clear();
			for (size_t i=0;i<m_shapes.size();i++) {
				cpSpaceRemoveShape(m_space, m_shapes[i]->get_shape());
			}
            m_shapes.clear();
			for (size_t i=0;i<m_constraints.size();i++) {
				cpSpaceRemoveConstraint(m_space, m_constraints[i]->get_constraint());
			}
			cpSpaceFree(m_space);
            LogDebug(MODULE) << "Space::~Space";
		}
		
		void Space::SetGravity(const Vector2f& g) {
			cpSpaceSetGravity(m_space, vect(g));
		}
		Vector2f Space::GetGravity() const {
			return vect(cpSpaceGetGravity(m_space));
		}
		int Space::GetIterations() const {
			return cpSpaceGetIterations(m_space);
		}
		void Space::SetIterations(int i) {
			cpSpaceSetIterations(m_space, i);
		}
		void Space::AddBody(const BodyPtr& body) {
			if (add(m_bodies,body)) {
				cpSpaceAddBody(m_space, body->get_body());
			}
		}
		void Space::RemoveBody(const BodyPtr& body) {
			if (remove(m_bodies,body)) {
				cpSpaceRemoveBody(m_space, body->get_body());
			}
		}
		void Space::AddShape(const ShapePtr& shape) {
			if (add(m_shapes,shape)) {
				shape->set_space( this );
				cpSpaceAddShape(m_space, shape->get_shape());
			}
		}
		void Space::RemoveShape( const ShapePtr& shape) {
			if (remove(m_shapes,shape)) {
				shape->set_space( 0 );
				cpSpaceRemoveShape(m_space, shape->get_shape());
			}
		}
		void Space::AddConstraint(const ConstraintPtr& constraint) {
			if (add(m_constraints,constraint))
				cpSpaceAddConstraint(m_space, constraint->get_constraint());
		}
		void Space::RemoveConstraint(const ConstraintPtr& constraint) {
			if (remove(m_constraints,constraint))
				cpSpaceRemoveConstraint(m_space, constraint->get_constraint());
		}
		void Space::Step( float dt ) {
			cpSpaceStep(m_space, dt);
		}
		bool Space::Update( float dt ) {
			m_time_cum += dt;
			static const float static_step = 1.0f / 50.0f;
			size_t steps = size_t( m_time_cum / static_step );
			if (steps>5) steps = 5;
			if ( steps ) {
				for (size_t i=0;i<steps;i++) {
					cpSpaceStep(m_space, static_step );
                    for (std::vector<collision>::iterator i=m_postprocess_collisions.begin();
                         i!=m_postprocess_collisions.end();++i) {
                        process_collision(*i);
                    }
                    m_postprocess_collisions.clear();
				}
				m_time_cum -= static_step * steps;
			}
    		return false;
		}
        void Space::process_collision( const collision& c ) {
            c.handler->Handle(c.a, c.b);
        }
        int Space::collision_begin(cpArbiter *arb, struct cpSpace *space, void *data) {
            CollisionHandler* h = static_cast<CollisionHandler*>(data);
            if (!h) return 0;
            cpShape *a, *b; cpArbiterGetShapes(arb, &a, &b);
            Space* _this = static_cast<Space*>(cpSpaceGetUserData(space));
            Shape* sa = static_cast<Shape*>(cpShapeGetUserData(a));
            Shape* sb = static_cast<Shape*>(cpShapeGetUserData(b));
            CollisionHandlerPtr handler = h->shared_from_this();
            if (sa && sb)
                _this->add_collision_postprocess( collision(handler,
                                                            sa->shared_from_this(),
                                                            sb->shared_from_this()) );
            return 1;
        }
        CollisionHandler::CollisionHandler( long a, long b ) : m_collision_a(a), m_collision_b(b) {
            
        }
		void Space::AddCollisionHandler(const CollisionHandlerPtr& handler ) {
            if (add(m_collision_handlers,handler)) {
                cpSpaceAddCollisionHandler(m_space, 
                                           handler->GetCollisionTypeA(), 
                                           handler->GetCollisionTypeB(), 
                                           &Space::collision_begin, 
                                           0, 0, 0, 
                                           handler.get());
            }
        }
        
        void Space::RemoveCollisionHandler( const CollisionHandlerPtr& handler ) {
            if (remove(m_collision_handlers,handler)) {
                cpSpaceRemoveCollisionHandler(m_space, handler->GetCollisionTypeA(), 
                                              handler->GetCollisionTypeB());
            }
        }
		
		
		Shape::Shape() : m_shape(0),m_space(0) {
		}
		
		Shape::~Shape() {
			LogDebug(MODULE) << "Shape::~Shape";
			if(m_shape) {
                cpShapeSetUserData(m_shape, 0);
                cpShapeFree(m_shape);
            }
		}
		
		void Shape::SetShape(cpShape* shape) {
			m_shape = shape;
            if (m_shape)
                cpShapeSetUserData(m_shape, this);
		}
		float Shape::GetFriction() const {
			return float(cpShapeGetFriction(m_shape));
		}
		void Shape::SetFriction(float f) {
			cpShapeSetFriction(m_shape, f);
		}
		bool Shape::GetSensor() const {
			return cpShapeGetSensor(m_shape)==cpTrue;
		}
		void Shape::SetSensor(bool s) {
			cpShapeSetSensor(m_shape, s?cpTrue:cpFalse);
		}
		
		float Shape::GetElasticity() const {
			return float(cpShapeGetElasticity(m_shape));
		}
		void Shape::SetElasticity(float e) {
			cpShapeSetElasticity(m_shape, e);
		}
        
        long Shape::GetCollisionType() const {
            return cpShapeGetCollisionType( m_shape );
        }
        void Shape::SetCollisionType( long type ) {
            cpShapeSetCollisionType( m_shape, type );
        }
        
       	
			
		CircleShape::CircleShape( const BodyPtr& body, float radius, const Vector2f& pos ) {
			SetShape(cpCircleShapeNew(body->get_body(),radius,vect(pos)));
		}
		Vector2f CircleShape::GetOffset() const {
			return vect(cpCircleShapeGetOffset(m_shape));
		}
		float	CircleShape::GetRadius() const {
			return float(cpCircleShapeGetRadius(m_shape));
		}
        
        
		
		PolyShape::PolyShape( const BodyPtr& body, const std::vector<Vector2f>& points, const Vector2f& offset )
        {
			cpVect* v = new cpVect[points.size()];
			for (size_t i=0;i<points.size();i++) {
				v[i].x = points[i].x;
				v[i].y = points[i].y;
			}
			SetShape(cpPolyShapeNew(body->get_body(),int(points.size()),v,cpv(offset.x,offset.y)));
			delete [] v;
		}
		
		SegmentShape::SegmentShape( const BodyPtr& body, const Vector2f& a,const Vector2f& b,float radius) {
			SetShape(cpSegmentShapeNew(body->get_body(),vect(a),vect(b),radius));
		}
			
		BoxShape::BoxShape( const BodyPtr& body, float w, float h ) {
			SetShape(cpBoxShapeNew(body->get_body(), w, h));
		}
		
		Body::Body( float mass, float inertia )  {
			m_body = cpBodyNew(mass, inertia);
			cpBodySetUserData(m_body, this);
		}
		Body::Body(cpBody* b) :m_body(b) {
			cpBodySetUserData(m_body, this);
		}
		
		static void FreeBodyShapeIteratorFunc(cpBody *, cpShape *shape, void *) {
			Shape* shape_p = reinterpret_cast<Shape*>( cpShapeGetUserData(shape) );
			if (shape_p) {
				Space* space = shape_p->get_space();
				if (space) {
					space->RemoveShape(shape_p->shared_from_this());
				}
			}
		}
			
		Body::~Body() {
			LogDebug(MODULE) << "Body::~Body";
			if (m_body) {
				cpBodyEachShape(m_body, FreeBodyShapeIteratorFunc,0 );
				cpBodyFree(m_body);
			}
		}
		
		float Body::GetMass() const {
			return float(cpBodyGetMass(m_body));
		}
		void Body::SetMass(float m) {
			cpBodySetMass(m_body, m);
		}
		float Body::GetMoment() const {
			return float(cpBodyGetMoment(m_body));
		}
		void Body::SetMoment(float i) {
			cpBodySetMoment(m_body, i);
		}
		Vector2f Body::GetPos() const {
			cpVect p = cpBodyGetPos(m_body);
			return Vector2f( float(p.x),float(p.y) );
		}
		void Body::SetPos(const Vector2f& pos){
			cpBodySetPos(m_body, vect(pos));
		}
		Vector2f Body::GetVel() const {
			return vect(cpBodyGetVel(m_body));
		}
		void Body::SetVel(const Vector2f& v){
			cpBodySetVel(m_body, vect(v));
		}
		Vector2f Body::GetForce() const{
			return vect(cpBodyGetForce(m_body));
		}
		void Body::SetForce(const Vector2f& force) {
			cpBodySetForce(m_body, vect(force));
		}
		float Body::GetAngle() const {
			return float(cpBodyGetAngle(m_body));
		}
		void Body::SetAngle(float a) {
			cpBodySetAngle(m_body, a);
		}
		float Body::GetAngVel() const {
			return float(cpBodyGetAngVel(m_body));
		}
		void Body::SetAngVel(float v) {
			cpBodySetAngVel(m_body, v);
		}
		float Body::GetTorque() const {
			return float(cpBodyGetTorque(m_body));
		}
		void Body::SetTorque(float t) {
			cpBodySetTorque(m_body, t);
		}
		Vector2f Body::GetRot() const {
			return vect(cpBodyGetRot(m_body));
		}
		float Body::GetVelLimit() const {
			return float(cpBodyGetVelLimit(m_body));
		}
		void Body::SetVelLimit(float v) {
			cpBodySetVelLimit(m_body, v);
		}
		float Body::GetAngVelLimit() const {
			return float(cpBodyGetAngVelLimit(m_body));
		}
		void Body::SetAngVelLimit(float v) {
			cpBodySetAngVelLimit(m_body, v);
		}
		Vector2f Body::Local2World(const Vector2f& pos) const {
			return vect(cpBodyLocal2World(m_body, vect(pos)));
		}
		Vector2f Body::World2Local(const Vector2f& pos) const {
			return vect(cpBodyWorld2Local( m_body, vect(pos)));
		}
		void Body::ResetForces() {
			cpBodyResetForces(m_body);
		}
		void Body::ApplyForce(const Vector2f& f, const Vector2f& r) {
			cpBodyApplyForce(m_body, vect(f), vect(r));
		}
		void Body::ApplyImpulse(const Vector2f& j, const Vector2f& r) {
			cpBodyApplyImpulse(m_body, vect(j), vect(r));
		}
		bool Body::IsSleeping() const {
			return cpBodyIsSleeping(m_body)==cpTrue;
		}
		void Body::Activate() {
			cpBodyActivate(m_body);
		}
		void Body::Sleep() {
			cpBodySleep(m_body);
		}
		bool Body::IsStatic() const{
			return cpBodyIsStatic(m_body)==cpTrue;
		}

		StaticBody::StaticBody() : Body(cpBodyNewStatic()){
		}

		
		Constraint::Constraint( cpConstraint* c ) : m_constraint(c) {
		}
		
		Constraint::~Constraint() {
			cpConstraintFree(m_constraint);
		}
		
		BodyPtr Constraint::GetA() const {
			void* d = cpBodyGetUserData(cpConstraintGetA(m_constraint));
			Body* body = reinterpret_cast<Body*> (d);
			return body ? body->shared_from_this() : BodyPtr();
		}
		
		BodyPtr Constraint::GetB() const {
			void* d = cpBodyGetUserData(cpConstraintGetB(m_constraint));
			Body* body = reinterpret_cast<Body*> (d);
			return body ? body->shared_from_this() : BodyPtr();
		}
		
		
		float Constraint::GetMaxForce() const {
			return float(cpConstraintGetMaxForce(m_constraint));
		}
		void Constraint::SetMaxForce(float f) {
			cpConstraintSetMaxForce(m_constraint, f);
		}
		float Constraint::GetMaxBias() const {
			return float(cpConstraintGetMaxBias(m_constraint));
		}
		void Constraint::SetMaxBias(float b) {
			cpConstraintSetMaxBias(m_constraint, b);
		}
		float Constraint::GetImpulse() const {
			return float(cpConstraintGetImpulse(m_constraint));
		}
		
		
		
		
		PinJoint::PinJoint(const BodyPtr& a,const BodyPtr& b,
						   const Vector2f& anchr1, const Vector2f& anchr2 ) 
		: Constraint(cpPinJointNew(a->get_body(), b->get_body(), vect(anchr1), vect(anchr2))){
			
		}
		Vector2f PinJoint::GetAnchr1() const {
			return vect(cpPinJointGetAnchr1(m_constraint));
		}
		void PinJoint::SetAnchr1(const Vector2f& v) {
			cpPinJointSetAnchr1(m_constraint, vect(v));
		}
		Vector2f PinJoint::GetAnchr2() const {
			return vect(cpPinJointGetAnchr2(m_constraint));
		}
		void PinJoint::SetAnchr2(const Vector2f& v) {
			cpPinJointSetAnchr2(m_constraint, vect(v));
		}
		float PinJoint::GetDist() const {
			return float(cpPinJointGetDist(m_constraint));
		}
		void PinJoint::SetDist(float d) {
			cpPinJointSetDist(m_constraint, d);
		}
		
		
		
		
		SlideJoint::SlideJoint(const BodyPtr& a,const BodyPtr& b,const Vector2f& anchr1, 
				   const Vector2f& anchr2, float min,float max )
		: Constraint(cpSlideJointNew(a->get_body(), b->get_body(), vect(anchr1), vect(anchr2), min, max)){
		}
		Vector2f SlideJoint::GetAnchr1() const {
			return vect(cpSlideJointGetAnchr1(m_constraint));
		}
		void SlideJoint::SetAnchr1(const Vector2f& v) {
			cpSlideJointSetAnchr1(m_constraint, vect(v));
		}
		Vector2f SlideJoint::GetAnchr2() const {
			return vect(cpSlideJointGetAnchr2(m_constraint));
		}
		void SlideJoint::SetAnchr2(const Vector2f& v) {
			cpSlideJointSetAnchr2(m_constraint, vect(v));
		}
		float SlideJoint::GetMin() const {
			return float(cpSlideJointGetMin(m_constraint));
		}
		void SlideJoint::SetMin(float d) {
			cpSlideJointSetMin(m_constraint, d);
		}
		float SlideJoint::GetMax() const {
			return float(cpSlideJointGetMax(m_constraint));
		}
		void SlideJoint::SetMax(float d) {
			cpSlideJointSetMax(m_constraint, d);
		}
		
		
		PivotJoint::PivotJoint(const BodyPtr& a,const BodyPtr& b,
				   const Vector2f& anchr1, const Vector2f& anchr2 )
		: Constraint(cpPivotJointNew2(a->get_body(), b->get_body(), vect(anchr1),vect(anchr2))){
		}
		Vector2f PivotJoint::GetAnchr1() const {
			return vect(cpPivotJointGetAnchr1(m_constraint));
		}
		void PivotJoint::SetAnchr1(const Vector2f& v) {
			cpPivotJointSetAnchr1(m_constraint, vect(v));
		}
		Vector2f PivotJoint::GetAnchr2() const {
			return vect(cpPivotJointGetAnchr2(m_constraint));
		}
		void PivotJoint::SetAnchr2(const Vector2f& v) {
			cpPivotJointSetAnchr2(m_constraint, vect(v));
		}
		
		GrooveJoint::GrooveJoint(const BodyPtr& a,const BodyPtr& b,
					const Vector2f& grove_a, const Vector2f& grove_b, 
					const Vector2f& anchr2 )
		: Constraint(cpGrooveJointNew(a->get_body(), b->get_body(),
									  vect(grove_a), vect(grove_b), vect(anchr2))) {
		}
		Vector2f GrooveJoint::GetGrooveA() const {
			return vect(cpGrooveJointGetGrooveA(m_constraint));
		}
		void GrooveJoint::SetGrooveA(const Vector2f& v) {
			cpGrooveJointSetGrooveB(m_constraint, vect(v));
		}
		Vector2f GrooveJoint::GetGrooveB() const {
			return vect(cpGrooveJointGetGrooveB(m_constraint));
		}
		void GrooveJoint::SetGrooveB(const Vector2f& v) {
			cpGrooveJointSetGrooveB(m_constraint, vect(v));
		}
		Vector2f GrooveJoint::GetAnchr2() const {
			return vect(cpGrooveJointGetAnchr2(m_constraint)); 
		}
		void GrooveJoint::SetAnchr2(const Vector2f& v) {
			cpGrooveJointSetAnchr2(m_constraint, vect(v));
		}
		
		DampedSpring::DampedSpring(const BodyPtr& a,const BodyPtr& b,
					 const Vector2f& anchr1, const Vector2f& anchr2,
					 float restLength, float stiffness, float damping)
		: Constraint(cpDampedSpringNew(a->get_body(), b->get_body(),
									   vect(anchr1), vect(anchr2),
									   restLength, stiffness, damping)) {
		}
		Vector2f DampedSpring::GetAnchr1() const {
			return vect(cpDampedSpringGetAnchr1(m_constraint));
		}
		void DampedSpring::SetAnchr1(const Vector2f& v) {
			cpDampedSpringSetAnchr1(m_constraint, vect(v));
		}
		Vector2f DampedSpring::GetAnchr2() const {
			return vect(cpDampedSpringGetAnchr2(m_constraint));
		}
		void DampedSpring::SetAnchr2(const Vector2f& v) {
			cpDampedSpringSetAnchr2(m_constraint, vect(v));
		}
		float DampedSpring::GetRestLength() const {
			return float(cpDampedSpringGetRestLength(m_constraint));
		}
		void DampedSpring::SetRestLength(float l) {
			cpDampedSpringSetRestLength(m_constraint, l);
		}
		float DampedSpring::GetStiffness() const {
			return float(cpDampedSpringGetStiffness(m_constraint));
		}
		void DampedSpring::SetStiffness(float s) {
			cpDampedSpringSetStiffness(m_constraint, s);
		}
		float DampedSpring::GetDamping() const {
			return float(cpDampedSpringGetDamping(m_constraint));
		}
		void DampedSpring::SetDamping(float d) {
			cpDampedSpringSetDamping(m_constraint, d);
		}

		
		DampedRotarySpring::DampedRotarySpring(const BodyPtr& a,const BodyPtr& b,
						   float restAngle, float stiffness, float damping)
		: Constraint(cpDampedRotarySpringNew(a->get_body(),b->get_body(),
											 restAngle,stiffness,damping)) {
		}
		float DampedRotarySpring::GetRestAngle() const {
			return float(cpDampedRotarySpringGetRestAngle(m_constraint));
		}
		void DampedRotarySpring::SetRestAngle(float l) {
			cpDampedRotarySpringSetRestAngle(m_constraint, l);
		}
		float DampedRotarySpring::GetStiffness() const {
			return float(cpDampedRotarySpringGetStiffness(m_constraint));
		}
		void DampedRotarySpring::SetStiffness(float s) {
			cpDampedRotarySpringSetStiffness(m_constraint, s);
		}
		float DampedRotarySpring::GetDamping() const {
			return float(cpDampedRotarySpringGetDamping(m_constraint));
		}
		void DampedRotarySpring::SetDamping(float d) {
			cpDampedRotarySpringSetDamping(m_constraint, d);
		}
		
		
		RotaryLimitJoint::RotaryLimitJoint(const BodyPtr& a,const BodyPtr& b,
						 float min, float max)
		: Constraint(cpRotaryLimitJointNew(a->get_body(), b->get_body(), min, max)) {
		}
		float RotaryLimitJoint::GetMin() const {
			return float(cpRotaryLimitJointGetMin(m_constraint));
		}
		void RotaryLimitJoint::SetMin(float m) {
			cpRotaryLimitJointSetMin(m_constraint, m);
		}
		float RotaryLimitJoint::GetMax() const {
			return float(cpRotaryLimitJointGetMax(m_constraint));
		}
		void RotaryLimitJoint::SetMax(float m) {
			cpRotaryLimitJointSetMax(m_constraint, m);
		}
		
		
		
		RatchetJoint::RatchetJoint(const BodyPtr& a,const BodyPtr& b,
					 float phase, float ratchet)
		: Constraint(cpRatchetJointNew(a->get_body(), b->get_body(), phase, ratchet)) {
		}
		float RatchetJoint::GetAngle() const {
			return float(cpRatchetJointGetAngle(m_constraint));
		}
		void RatchetJoint::SetAngle(float m) {
			cpRatchetJointSetAngle(m_constraint, m);
		}
		float RatchetJoint::GetPhase() const {
			return float(cpRatchetJointGetPhase(m_constraint));
		}
		void RatchetJoint::SetPhase(float m) {
			cpRatchetJointSetPhase(m_constraint, m);
		}
		float RatchetJoint::GetRatchet() const {
			return float(cpRatchetJointGetRatchet(m_constraint));
		}
		void RatchetJoint::SetRatchet(float m) {
			cpRatchetJointSetRatchet(m_constraint, m);
		}
		
		
		
		GearJoint::GearJoint(const BodyPtr& a,const BodyPtr& b,
				  float phase, float ratio)
		: Constraint(cpGearJointNew(a->get_body(), b->get_body(), phase, ratio)) {
		}
		float GearJoint::GetPhase() const {
			return float(cpGearJointGetPhase(m_constraint));
		}
		void GearJoint::SetPhase(float m) {
			cpGearJointSetPhase(m_constraint, m);
		}
		float GearJoint::GetRatio() const {
			return float(cpGearJointGetRatio(m_constraint));
		}
		void GearJoint::SetRatio(float m) {
			cpGearJointSetRatio(m_constraint, m);
		}
		
		SimpleMotor::SimpleMotor(const BodyPtr& a,const BodyPtr& b,
					float rate)
		: Constraint(cpSimpleMotorNew(a->get_body(), b->get_body(), rate)) {
		}
		float SimpleMotor::GetRate() const {
			return float(cpSimpleMotorGetRate(m_constraint));
		}
		void SimpleMotor::SetRate(float r) {
			cpSimpleMotorSetRate(m_constraint, r);
		}
		
		
		TransformAdapter::TransformAdapter( const BodyPtr& body ) {
			m_body = body;
            m_apply_rotate = true;
		}
		
		void TransformAdapter::Update(float ) {
            SetTranslate( m_body->GetPos() );
            if (m_apply_rotate)
                SetAngle( m_body->GetAngle() );
		}
		
		
		
		
		class DebugDrawImpl {
		public:
			static void Draw( Graphics& g, cpSpace* space )  {
				cpSpaceEachBody(space, &SpaceBodyIteratorFunc, &g);
				cpSpaceEachShape(space, &SpaceShapeIteratorFunc, &g);	
				cpSpaceEachConstraint(space, &SpaceConstraintIteratorFunc, &g);
			}
			static void Draw( Graphics& g, cpBody* body , const Color& color)  {
				Color c = g.GetColor();
				g.SetColor(c*color);
				Transform2d tr = g.GetTransform();
				g.SetTransform(tr.translated(vect(cpBodyGetPos(body))).rotated(float(cpBodyGetAngle(body))));
				cpBodyEachShape(body, &BodyShapeIteratorFunc, &g);
				g.SetTransform(tr);
				g.SetColor(c);
			}
			static void SpaceBodyIteratorFunc(cpBody *body, void *data) {
				Graphics* g = reinterpret_cast<Graphics*> (data);
				Draw(*g,body,Color(1,0,0,1));
			}
			static void SpaceConstraintIteratorFunc(cpConstraint *constraint, void *data) {
				Graphics* g = reinterpret_cast<Graphics*> (data);
				cpBody* A = cpConstraintGetA(constraint);
				cpBody* B = cpConstraintGetB(constraint);
				Vector2f posA = vect(cpBodyGetPos(A));
				Vector2f posB = vect(cpBodyGetPos(B));
				g->DrawLine(posA,posB,Color(0,1,0,1));
			}
			static void SpaceShapeIteratorFunc(cpShape *shape, void *data) {
				//Graphics* g = reinterpret_cast<Graphics*> (data);
				cpBody* b = cpShapeGetBody(shape);
				if (b && cpBodyIsStatic(b)) {
					BodyShapeIteratorFunc(b,shape,data);
				}
			}
			static void BodyShapeIteratorFunc(cpBody *, cpShape *shape, void *data) {
				Graphics* g = reinterpret_cast<Graphics*> (data);
				if (CP_CIRCLE_SHAPE==shape->klass_private->type) {
					Vector2f pos = vect(cpCircleShapeGetOffset(shape));
					float r = float(cpCircleShapeGetRadius(shape));
					g->DrawCircle(pos,r);
					g->DrawLine(pos, Vector2f(0,1)*r);
				} else if (CP_POLY_SHAPE==shape->klass_private->type) {
					size_t cnt = cpPolyShapeGetNumVerts(shape);
					std::vector<Vector2f> points;
					for (size_t i=0;i<cnt;i++) {
						points.push_back(vect(cpPolyShapeGetVert(shape, int(i))));
					}
					points.push_back(points.front());
					g->DrawLineStrip(points);
				} 
			}
		};
		
		
		
		SpaceDebugDraw::SpaceDebugDraw( const SpacePtr& space ) : m_space(space) {
			m_impl = new DebugDrawImpl();
		}
		
		SpaceDebugDraw::~SpaceDebugDraw() {
			delete m_impl;
		}
		
		void SpaceDebugDraw::Draw( Graphics& g ) const {
			if ( m_space ) {
				m_impl->Draw( g , m_space->get_space() );
			}
		}
		
		
		BodyDebugDraw::BodyDebugDraw( const BodyPtr& body ) : m_body(body) {
			m_impl = new DebugDrawImpl();
		}
		
		BodyDebugDraw::~BodyDebugDraw() {
			delete m_impl;
		}
		
		void BodyDebugDraw::Draw( Graphics& g ) const {
			if ( m_body ) {
				m_impl->Draw( g , m_body->get_body() , m_color);
			}
		}
		
	
		
	}
}
