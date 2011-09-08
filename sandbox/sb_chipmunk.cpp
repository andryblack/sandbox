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
		
	
	
	template <class T>
	static inline bool remove( std::vector<T>& v, const T& x) {
		typename std::vector<T>::iterator i = std::find(v.begin(),v.end(),x);
		if (i!=v.end()) {
			v.erase(i);
			return true;
		}
		return false;
	}
	
	template <class T>
	static inline bool add( std::vector<T>& v, const T& x) {
		typename std::vector<T>::iterator i = std::find(v.begin(),v.end(),x);
		if (i!=v.end()) {
			return false;
		}
		v.push_back(x);
		return true;
	}
	
	class SpaceStaticBody : public Body {
	public:
		SpaceStaticBody( cpSpace* space ) : Body(space->staticBody){
		}
	};

	Space::Space() {
		m_time_cum = 0.0f;
		m_space = cpSpaceNew();
		m_static_body = BodyPtr( new SpaceStaticBody(m_space) );
	}
	
	Space::~Space() {
		cpSpaceFree(m_space);
	}
	
	void Space::SetGravity(const Vector2f& g) {
		cpSpaceSetGravity(m_space, cpv(g.x,g.y));
	}
	Vector2f Space::GetGravity() const {
		cpVect v = cpSpaceGetGravity(m_space);
		return Vector2f( v.x, v.y );
	}
	int Space::GetIterations() const {
		return cpSpaceGetIterations(m_space);
	}
	void Space::SetIterations(int i) {
		cpSpaceSetIterations(m_space, i);
	}
	void Space::AddBody(const BodyPtr& body) {
		if (add(m_bodys,body))
			cpSpaceAddBody(m_space, body->m_body);
	}
	void Space::RemoveBody(const BodyPtr& body) {
		if (remove(m_bodys,body))
			cpSpaceRemoveBody(m_space, body->m_body);
	}
	void Space::AddShape(const ShapePtr& shape) {
		if (add(m_shapes,shape))
			cpSpaceAddShape(m_space, shape->m_shape);
	}
	void Space::RemoveShape( const ShapePtr& shape) {
		if (remove(m_shapes,shape))
			cpSpaceRemoveShape(m_space, shape->m_shape);
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
		SetShape(cpCircleShapeNew(body->m_body,radius,cpv(pos.x,pos.y)));
	}
	Vector2f CircleShape::GetOffset() const {
		cpVect v = cpCircleShapeGetOffset(m_shape);
		return Vector2f(v.x,v.y);
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
		SetShape(cpPolyShapeNew(body->m_body,points.size(),v,cpv(offset.x,offset.y)));
	}
	
	SegmentShape::SegmentShape( const BodyPtr& body, const Vector2f& a,const Vector2f& b,float radius) {
		SetShape(cpSegmentShapeNew(body->m_body,cpv(a.x,a.y),cpv(b.x,b.y),radius));
	}
		
	BoxShape::BoxShape( const BodyPtr& body, float w, float h ) {
		SetShape(cpBoxShapeNew(body->m_body, w, h));
	}
		
	
	Body::Body( float mass, float inertia ) {
		m_body = cpBodyNew(mass, inertia);
		m_hold = true;
	}
	Body::Body(cpBody* external) {
		m_body = external;
		m_hold = false;
	}
	Body::~Body() {
		if (m_hold && m_body)
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
		cpBodySetPos(m_body, cpv(pos.x,pos.y));
	}
	Vector2f Body::GetVel() const {
		cpVect p = cpBodyGetVel(m_body);
		return Vector2f( p.x,p.y );
	}
	void Body::SetVel(const Vector2f& v){
		cpBodySetVel(m_body, cpv(v.x,v.y));
	}
	Vector2f Body::GetForce() const{
		cpVect p = cpBodyGetForce(m_body);
		return Vector2f( p.x,p.y );
	}
	void Body::SetForce(const Vector2f& force) {
		cpBodySetForce(m_body, cpv(force.x,force.y));
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
		cpVect v = cpBodyGetRot(m_body);
		return Vector2f(v.x,v.y);
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
		cpVect v = cpBodyLocal2World(m_body, cpv(pos.x,pos.y));
		return Vector2f( v.x,v.y );
	}
	Vector2f Body::World2Local(const Vector2f& pos) const {
		cpVect v = cpBodyWorld2Local( m_body, cpv(pos.x,pos.y));
		return Vector2f( v.x,v.y );
	}
	void Body::ResetForces() {
		cpBodyResetForces(m_body);
	}
	void Body::ApplyForce(const Vector2f& f, const Vector2f& r) {
		cpBodyApplyForce(m_body, cpv(f.x,f.y), cpv(r.x,r.y));
	}
	void Body::ApplyImpulse(const Vector2f& j, const Vector2f& r) {
		cpBodyApplyImpulse(m_body, cpv(j.x,j.y), cpv(r.x,r.y));
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
