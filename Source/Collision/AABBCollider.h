#pragma once
#include "Collider.h"

// AABB �浹ü Ŭ����
class AABBCollider : public Collider {
public:
    // owner: �� Collider�� ������ GameObject  
    // �⺻��: �»�� ��ǥ 0, ũ�⵵ 0
    AABBCollider(GameObject* owner, int x = 0, int y = 0, int width = 0, int height = 0)
        : Collider(ColliderType::AABB, owner), m_x(x), m_y(y), m_width(width), m_height(height) {}
    virtual ~AABBCollider() {}

    virtual bool CheckCollision(const Collider* other) const override;
    bool CheckCollisionWithAABB(const AABBCollider* other) const;
    virtual void Update(const Transform& transform) override;
    virtual void Render(HDC hDC, int renderX, int renderY) const override;
    virtual std::string GetDebugString() const override;

    // Getter
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    // �߽� ��ǥ ��ȯ �Լ� (�»�� ����)
    virtual const Vector2<float> GetCenterPosition() const override;


    virtual void SetRenderPosition(const Vector2<float>& pos) override;
    virtual Vector2<float> GetRenderPosition() const override;

private:
    int m_x, m_y;         // �»�� ��ǥ
    int m_width, m_height;  // ũ��
};
