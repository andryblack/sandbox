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

namespace Sandbox {
	
	namespace Chipmunk {
		
		static inline cpVect vect( const Vector2f& v) {
			return cpv(v.x,v.y);
		}
		
		static inline Vector2f vect( const cpVect& v) {
			return Vector2f(v.x,v.y);
		}
	
		

		Space::Space() {
			m_time_cum = 0.0f;
			m_space = cpSpaceNew();
		}
		
		Space::~Space() {
			cpSpaceFree(m_space);
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
			cpSpaceAddBody(m_space, body->get_body());
		}
		void Space::RemoveBody(const BodyPtr& body) {
			cpSpaceRemoveBody(m_space, body->get_body());
		}
		void Space::AddShape(const ShapePtr& shape) {
			cpSpaceAddShape(m_space, shape->get_shape());
		}
		void Space::RemoveShape( const ShapePtr& shape) {
			cpSpaceRemoveShape(m_space, shape->get_shape());
		}
		void Space::AddConstraint(const ConstraintPtr& constraint) {
			cpSpaceAddConstraint(m_space, constraint->get_constraint());
		}
		void Space::RemoveConstraint(const ConstraintPtr& constraint) {
			cpSpaceRemoveConstraint(m_space, constraint->get_constraint());
		}
		void Space::Step( float dt ) {
			cpSpaceStep(m_space, dt);
		}
		bool Space::Update( float dt ) {
			m_time_cum += dt;
			static const float static_step = 1.0f / 60.0f;
			size_t steps = size_t( m_time_cum / static_step );
			if ( steps ) {
				for (size_t i=0;i<steps;i++) {
					cpSpaceStep(m_space, static_step );
				}
				m_time_cum -= static_step * steps;
			}
			return false;
		}
		
		
		Shape::Shape() : m_shape(0) {
		}
		
		Shape::~Shape() {
			if(m_shape) cpShapeFree(m_shape);
		}
		
		void Shape::SetShape(cpShape* shape) {
			m_shape = shape;
		}
		float Shape::GetFriction() const {
			return cpShapeGetFriction(m_shape);
		}
		void Shape::SetFriction(float f) {
			cpShapeSetFriction(m_shape, f);
		}
		bool Shape::GetSensor() const {
			return cpShapeGetSensor(m_shape);
		}
		void Shape::SetSensor(bool s) {
			cpShapeSetSensor(m_shape, s?cpTrue:cpFalse);
		}
		
		float Shape::GetElasticity() const {
			return cpShapeGetElasticity(m_shape);
		}
		void Shape::SetElasticity(float e) {
			cpShapeSetElasticity(m_shape, e);
		}
			
			
		CircleShape::CircleShape( const BodyPtr& body, float radius, const Vector2f& pos ) {
			SetShape(cpCircleShapeNew(body->get_body(),radius,vect(pos)));
		}
		Vector2f CircleShape::GetOffset() const {
			return vect(cpCircleShapeGetOffset(m_shape));
		}
		float	CircleShape::GetRadius() const {
			return cpCircleShapeGetRadius(m_shape);
		}
		
		PolyShape::PolyShape( const BodyPtr& body, const std::vector<Vector2f>& points, const Vector2f& offset ) {
			cpVect v[points.size()];
			for (size_t i=0;i<points.size();i++) {
				v[i].x = points[i].x;
				v[i].y = points[i].y;
			}
			SetShape(cpPolyShapeNew(body->get_body(),points.size(),v,cpv(offset.x,offset.y)));
		}
		
		SegmentShape::SegmentShape( const BodyPtr& body, const Vector2f& a,const Vector2f& b,float radius) {
			SetShape(cpSegmentShapeNew(body->get_body(),vect(a),vect(b),radius));
		}
			
		BoxShape::BoxShape( const BodyPtr& body, float w, float h ) {
			SetShape(cpBoxShapeNew(body->get_body(), w, h));
		}
		
