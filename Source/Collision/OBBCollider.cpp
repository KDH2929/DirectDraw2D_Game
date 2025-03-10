#include "stdafx.h"
#include "OBBCollider.h"
#include "AABBCollider.h"
#include "MathUtils.h"
#include <algorithm>
#include <sstream>

// OBB�� ������ ���
// box->GetX(), box->GetY()�� ���� ��ǥ�迡���� �߽� ��ǥ��� ����
static void ComputeOBBCorners(const OBBCollider* box, Vector2<float> corners[4]) {
    float halfWidth = box->GetWidth() / 2.0f;
    float halfHeight = box->GetHeight() / 2.0f;
    float rad = degToRad(box->GetRotation());

    // ȸ���� ���� �������� (axisX, axisY)
    Vector2<float> axisX = { std::cos(rad), std::sin(rad) };
    Vector2<float> axisY = { -std::sin(rad), std::cos(rad) };

    // ���� ��ǥ�迡���� �߽�
    Vector2<float> center = { static_cast<float>(box->GetX()), static_cast<float>(box->GetY()) };

    // 4�� ������ ��� (�ð����)
    corners[0] = center + axisX * halfWidth + axisY * halfHeight;
    corners[1] = center - axisX * halfWidth + axisY * halfHeight;
    corners[2] = center - axisX * halfWidth - axisY * halfHeight;
    corners[3] = center + axisX * halfWidth - axisY * halfHeight;
}

// �־��� ��(axis)�� ���� 4�� �������� �����Ͽ� �ּ�/�ִ� ���� ���
static void ProjectOntoAxis(const Vector2<float> corners[4], const Vector2<float>& axis, float& minProj, float& maxProj) {
    minProj = maxProj = Dot(corners[0], axis);
    for (int i = 1; i < 4; i++) {
        float projection = Dot(corners[i], axis);
        minProj = std::min<float>(minProj, projection);
        maxProj = std::max<float>(maxProj, projection);
    }
}

// �ٸ� Collider Ÿ�Կ� ���� �б�
bool OBBCollider::CheckCollision(const Collider* other) const {

    // �浹 ���� �˻�: �� Collider�� ���̾�� ����ũ�� ���� �浹 �������� Ȯ��
    if (((this->GetCollisionMask() & other->GetCollisionLayer()) == 0) || ((other->GetCollisionMask() & this->GetCollisionLayer()) == 0)) {
        return false;
    }


    if (other->GetType() == ColliderType::OBB) {
        const OBBCollider* otherOBB = static_cast<const OBBCollider*>(other);
        return CheckCollisionWithOBB(otherOBB);
    }
    else if (other->GetType() == ColliderType::AABB) {
        // �ٸ� Collider�� AABB�� ���,
        // AABB�� ��ǥ�� �»�� �����̹Ƿ� �߽� ��ǥ�� ��ȯ
        const AABBCollider* otherAABB = static_cast<const AABBCollider*>(other);
        float centerX = otherAABB->GetX() + otherAABB->GetWidth() / 2;
        float centerY = otherAABB->GetY() + otherAABB->GetHeight() / 2;
        // ȸ���� 0�� OBB�� ��ȯ�Ͽ� �浹 �˻縦 ����
        OBBCollider aabbAsObb(nullptr, centerX, centerY, otherAABB->GetWidth(), otherAABB->GetHeight(), 0.0f);
        return CheckCollisionWithOBB(&aabbAsObb);
    }
    return false;
}

// OBBCollider::CheckCollisionWithOBB ���� (SAT ���)
bool OBBCollider::CheckCollisionWithOBB(const OBBCollider* other) const {
    // �� OBB�� ������ ���
    Vector2<float> cornersA[4], cornersB[4];
    ComputeOBBCorners(this, cornersA);
    ComputeOBBCorners(other, cornersB);

    // �ڽ��� ȸ������ �� �������Ϳ� other�� ȸ������ �� �������� (�� 4��)
    float radA = degToRad(GetRotation());
    float radB = degToRad(other->GetRotation());
    Vector2<float> axes[4];
    axes[0] = Normalize(Vector2<float>{ std::cos(radA), std::sin(radA) });
    axes[1] = Normalize(Vector2<float>{ -std::sin(radA), std::cos(radA) });
    axes[2] = Normalize(Vector2<float>{ std::cos(radB), std::sin(radB) });
    axes[3] = Normalize(Vector2<float>{ -std::sin(radB), std::cos(radB) });

    // �� �࿡ ���� ������ ������ ��ġ���� Ȯ��
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(cornersA, axes[i], minA, maxA);
        ProjectOntoAxis(cornersB, axes[i], minB, maxB);

        // �� ���̶� ������ ��ġ�� ������ �浹���� ����
        if (maxA < minB || maxB < minA)
            return false;
    }
    // ��� �࿡�� ������ ��ġ�� �浹�� �߻���
    return true;
}

void OBBCollider::Update(const Transform& transform)
{
    // Transform ������ �̿��� �߽� ��ǥ�� ������Ʈ
    // �ʿ�� transform���� ȸ�� ���� ��������
    m_x = static_cast<float>(transform.position.x);
    m_y = static_cast<float>(transform.position.y);
}

void OBBCollider::Render(HDC hDC, int renderX, int renderY) const
{

}

const Vector2<float> OBBCollider::GetCenterPosition() const
{
    return Vector2<float>(m_x, m_y);
}

std::string OBBCollider::GetDebugString() const {
    std::ostringstream oss;
    oss << "OBB(center=(" << m_x << ", " << m_y << "), w=" << m_width
        << ", h=" << m_height << ", rot=" << m_rotation << ")";
    return oss.str();
}

void OBBCollider::SetRenderPosition(const Vector2<float>& pos)
{
    m_RenderPosition = pos;
}

Vector2<float> OBBCollider::GetRenderPosition() const
{
    return m_RenderPosition;
}