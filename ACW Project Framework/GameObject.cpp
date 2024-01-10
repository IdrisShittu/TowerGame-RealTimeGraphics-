#include "GameObject.h"

//For adding default components or making it empty (defaults components: Position, Rotation, Scale)
GameObject::GameObject() : m_initializationFailed(false), m_updateInstanceData(false), m_maxTessellationDistance(1.0f), m_minTessellationDistance(1.0f), m_maxTessellationFactor(1.0f), m_minTessellationFactor(1.0f), m_mipInterval(0.0f), m_mipClampMinimum(0.0f), m_mipClampMaximum(0.0f), m_displacementPower(0.0f), m_position(nullptr), m_rotation(nullptr), m_scale(nullptr), m_rigidBody(nullptr), m_model(nullptr), m_texture(nullptr), m_shader(nullptr), m_parentObject(nullptr)
{
	//Empty GameObject with no components
}

//GameObject::GameObject(const GameObject& other) = default;

//GameObject::GameObject(GameObject&& other) noexcept = default;

GameObject::~GameObject()
{

}

//GameObject& GameObject::operator=(const GameObject& other) = default;

//GameObject& GameObject::operator=(GameObject&& other) noexcept = default;

void GameObject::AddPositionComponent() {
	m_position = make_shared<Position>();
}

void GameObject::AddPositionComponent(const XMFLOAT3& position) {
	m_position = make_shared<Position>(position);
}

void GameObject::AddPositionComponent(const float x, const float y, const float z) {
	m_position = make_shared<Position>(x, y, z);
}

void GameObject::AddPositionComponent(const vector<XMFLOAT3>& positions) {
	m_position = make_shared<Position>(positions);
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	m_position->SetPositionAt(position, 0);

	m_updateInstanceData = true;

	//m_model->Update(m_position->GetPositions(), scale);
}

void GameObject::SetPosition(const float x, const float y, const float z)
{
	m_position->SetPositionAt(x, y, z, 0);

	m_updateInstanceData = true;

	//m_model->Update(m_position->GetPositions(), scale);
}

void GameObject::SetPosition(const vector<XMFLOAT3>& positions)
{
	for (unsigned int i = 0; i < m_position->GetPositions().size(); i++)
	{
		m_position->SetPositionAt(positions[i], i);
	}

	m_updateInstanceData = true;

	//m_model->Update(positions, m_scale->GetScales());
}

void GameObject::AddRotationComponent() {
	m_rotation = make_shared<Rotation>();
}

void GameObject::AddRotationComponent(const XMFLOAT3& rotation) {
	m_rotation = make_shared<Rotation>(rotation);
}

void GameObject::AddRotationComponent(const float x, const float y, const float z) {
	m_rotation = make_shared<Rotation>(x, y, z);
}

void GameObject::AddRotationComponent(const vector<XMFLOAT3>& rotations)
{
	m_rotation = make_shared<Rotation>(rotations);
}

void GameObject::SetRotation(const XMFLOAT3& rotation)
{
	m_rotation->SetRotationAt(rotation, 0);

	m_updateInstanceData = true;
}

void GameObject::SetRotation(const float x, const float y, const float z)
{
	m_rotation->SetRotationAt(x, y, z, 0);

	m_updateInstanceData = true;
}

void GameObject::SetRotation(const vector<XMFLOAT3>& rotations)
{
	for (unsigned int i = 0; i < m_rotation->GetRotations().size(); i++)
	{
		m_rotation->SetRotationAt(rotations[i], i);
	}

	m_updateInstanceData = true;
}

void GameObject::AddScaleComponent() {
	m_scale = make_shared<Scale>();
}

void GameObject::AddScaleComponent(const XMFLOAT3& scale) {
	m_scale = make_shared<Scale>(scale);
}

void GameObject::AddScaleComponent(const float x, const float y, const float z) {
	m_scale = make_shared<Scale>(x, y, z);
}

void GameObject::AddScaleComponent(const vector<XMFLOAT3>& scales)
{
	m_scale = make_shared<Scale>(scales);
}

void GameObject::SetScale(const XMFLOAT3& scale)
{
	m_scale->SetScaleAt(scale, 0);

	m_updateInstanceData = true;
}

void GameObject::SetScale(const float x, const float y, const float z)
{
	m_scale->SetScaleAt(x, y, z, 0);

	m_updateInstanceData = true;
}

void GameObject::SetScale(const vector<XMFLOAT3>& scales)
{
	for (unsigned int i = 0; i < m_scale->GetScales().size(); i++)
	{
		m_scale->SetScaleAt(scales[i], i);
	}

	m_updateInstanceData = true;
}

void GameObject::AddRigidBodyComponent(const bool useGravity, const float mass, const float drag, const float angularDrag) {
	m_rigidBody = make_shared<RigidBody>(useGravity, mass, drag, angularDrag);
}