		Body::Body( float mass, float inertia ) {
			m_body = cpBodyNew(mass, inertia);
			cpBodySetUserData(m_body, this);
		}
		Body::Body(cpBody* b) :m_body(b){
			cpBodySetUserData(m_body, this);
		}
			
		Body::~Body() {
			cpBodyFree(m_body);
		}
		
		float Body::GetMass() const {
			return cpBodyGetMass(m_body);
		}
		void Body::SetMass(float m) {
			cpBodySetMass(m_body, m);
		}
		float Body::GetMoment() const {
			return cpBodyGetMoment(m_body);
		}
		void Body::SetMoment(float i) {
			cpBodySetMoment(m_body, i);
		}
		Vector2f Body::GetPos() const {
			cpVect p = cpBodyGetPos(m_body);
			return Vector2f( p.x,p.y );
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
			return cpBodyGetAngle(m_body);
		}
		void Body::SetAngle(float a) {
			cpBodySetAngle(m_body, a);
		}
		float Body::GetAngVel() const {
			return cpBodyGetAngVel(m_body);
		}
		void Body::SetAngVel(float v) {
			cpBodySetAngVel(m_body, v);
		}
		float Body::GetTorque() const {
			return cpBodyGetTorque(m_body);
		}
		void Body::SetTorque(float t) {
			cpBodySetTorque(m_body, t);
		}
		Vector2f Body::GetRot() const {
			return vect(cpBodyGetRot(m_body));
		}
		float Body::GetVelLimit() const {
			return cpBodyGetVelLimit(m_body);
		}
		void Body::SetVelLimit(float v) {
			cpBodySetVelLimit(m_body, v);
		}
		float Body::GetAngVelLimit() const {
			return cpBodyGetAngVelLimit(m_body);
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
			return cpBodyIsSleeping(m_body);
		}
		void Body::Activate() {
			cpBodyActivate(m_body);
		}
		void Body::Sleep() {
			cpBodySleep(m_body);
		}
		bool Body::IsStatic() const{
			return cpBodyIsStatic(m_body);
		}

		StaticBody::StaticBody() : Body(cpBodyNewStatic()){
		}

		
		Constraint::Constraint( cpConstraint* c ) : m_constraint(c) {
		}
		
		Constraint::~Constraint() {
			cpConstraintFree(m_constraint);
		}
		
		Body* Constraint::GetA() const {
			void* d = cpBodyGetUserData(cpConstraintGetA(m_constraint));
			return reinterpret_cast<Body*> (d);
		}
		
		Body* Constraint::GetB() const {
			void* d = cpBodyGetUserData(cpConstraintGetB(m_constraint));
			return reinterpret_cast<Body*> (d);
		}
		
		
		float Constraint::GetMaxForce() const {
			return cpConstraintGetMaxForce(m_constraint);
		}
		void Constraint::SetMaxForce(float f) {
			cpConstraintSetMaxForce(m_constraint, f);
		}
		float Constraint::GetMaxBias() const {
			return cpConstraintGetMaxBias(m_constraint);
		}
		void Constraint::SetMaxBias(float b) {
			cpConstraintSetMaxBias(m_constraint, b);
		}
		float Constraint::GetImpulse() const {
			return cpConstraintGetImpulse(m_constraint);
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
			return cpPinJointGetDist(m_constraint);
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
			return cpSlideJointGetMin(m_constraint);
		}
		void SlideJoint::SetMin(float d) {
			cpSlideJointSetMin(m_constraint, d);
		}
		float SlideJoint::GetMax() const {
			return cpSlideJointGetMax(m_constraint);
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
			return cpDampedSpringGetRestLength(m_constraint);
		}
		void DampedSpring::SetRestLength(float l) {
			cpDampedSpringSetRestLength(m_constraint, l);
		}
		float DampedSpring::GetStiffness() const {
			return cpDampedSpringGetStiffness(m_constraint);
		}
		void DampedSpring::SetStiffness(float s) {
			cpDampedSpringSetStiffness(m_constraint, s);
		}
		float DampedSpring::GetDamping() const {
			return cpDampedSpringGetDamping(m_constraint);
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
			return cpDampedRotarySpringGetRestAngle(m_constraint);
		}
		void DampedRotarySpring::SetRestAngle(float l) {
			cpDampedRotarySpringSetRestAngle(m_constraint, l);
		}
		float DampedRotarySpring::GetStiffness() const {
			return cpDampedRotarySpringGetStiffness(m_constraint);
		}
		void DampedRotarySpring::SetStiffness(float s) {
			cpDampedRotarySpringSetStiffness(m_constraint, s);
		}
		float DampedRotarySpring::GetDamping() const {
			return cpDampedRotarySpringGetDamping(m_constraint);
		}
		void DampedRotarySpring::SetDamping(float d) {
			cpDampedRotarySpringSetDamping(m_constraint, d);
		}
		
		
		RotaryLimitJoint::RotaryLimitJoint(const BodyPtr& a,const BodyPtr& b,
						 float min, float max)
		: Constraint(cpRotaryLimitJointNew(a->get_body(), b->get_body(), min, max)) {
		}
		float RotaryLimitJoint::GetMin() const {
			return cpRotaryLimitJointGetMin(m_constraint);
		}
		void RotaryLimitJoint::SetMin(float m) {
			cpRotaryLimitJointSetMin(m_constraint, m);
		}
		float RotaryLimitJoint::GetMax() const {
			return cpRotaryLimitJointGetMax(m_constraint);
		}
		void RotaryLimitJoint::SetMax(float m) {
			cpRotaryLimitJointSetMax(m_constraint, m);
		}
		
		
		
