#pragma once
#include "Collider.h"
#include "ColliderManager.h"
#include "MathUtils.h"
#include <string>


enum class PhysicsType {
    Static,         // �������� ���� (ex: ��, �ٴ�)
    Dynamic,    // ���� �ùķ��̼ǿ� ���� ������
};


struct DamageInfo {
    int damageAmount;
    GameObject* instigator; 
    Vector2<float> hitLocation;

    DamageInfo(int dmg, GameObject* inst, const Vector2<float>& hitLoc)
        : damageAmount(dmg), instigator(inst), hitLocation(hitLoc)
    {}
};



class CDDrawDevice;


class GameObject {
public:
    GameObject();
    virtual ~GameObject();

    // �� ������ ������Ʈ (deltaTime: �и��� ����)
    virtual void Update(float deltaTime);
    // �� ������ ������
    virtual void Render(CDDrawDevice* pDevice);     // ���� winAPI ��������� ���� ������ HDC�� �Ű������� �߰�

    // Transform ������/������
    Transform& GetTransform();
    Vector2<float> GetPosition() const;
    void SetTransform(const Transform& newTransform);

    // Collider ����
    Collider* GetCollider();
    void SetCollider(Collider* collider);
    void SetColliderLocalPosition(const Vector2<float>& pos);
    const Vector2<float>& GetColliderLocalPosition() const;

    // �浹 �̺�Ʈ �ݹ� (�浹 �߻� �� ȣ��)
    // other: �浹�� �ٸ� GameObject, response: �浹 ���� Ÿ��
    virtual void OnCollision(const CollisionInfo& collisionInfo);
    virtual void TakeDamage(const DamageInfo& damageInfo) {};


    // ���� ���� �Լ���
    virtual void SetPosition(const Vector2<float>& newPos);
    void ResetInterpolation();
    void UpdateInterpolation(float alpha);
    Vector2<float> GetInterpolatedPosition() const;


    // ������ ��ġ ���� �Լ�
    // ������ ��ġ�� ���� ��ǥ�� ���� ��ġ�� ���Ӽ��迡 ���� ���� ���� �ٸ� ���� ����
    void SetRenderPosition(const Vector2<float>& pos);
    Vector2<float> GetRenderPosition() const;
    

    // ���� ���� Getter/Setter
    void SetMass(float mass);         // ������ �����ϰ�, ���ÿ� �������� ���
    float GetMass() const;
    float GetInvMass() const;

    void SetPhysicsType(PhysicsType type);
    PhysicsType GetPhysicsType() const;


protected:
    Transform m_Transform;
    Collider* m_pCollider;

    Vector2<float> m_ColliderLocalPosition;


    // ������ ���� ��ġ ����
    Vector2<float> m_CurrentPosition;
    Vector2<float> m_PreviousPosition;
    Vector2<float> m_InterpolatedPosition;


    // ���� �Ӽ�
    float m_mass;         // ����
    float m_invMass;      // ������ (mass�� 0�̸� 0, �׷��� ������ 1/mass)
    PhysicsType m_physicsType;


    // ������ ��ġ 
    // DirectX ��ũ�� ��ǥ��󿡼��� ��ġ
    Vector2<float> m_RenderPosition;

};
