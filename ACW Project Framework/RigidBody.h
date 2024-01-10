#pragma once
class RigidBody
{
public:
	RigidBody(const bool useGravity, const float mass, const float drag, const float angularDrag);
	RigidBody(const RigidBody& other); // Copy Constructor
	RigidBody(RigidBody&& other) noexcept; // Move Constructor
	~RigidBody(); // Destructor

	RigidBody& operator = (const RigidBody& other); // Copy Assignment Operator
	RigidBody& operator = (RigidBody&& other) noexcept; // Move Assignment Operator

	bool GetUseGravity() const;
	float GetMass() const;
	float GetDrag() const;
	float GetAngularDrag() const;

	void SetUseGravity(const bool useGravity);
	void SetMass(const float mass);
	void SetDrag(const float drag);
	void SetAngularDrag(const float angularDrag);

private:
	bool m_useGravity;
	float m_mass;
	float m_drag;
	float m_angularDrag;
};