		RatchetJoint::RatchetJoint(const BodyPtr& a,const BodyPtr& b,
					 float phase, float ratchet)
		: Constraint(cpRatchetJointNew(a->get_body(), b->get_body(), phase, ratchet)) {
		}
		float RatchetJoint::GetAngle() const {
			return cpRatchetJointGetAngle(m_constraint);
		}
		void RatchetJoint::SetAngle(float m) {
			cpRatchetJointSetAngle(m_constraint, m);
		}
		float RatchetJoint::GetPhase() const {
			return cpRatchetJointGetPhase(m_constraint);
		}
		void RatchetJoint::SetPhase(float m) {
			cpRatchetJointSetPhase(m_constraint, m);
		}
		float RatchetJoint::GetRatchet() const {
			return cpRatchetJointGetRatchet(m_constraint);
		}
		void RatchetJoint::SetRatchet(float m) {
			cpRatchetJointSetRatchet(m_constraint, m);
		}
		
		
		
		GearJoint::GearJoint(const BodyPtr& a,const BodyPtr& b,
				  float phase, float ratio)
		: Constraint(cpGearJointNew(a->get_body(), b->get_body(), phase, ratio)) {
		}
		float GearJoint::GetPhase() const {
			return cpGearJointGetPhase(m_constraint);
		}
		void GearJoint::SetPhase(float m) {
			cpGearJointSetPhase(m_constraint, m);
		}
		float GearJoint::GetRatio() const {
			return cpGearJointGetRatio(m_constraint);
		}
		void GearJoint::SetRatio(float m) {
			cpGearJointSetRatio(m_constraint, m);
		}
		
		SimpleMotor::SimpleMotor(const BodyPtr& a,const BodyPtr& b,
					float rate)
		: Constraint(cpSimpleMotorNew(a->get_body(), b->get_body(), rate)) {
		}
		float SimpleMotor::GetRate() const {
			return cpSimpleMotorGetRate(m_constraint);
		}
		void SimpleMotor::SetRate(float r) {
			cpSimpleMotorSetRate(m_constraint, r);
		}
		
		
		TransformAdapter::TransformAdapter( const BodyPtr& body, const ContainerTransformPtr& transform ) {
			m_body = body;
			m_transform = transform;
		}
		
		void TransformAdapter::Sync() {
			m_transform->SetTranslate( m_body->GetPos() );
			m_transform->SetAngle( m_body->GetAngle() );
		}
		
		bool TransformAdapter::Update(float ) {
			Sync();
			return false;
		}
		
	}
}
