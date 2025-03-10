#pragma once
#include "Monster.h"
#include "CentipedeAnim.h"  

// ���� ����
class CImageData;
class SpriteManager;
class Character;

class Centipede : public Monster {
public:
    // ������: ���� ��ǥ�� ��������Ʈ �����͸� ������
    Centipede(CImageData* pMonsterImgData, float startX, float startY);
    virtual ~Centipede();

    virtual void Update(float deltaTime) override;
    virtual void Render(CDDrawDevice* pDevice) override;

    
    // Component �ʱ�ȭ ���� �Լ���
    void InitCollider(int x, int y, int width, int height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();


    int GetHealth() const { return m_health; }
    void SetHealth(int health) { m_health = health; }
    int GetSpeed() const { return m_speed; }
    void SetSpeed(int speed) { m_speed = speed; }

    int GetSpriteFrameWidth() const;
    int GetSpriteFrameHeight() const;

    virtual MonsterType GetMonsterType() const override;
    
    virtual void TakeDamage(const DamageInfo& damageInfo) override;
    
    void Attack();
    void Chase();
    void Dead();


private:

    // �ִϸ��̼� ���� ��ȯ ���� (FSM)
    virtual void UpdateAnimFSM() override;
    virtual void UpdateAI(float deltaTime) override;


private:

    float m_health;

    float m_speed;         // ���� �̵� �ӵ�
    float m_baseSpeed;     // ������ �̵� �ӵ�

    bool m_isHurt = false;
    bool m_isDead = false;
    int m_hurtEndFrame = 0;

    // �ִϸ��̼� ���� ���� ���
    SpriteManager* m_spriteManager;
    CentipedeAnim* m_anim;     // CentipedeAnim�� ���ø� ����� SpriteAnim<CentipedeAnimState>

    // Centipede ���� ���� (��: �߷�, �̵� ����)
    float m_verticalVelocity;
    const float GRAVITY_ACCEL = 0.0015f;

    bool m_isGrounded = false;


    // �� ��迡�� ����ϱ� ���� ����
    float m_waitTime = 0.0f;       // ��� �ð� (��)
    float m_currentWaitTimer = 0.0f;
    
    // ���ݰ��� ����
    float m_attackDelay = 0.0f;     // �� ����
    float m_attackDelayTimer = 0.0f;
    float m_attackRange = 0.0f;
    bool m_isAttacking = false;
    bool m_attack2OverlapBoxSpawned = false;
    int m_attack2EndFrame = 0;
    
    float m_chaseStopDistance = 0.0f;


    MonsterAIState m_AIState;
    Character* m_chaseTarget;
};
