#include "RigidBody.h"

RigidBody::RigidBody(const bool useGravity, const float mass, const float drag, const float angularDrag) : useGravity(useGravity), mass(mass), drag(drag), angularDrag(angularDrag) {
	
}

RigidBody::RigidBody(const RigidBody& other) = default;

RigidBody::RigidBody(RigidBody&& other) noexcept = default;

RigidBody::~RigidBody() = default;

RigidBody& RigidBody::operator=(const RigidBody& other) = default;

RigidBody& RigidBody::operator=(RigidBody&& other) noexcept = default;

bool RigidBody::GetUseGravity() const {
	return useGravity;
}

float RigidBody::GetMass() const {
	return mass;
}

float RigidBody::GetDrag() const {
	return drag;
}

float RigidBody::GetAngularDrag() const {
	return angularDrag;
}

void RigidBody::SetUseGravity(const bool useGrav) {
	useGravity = useGrav;
}

void RigidBody::SetMass(const float ma) {
	mass = ma;
}

void RigidBody::SetDrag(const float dg) {
	drag = dg;
}

void RigidBody::SetAngularDrag(const float angularDg) {
	angularDrag = angularDg;
}