void GameObject::AddModelComponent(ID3D11Device* const device, const ModelType modelType, const shared_ptr<ResourceManager>& resourceManager) {

	const auto* modelFileName = "";

	switch (modelType)
	{
		case ModelType::Sphere:
			modelFileName = "sphere.obj";
			break;
		case ModelType::SphereInverted:
			modelFileName = "SphereInverted.obj";
			break;
		case ModelType::HighPolyCube:
			modelFileName = "cubeHigh.obj";
			break;
		case ModelType::LowPolyCube:
			modelFileName = "cubeLow.obj";
			break;
		case ModelType::Plane:
			modelFileName = "plane.obj";
			break;
		case ModelType::HighPolyCylinder:
			modelFileName = "cylinderHigh.obj";
			break;
		case ModelType::LowPolyCylinder:
			modelFileName = "cylinderLow.obj";
			break;
		case ModelType::Cone:
			modelFileName = "cone.obj";
			break;
		case ModelType::Quad:
			modelFileName = "quad.obj";
			break;
		default:
			m_initializationFailed = true;
			return;
	}

	m_model = make_shared<Model>(device, modelFileName, resourceManager, m_scale->GetScales(), m_rotation->GetRotations(), m_position->GetPositions());

	modelFileName = nullptr;

	if (m_model->GetInitializationState())
	{
		m_initializationFailed = true;
	}
}

void GameObject::AddTextureComponent(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager) {
	
	//Create and load texture
	m_texture = make_shared<Texture>(device, textureFileNames, resourceManager);

	if (m_texture->GetInitializationState())
	{
		m_initializationFailed = true;
	}
}

void GameObject::SetShaderComponent(const shared_ptr<Shader>& shader) {
	m_shader = shader;
}

void GameObject::AddParentGameObject(const shared_ptr<GameObject>& parentObject)
{
	m_parentObject = parentObject;

	m_updateInstanceData = true;
}

void GameObject::SetTessellationVariables(const float& maxTessellationDistance, const float& minTessellationDistance, const float& maxTessellationFactor, const float& minTessellationFactor)
{
	m_maxTessellationDistance = maxTessellationDistance;
	m_minTessellationDistance = minTessellationDistance;
	m_maxTessellationFactor = maxTessellationFactor;
	m_minTessellationFactor = minTessellationFactor;
}


void GameObject::SetDisplacementVariables(const float& mipInterval, const float& mipClampMinimum, const float& mipClampMaximum, const float& displacementPower)
{
	m_mipInterval = mipInterval;
	m_mipClampMinimum = mipClampMinimum;
	m_mipClampMaximum = mipClampMaximum;
	m_displacementPower = displacementPower;
}

const shared_ptr<Position>& GameObject::GetPositionComponent() const {
	return m_position;
}

//const XMFLOAT3& GameObject::GetPosition() const {
//	return m_position->GetPosition();
//}

const shared_ptr<Rotation>& GameObject::GetRotationComponent() const {
	return m_rotation;
}

//const XMFLOAT4& GameObject::GetRotation() const {
//	return m_rotation->GetRotation();
//}

const shared_ptr<Scale>& GameObject::GetScaleComponent() const {
	return m_scale;
}

//const XMFLOAT3& GameObject::GetScale() const {
//	return m_scale->GetScale();
//}

const shared_ptr<RigidBody>& GameObject::GetRigidBodyComponent() const {
	return m_rigidBody;
}

const shared_ptr<Model>& GameObject::GetModelComponent() const
{
	return m_model;
}

const shared_ptr<Shader>& GameObject::GetShaderComponent() const
{
	return m_shader;
}

int GameObject::GetIndexCount() const {
	return m_model->GetIndexCount();
}

const vector<ID3D11ShaderResourceView*>& GameObject::GetTextureList() const {
	return m_texture->GetTextureList();
}

bool GameObject::GetInitializationState() const {
	return m_initializationFailed;
}

void GameObject::UpdateInstanceData()
{
	m_updateInstanceData = true;
}

bool GameObject::Update()
{
	const auto result = true;

	if (m_updateInstanceData || m_parentObject)
	{
		if (m_model)
		{
			auto parentObjectMatrix = XMMatrixIdentity();

			if (m_parentObject)
			{
				const auto scale = m_parentObject->GetScaleComponent()->GetScaleAt(0);
				const auto rotation = m_parentObject->GetRotationComponent()->GetRotationAt(0);
				const auto position = m_parentObject->GetPositionComponent()->GetPositionAt(0);

				parentObjectMatrix = XMMatrixMultiply(parentObjectMatrix, XMMatrixScaling(1.0f, 1.0f, 1.0f));
				parentObjectMatrix = XMMatrixMultiply(parentObjectMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));
				parentObjectMatrix = XMMatrixMultiply(parentObjectMatrix, XMMatrixTranslation(position.x, position.y, position.z));
			}

			m_model->Update(m_scale->GetScales(), m_rotation->GetRotations(), m_position->GetPositions(), parentObjectMatrix);
		}

		m_updateInstanceData = false;
	}

	return result;
}

bool GameObject::Render(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const
{
	auto result = true;

	if (m_model)
	{
		m_model->Render(deviceContext);

		//Render shader
		if (m_shader)
		{
			m_shader->SetTessellationVariables(m_maxTessellationDistance, m_minTessellationDistance, m_maxTessellationFactor, m_minTessellationFactor);
			m_shader->SetDisplacementVariables(m_mipInterval, m_mipClampMinimum, m_mipClampMaximum, m_displacementPower * m_scale->GetScaleAt(0).x);

			result = m_shader->Render(deviceContext, GetIndexCount(), m_model->GetInstanceCount(), viewMatrix, projectionMatrix, GetTextureList(), depthTextures, pointLightList, cameraPosition);
		}
	}

	return result;
}
