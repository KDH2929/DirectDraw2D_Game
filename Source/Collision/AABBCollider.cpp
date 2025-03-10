#include "stdafx.h"
#include <algorithm>
#include <sstream>
#include "AABBCollider.h"
#include "OBBCollider.h"  // OBB �浹 ���� �Լ� ���


bool AABBCollider::CheckCollision(const Collider* other) const {

    // �浹 ���� �˻�: �� Collider�� ���̾�� ����ũ�� ���� �浹 �������� Ȯ��
    if (((this->GetCollisionMask() & other->GetCollisionLayer()) == 0) || ((other->GetCollisionMask() & this->GetCollisionLayer()) == 0)) 
    {
        return false;
    }


    if (other->GetType() == ColliderType::AABB) {
        const AABBCollider* otherAABB = static_cast<const AABBCollider*>(other);
        return CheckCollisionWithAABB(otherAABB);
    }
    else if (other->GetType() == ColliderType::OBB) {
        // AABB�� ȸ���� 0�� OBB�� �� �� ����.
        // ��, AABB�� m_x, m_y�� �»�� ��ǥ�̹Ƿ�, �߽� ��ǥ�� ��ȯ
        int centerX = m_x + m_width / 2;
        int centerY = m_y + m_height / 2;
        OBBCollider aabbAsObb(GetOwner(), centerX, centerY, m_width, m_height, 0.0f);
        const OBBCollider* otherOBB = static_cast<const OBBCollider*>(other);
        return aabbAsObb.CheckCollisionWithOBB(otherOBB);
    }
    return false;
}

bool AABBCollider::CheckCollisionWithAABB(const AABBCollider* other) const {
    return !(m_x + m_width < other->m_x ||
        other->m_x + other->m_width < m_x ||
        m_y + m_height < other->m_y ||
        other->m_y + other->m_height < m_y);
}

void AABBCollider::Update(const Transform& transform) {
    m_x = transform.position.x;
    m_y = transform.position.y;
}

void AABBCollider::Render(HDC hDC, int renderX, int renderY) const {
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0));

    int left = renderX;
    int top = renderY;
    int right = renderX + m_width;
    int bottom = renderY + m_height;

    // ���� ���õ� �귯�ø� �����ϰ�, NULL �귯�÷� �����Ͽ� ���� ä��⸦ ����
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
    Rectangle(hDC, left, top, right, bottom);
    SelectObject(hDC, hOldBrush);
}

std::string AABBCollider::GetDebugString() const {
    std::ostringstream oss;
    oss << "AABB(x=" << m_x << ", y=" << m_y
        << ", w=" << m_width << ", h=" << m_height << ")";
    return oss.str();
}


const Vector2<float> AABBCollider::GetCenterPosition() const
{
    return Vector2<float>(m_x + m_width * 0.5f, m_y + m_height * 0.5f);
}


void AABBCollider::SetRenderPosition(const Vector2<float>& pos)
{
    m_RenderPosition = pos;
}

Vector2<float> AABBCollider::GetRenderPosition() const
{
    return m_RenderPosition;
}
