#include "GameObject.h"

//For adding default components or making it empty (defaults components: Position, Rotation, Scale)
GameObject::GameObject() : initializationFailed(false), updateInstanceData(false), maxTessellationDistance(1.0f), minTessellationDistance(1.0f), maxTessellationFactor(1.0f), minTessellationFactor(1.0f), mipInterval(0.0f), mipClampMinimum(0.0f), mipClampMaximum(0.0f), displacementPower(0.0f), position(nullptr), rotation(nullptr), scale(nullptr), rigidBody(nullptr), model(nullptr), texture(nullptr), shader(nullptr), parentObject(nullptr)
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
	position = make_shared<Position>();
}

void GameObject::AddPositionComponent(const XMFLOAT3& position) {
	position = make_shared<Position>(position);
}

void GameObject::AddPositionComponent(const float x, const float y, const float z) {
	position = make_shared<Position>(x, y, z);
}

void GameObject::AddPositionComponent(const vector<XMFLOAT3>& positions) {
	position = make_shared<Position>(positions);
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	position->SetPositionAt(position, 0);

	updateInstanceData = true;

	//model->Update(position->GetPositions(), scale);
}

void GameObject::SetPosition(const float x, const float y, const float z)
{
	position->SetPositionAt(x, y, z, 0);

	updateInstanceData = true;

	//model->Update(position->GetPositions(), scale);
}

void GameObject::SetPosition(const vector<XMFLOAT3>& positions)
{
	for (unsigned int i = 0; i < position->GetPositions().size(); i++)
	{
		position->SetPositionAt(positions[i], i);
	}

	updateInstanceData = true;

	//model->Update(positions, scale->GetScales());
}

void GameObject::AddRotationComponent() {
	rotation = make_shared<Rotation>();
}

void GameObject::AddRotationComponent(const XMFLOAT3& rotation) {
	rotation = make_shared<Rotation>(rotation);
}

void GameObject::AddRotationComponent(const float x, const float y, const float z) {
	rotation = make_shared<Rotation>(x, y, z);
}

void GameObject::AddRotationComponent(const vector<XMFLOAT3>& rotations)
{
	rotation = make_shared<Rotation>(rotations);
}

void GameObject::SetRotation(const XMFLOAT3& rotation)
{
	rotation->SetRotationAt(rotation, 0);

	updateInstanceData = true;
}

void GameObject::SetRotation(const float x, const float y, const float z)
{
	rotation->SetRotationAt(x, y, z, 0);

	updateInstanceData = true;
}

void GameObject::SetRotation(const vector<XMFLOAT3>& rotations)
{
	for (unsigned int i = 0; i < rotation->GetRotations().size(); i++)
	{
		rotation->SetRotationAt(rotations[i], i);
	}

	updateInstanceData = true;
}

void GameObject::AddScaleComponent() {
	scale = make_shared<Scale>();
}

void GameObject::AddScaleComponent(const XMFLOAT3& scale) {
	scale = make_shared<Scale>(scale);
}

void GameObject::AddScaleComponent(const float x, const float y, const float z) {
	scale = make_shared<Scale>(x, y, z);
}

void GameObject::AddScaleComponent(const vector<XMFLOAT3>& scales)
{
	scale = make_shared<Scale>(scales);
}

void GameObject::SetScale(const XMFLOAT3& scale)
{
	scale->SetScaleAt(scale, 0);

	updateInstanceData = true;
}

void GameObject::SetScale(const float x, const float y, const float z)
{
	scale->SetScaleAt(x, y, z, 0);

	updateInstanceData = true;
}

void GameObject::SetScale(const vector<XMFLOAT3>& scales)
{
	for (unsigned int i = 0; i < scale->GetScales().size(); i++)
	{
		scale->SetScaleAt(scales[i], i);
	}

	updateInstanceData = true;
}

void GameObject::AddRigidBodyComponent(const bool useGravity, const float mass, const float drag, const float angularDrag) {
	rigidBody = make_shared<RigidBody>(useGravity, mass, drag, angularDrag);
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
			initializationFailed = true;
			return;
	}

	model = make_shared<Model>(device, modelFileName, resourceManager, scale->GetScales(), rotation->GetRotations(), position->GetPositions());

	modelFileName = nullptr;

	if (model->GetInitializationState())
	{
		initializationFailed = true;
	}
}

