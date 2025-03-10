#include "stdafx.h"
#include "GameObject.h"
#include "Collider.h"
#include "DebugManager.h"

GameObject::GameObject()
    : m_pCollider(nullptr),
    m_CurrentPosition(m_Transform.position),
    m_PreviousPosition(m_Transform.position),
    m_InterpolatedPosition(m_Transform.position),
    m_RenderPosition(Vector2<float>{0, 0}),
    m_ColliderLocalPosition(Vector2<float>{0, 0}),
    m_mass(1.0f), m_invMass(1.0f), m_physicsType(PhysicsType::Dynamic)
{
    // m_Transform�� �⺻ �����ڿ��� �ʱ�ȭ��.
}

GameObject::~GameObject() {
    if (m_pCollider) {
        delete m_pCollider;
        m_pCollider = nullptr;
    }
}

void GameObject::Update(float deltaTime) {
    // ���� �� �ܰ�: ���� ��ġ�� ���� ��ġ�� ���
    m_PreviousPosition = m_CurrentPosition;
    m_CurrentPosition = m_Transform.position;

    // Collider ������Ʈ
    Transform updatedColliderTransform = GetTransform();
    updatedColliderTransform.position = GetPosition() + GetColliderLocalPosition();
    if (m_pCollider)
    {
        m_pCollider->Update(updatedColliderTransform);
    }

}

void GameObject::Render(CDDrawDevice* pDevice) {

}

Transform& GameObject::GetTransform() {
    return m_Transform;
}

Vector2<float> GameObject::GetPosition() const {
    return m_Transform.position;
}

void GameObject::SetTransform(const Transform& newTransform) {
    m_Transform = newTransform;
    m_CurrentPosition = newTransform.position;
    m_PreviousPosition = newTransform.position;
    m_InterpolatedPosition = newTransform.position;
}

Collider* GameObject::GetCollider() {
    return m_pCollider;
}

void GameObject::SetCollider(Collider* collider)
{
    m_pCollider = collider;
}


void GameObject::SetColliderLocalPosition(const Vector2<float>& pos)
{
    m_ColliderLocalPosition = pos;
}

const Vector2<float>& GameObject::GetColliderLocalPosition() const
{
    return m_ColliderLocalPosition;
}



void GameObject::OnCollision(const CollisionInfo& collisionInfo) {
    // �⺻ �浹 �̺�Ʈ ó�� (��ӹ޾� ��ü���� ó������)
    switch (collisionInfo.response) {
    case CollisionResponse::Block:
        break;
    case CollisionResponse::Overlap:
        break;
    default:
        break;
    }
}


void GameObject::SetPosition(const Vector2<float>& newPos) {
    m_PreviousPosition = m_CurrentPosition;
    m_CurrentPosition = newPos;
    m_InterpolatedPosition = newPos;
    m_Transform.position = newPos;
}

void GameObject::ResetInterpolation() {
    m_PreviousPosition = m_CurrentPosition;
    m_InterpolatedPosition = m_CurrentPosition;
}

void GameObject::UpdateInterpolation(float alpha) {
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    m_InterpolatedPosition.x = m_PreviousPosition.x + (m_CurrentPosition.x - m_PreviousPosition.x) * alpha;
    m_InterpolatedPosition.y = m_PreviousPosition.y + (m_CurrentPosition.y - m_PreviousPosition.y) * alpha;
}

Vector2<float> GameObject::GetInterpolatedPosition() const {
    return m_InterpolatedPosition;
}


void GameObject::SetRenderPosition(const Vector2<float>& pos)
{
    m_RenderPosition = pos;
}

Vector2<float> GameObject::GetRenderPosition() const
{
    return m_RenderPosition;
}



void GameObject::SetMass(float mass) {
    m_mass = mass;
    // ������ 0�̸� ���� ��ü�� �����ϰ�, �������� 0���� ����
    m_invMass = (mass != 0.0f) ? (1.0f / mass) : 0.0f;
}

float GameObject::GetMass() const {
    return m_mass;
}

float GameObject::GetInvMass() const {
    return m_invMass;
}

void GameObject::SetPhysicsType(PhysicsType type) {
    m_physicsType = type;
    // ���� ��ü��� ������ 0����, �������� 0���� ����
    if (type == PhysicsType::Static) {
        m_mass = 0.0f;
        m_invMass = 0.0f;
    }
}

PhysicsType GameObject::GetPhysicsType() const {
    return m_physicsType;
}