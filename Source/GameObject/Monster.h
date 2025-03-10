#pragma once
#include "GameObject.h"


enum class MonsterType {
    Unknown,
    Centipede,
};

enum class MonsterAIState {
    Wait,
    Move,
    Turn,
    Attack,
    Chase,
    Dead,
};

class Character;

class Monster : public GameObject {
public:
    Monster();
    virtual ~Monster();

    virtual void Update(float deltaTime);
    virtual void Render(CDDrawDevice* pDevice);

    virtual MonsterType GetMonsterType() const = 0;


    const Vector2<float> GetForwardVector();
    void SetForwardVector(Vector2<float> forwardVec);

    // ���� ���ʹ� AABB Collider���� �����ϰ� ���
    // ���� MonsterAI Ŭ������ ����� ��ɺи��ϸ� ���� ��
    virtual bool CheckLeftObject();
    virtual bool CheckRightObject();
    virtual bool CheckForwardObject();

    virtual bool CheckGround();
    virtual bool CheckForwardGround();
    Character* DetectPlayer(float rayDistance);
    std::wstring MonsterAIStateToWString(MonsterAIState state);

protected:
    virtual void UpdateAI(float deltaTime) = 0;
    virtual void UpdateAnimFSM() = 0;


private:
    Vector2<float> m_forwardVec = Vector2<float>(1.0f, 0.0f);           // �⺻ ForwardVector �� �����ʹ���
};