void GameObject::AddTextureComponent(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager) {
	
	//Create and load texture
	texture = make_shared<Texture>(device, textureFileNames, resourceManager);

	if (texture->GetInitializationState())
	{
		initializationFailed = true;
	}
}

void GameObject::SetShaderComponent(const shared_ptr<Shader>& shader) {
	shader = shader;
}

void GameObject::AddParentGameObject(const shared_ptr<GameObject>& parentObject)
{
	parentObject = parentObject;

	updateInstanceData = true;
}

void GameObject::SetTessellationVariables(const float& maxTessellationDistance, const float& minTessellationDistance, const float& maxTessellationFactor, const float& minTessellationFactor)
{
	maxTessellationDistance = maxTessellationDistance;
	minTessellationDistance = minTessellationDistance;
	maxTessellationFactor = maxTessellationFactor;
	minTessellationFactor = minTessellationFactor;
}


void GameObject::SetDisplacementVariables(const float& mipInterval, const float& mipClampMinimum, const float& mipClampMaximum, const float& displacementPower)
{
	mipInterval = mipInterval;
	mipClampMinimum = mipClampMinimum;
	mipClampMaximum = mipClampMaximum;
	displacementPower = displacementPower;
}

const shared_ptr<Position>& GameObject::GetPositionComponent() const {
	return position;
}

//const XMFLOAT3& GameObject::GetPosition() const {
//	return position->GetPosition();
//}

const shared_ptr<Rotation>& GameObject::GetRotationComponent() const {
	return rotation;
}

//const XMFLOAT4& GameObject::GetRotation() const {
//	return rotation->GetRotation();
//}

const shared_ptr<Scale>& GameObject::GetScaleComponent() const {
	return scale;
}

//const XMFLOAT3& GameObject::GetScale() const {
//	return scale->GetScale();
//}

const shared_ptr<RigidBody>& GameObject::GetRigidBodyComponent() const {
	return rigidBody;
}

const shared_ptr<Model>& GameObject::GetModelComponent() const
{
	return model;
}

const shared_ptr<Shader>& GameObject::GetShaderComponent() const
{
	return shader;
}

int GameObject::GetIndexCount() const {
	return model->GetIndexCount();
}

const vector<ID3D11ShaderResourceView*>& GameObject::GetTextureList() const {
	return texture->GetTextureList();
}

bool GameObject::GetInitializationState() const {
	return initializationFailed;
}

void GameObject::UpdateInstanceData()
{
	updateInstanceData = true;
}

bool GameObject::Update()
{
	const auto result = true;

	if (updateInstanceData || parentObject)
	{
		if (model)
		{
			auto parentObjectMatrix = XMMatrixIdentity();

			if (parentObject)
			{
				const auto scale = parentObject->GetScaleComponent()->GetScaleAt(0);
				const auto rotation = parentObject->GetRotationComponent()->GetRotationAt(0);
				const auto position = parentObject->GetPositionComponent()->GetPositionAt(0);

				parentObjectMatrix = XMMatrixMultiply(parentObjectMatrix, XMMatrixScaling(1.0f, 1.0f, 1.0f));
				parentObjectMatrix = XMMatrixMultiply(parentObjectMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));
				parentObjectMatrix = XMMatrixMultiply(parentObjectMatrix, XMMatrixTranslation(position.x, position.y, position.z));
			}

			model->Update(scale->GetScales(), rotation->GetRotations(), position->GetPositions(), parentObjectMatrix);
		}

		updateInstanceData = false;
	}

	return result;
}

bool GameObject::Render(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const
{
	auto result = true;

	if (model)
	{
		model->Render(deviceContext);

		//Render shader
		if (shader)
		{
			shader->SetTessellationVariables(maxTessellationDistance, minTessellationDistance, maxTessellationFactor, minTessellationFactor);
			shader->SetDisplacementVariables(mipInterval, mipClampMinimum, mipClampMaximum, displacementPower * scale->GetScaleAt(0).x);

			result = shader->Render(deviceContext, GetIndexCount(), model->GetInstanceCount(), viewMatrix, projectionMatrix, GetTextureList(), depthTextures, pointLightList, cameraPosition);
		}
	}

	return result;
}
