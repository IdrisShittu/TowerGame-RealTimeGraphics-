#include "RigidBody.h"

RigidBody::RigidBody(const bool useGravity, const float mass, const float drag, const float angularDrag) : m_useGravity(useGravity), m_mass(mass), m_drag(drag), m_angularDrag(angularDrag) {
	
}

RigidBody::RigidBody(const RigidBody& other) = default;

RigidBody::RigidBody(RigidBody&& other) noexcept = default;

RigidBody::~RigidBody() = default;

RigidBody& RigidBody::operator=(const RigidBody& other) = default;

RigidBody& RigidBody::operator=(RigidBody&& other) noexcept = default;

bool RigidBody::GetUseGravity() const {
	return m_useGravity;
}

float RigidBody::GetMass() const {
	return m_mass;
}

float RigidBody::GetDrag() const {
	return m_drag;
}

float RigidBody::GetAngularDrag() const {
	return m_angularDrag;
}

void RigidBody::SetUseGravity(const bool useGravity) {
	m_useGravity = useGravity;
}

void RigidBody::SetMass(const float mass) {
	m_mass = mass;
}

void RigidBody::SetDrag(const float drag) {
	m_drag = drag;
}

void RigidBody::SetAngularDrag(const float angularDrag) {
	m_angularDrag = angularDrag;
}