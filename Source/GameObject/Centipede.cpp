#include "stdafx.h"
#include "Centipede.h"
#include "AABBCollider.h"
#include "../ImageData/ImageData.h"
#include "../DDrawLib/DDrawDevice.h"
#include "DebugManager.h"
#include "SpriteManager.h"
#include "CentipedeAnim.h"
#include "InputManager.h"
#include "CollisionQuery.h"
#include "Character.h"

#include <algorithm>
#include <cmath>

Centipede::Centipede(CImageData* pMonsterImgData, float startX, float startY)
    : m_health(100.0f), m_speed(4.0f), m_baseSpeed(3.0f),
    m_verticalVelocity(0.0f),
    m_spriteManager(nullptr),
    m_anim(nullptr),
    m_AIState(MonsterAIState::Wait)
{
    SetPosition(Vector2<float>{ startX, startY });
    SetColliderLocalPosition(Vector2<float>{65, 100});
    InitCollider(startX + GetColliderLocalPosition().x, startY + GetColliderLocalPosition().y, 160, 120);
    InitSpriteManager(pMonsterImgData, 216, 216);
    InitAnimation();
    SetPhysicsType(PhysicsType::Dynamic);
    SetHealth(100.0f);
    SetForwardVector(Vector2<float>(-1.0f, 0.0f));
}



Centipede::~Centipede() {
    if (m_spriteManager) {
        delete m_spriteManager;
        m_spriteManager = nullptr;
    }
    if (m_anim) {
        delete m_anim;
        m_anim = nullptr;
    }
}

void Centipede::Update(float deltaTime) {

    // �߷����� ������ ���� �ٴ�üũ
    m_isGrounded = CheckGround();

    UpdateAI(deltaTime);

    // �߷�����
    Vector2<float> pos = GetTransform().position;
    if (!m_isGrounded) {
        m_verticalVelocity += GRAVITY_ACCEL * deltaTime;
        pos.y += m_verticalVelocity * deltaTime;
    }
    else {
        m_verticalVelocity = 0.0f;
    }

    SetPosition(pos);

    if (m_anim) {
        m_anim->Update(deltaTime);
        UpdateAnimFSM();
    }

    Monster::Update(deltaTime);
}

void Centipede::Render(CDDrawDevice* pDevice) {
    if (pDevice && m_spriteManager && m_anim) {
        // ���� �ִϸ��̼� �����ӿ� �ش��ϴ� srcRect�� SpriteManager���� ������
        int currentFrame = m_anim->GetCurrentFrame();
        const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);


        const float FLIP_RENDER_OFFSET_X = 75.0f;


        // ȭ����� ������ ��ġ
        Vector2<float> renderPos = GetRenderPosition();

        if (GetForwardVector().x <= 0) {
            pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }

        else {
            pDevice->DrawSpriteFlip(static_cast<int>(renderPos.x + FLIP_RENDER_OFFSET_X), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }
    }

    Monster::Render(pDevice);
}


int Centipede::GetSpriteFrameWidth() const {
    return m_spriteManager ? m_spriteManager->GetFrameWidth() : 0;
}

int Centipede::GetSpriteFrameHeight() const {
    return m_spriteManager ? m_spriteManager->GetFrameHeight() : 0;
}

MonsterType Centipede::GetMonsterType() const {
    return MonsterType::Centipede;
}

void Centipede::TakeDamage(const DamageInfo& damageInfo)
{
    int currentHP = GetHealth();
    SetHealth(currentHP - damageInfo.damageAmount);

    CentipedeAnimState currentState = m_anim->GetState();

    if (!m_isAttacking) {
        m_anim->SetState(CentipedeAnimState::Hurt);
        m_isHurt = true;
    }

    if (GetHealth() <= 0) {
        Dead();
    }
}

void Centipede::InitCollider(int x, int y, int width, int height) {
    if (m_pCollider)
        delete m_pCollider;

    Vector2<float> pos = GetTransform().position;
    m_pCollider = new AABBCollider(this, x, y, width, height);
    m_pCollider->SetCollisionLayer(MONSTER_COLLISION_LAYER);
    ColliderManager::GetInstance()->AddCollider(GetCollider());
}

void Centipede::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight) {
    if (m_spriteManager)
        delete m_spriteManager;

    m_spriteManager = new SpriteManager;
    m_spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
}

void Centipede::InitAnimation() {
    if (!m_spriteManager) {
        DebugManager::GetInstance()->AddOnScreenMessage(L"SpriteManager is not initialized in Centipede::InitAnimation()", 20.0f);
    }

    if (m_anim)
        delete m_anim;

    m_anim = new CentipedeAnim;

    std::vector<CentipedeAnim::AnimSequence> sequences;
    const float SECONDS_TO_MS = 1000.0f;
    int currentFrame = 0;

    // ������ ������ ��������Ʈ �̹����� ������ ��

    // Attack1: 4 ������
    sequences.push_back({ CentipedeAnimState::Attack1, currentFrame, currentFrame + 3, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack2: 6 ������
    sequences.push_back({ CentipedeAnimState::Attack2, currentFrame, currentFrame + 5, 0.08f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack3: 6 ������
    sequences.push_back({ CentipedeAnimState::Attack3, currentFrame, currentFrame + 5, 0.08f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack4: 4 ������
    sequences.push_back({ CentipedeAnimState::Attack4, currentFrame, currentFrame + 3, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Death: 4 ������
    sequences.push_back({ CentipedeAnimState::Death, currentFrame, currentFrame + 3, 0.15f * SECONDS_TO_MS, false });
    currentFrame += 6;

    // Hurt: 2 ������
    sequences.push_back({ CentipedeAnimState::Hurt, currentFrame, currentFrame + 1, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Idle: 4 ������
    sequences.push_back({ CentipedeAnimState::Idle, currentFrame, currentFrame + 3, 0.15f * SECONDS_TO_MS });
    currentFrame += 6;

    // Sneer: 6 ������
    sequences.push_back({ CentipedeAnimState::Sneer, currentFrame, currentFrame + 5, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Walk: 4 ������
    sequences.push_back({ CentipedeAnimState::Walk, currentFrame, currentFrame + 3, 0.12f * SECONDS_TO_MS });
    currentFrame += 6;

    m_anim->SetSequences(sequences);
    m_anim->SetState(CentipedeAnimState::Idle);

    m_attack2EndFrame = m_anim->GetSequence(CentipedeAnimState::Attack2)->endFrame;
    m_hurtEndFrame = m_anim->GetSequence(CentipedeAnimState::Hurt)->endFrame;
    
}

void Centipede::UpdateAnimFSM() {

    // ������� �� ������ Death ����
    if (m_isDead) {
        m_anim->SetState(CentipedeAnimState::Death);
        return;
    }

    if (m_isHurt) {
        if (m_anim->GetCurrentFrame() >= m_hurtEndFrame) {
            m_isHurt = false;
        }
        else {
            m_anim->SetState(CentipedeAnimState::Hurt);
            return;
        }
    }


    if (m_isAttacking) {
        m_anim->SetState(CentipedeAnimState::Attack2);
        return;
    }

    const float speedThreshold = 1.0f;
    if (std::abs(m_speed) > speedThreshold) {
        m_anim->SetState(CentipedeAnimState::Walk);
    }
    else {
        m_anim->SetState(CentipedeAnimState::Idle);
    }

}



void Centipede::UpdateAI(float deltaTime) {

    if (m_isDead)
    {
        return;
    }

    const float MS_TO_SEC = 0.001f;
    const float SEC_TO_MS = 1000.0f;

    m_waitTime = 2.0f * SEC_TO_MS;
    m_attackDelay = 2.0f * SEC_TO_MS;

    // ���Ͱ� ���ϴ� ���⿡ ���� �ִ��� ����
    bool forwardObjectDetected = CheckForwardObject();

    /*
    std::wstring ObjectDetectMessage = L"LeftObject: " + std::wstring(leftObjectDetected ? L"true" : L"false") +
        L", RightObject: " + std::wstring(rightObjectDetected ? L"true" : L"false");

    DebugManager::GetInstance()->AddOnScreenMessage(ObjectDetectMessage, 0.03f);
    */
 
    if (m_attackDelayTimer <= 0.0f && m_AIState != MonsterAIState::Chase && m_AIState != MonsterAIState::Attack) {
        // Ray ���� ���� 200.0f
        Character* detectedPlayer = DetectPlayer(200.0f);
        if (detectedPlayer) {
            m_chaseTarget = detectedPlayer;
            m_AIState = MonsterAIState::Chase;
        }
    }


    if (m_attackDelayTimer > 0.0f) {
        m_attackDelayTimer -= deltaTime;
    }


    // AI ���� FSM (���ѻ��±��)
    switch (m_AIState) {
    case MonsterAIState::Wait:
    {
        m_speed = 0;

        // ��� �ð� ��� �˻�
        if (m_currentWaitTimer > 0.0f) {
            m_currentWaitTimer -= deltaTime;
        }
        else {
            if (!CheckForwardGround() || forwardObjectDetected) {
                m_AIState = MonsterAIState::Turn;
            }
            else {
                m_AIState = MonsterAIState::Move;
            }
        }
        break;
    }


    case MonsterAIState::Move:
    {
        // �̵� ó��
        if (!CheckForwardGround() || forwardObjectDetected) {
            m_speed = 0;
            m_AIState = MonsterAIState::Wait;
            m_currentWaitTimer = m_waitTime;        // waitTime��ŭ ���
        }
        else {
            Vector2<float> pos = GetTransform().position;
            m_speed = m_baseSpeed;
            pos.x += m_speed * GetForwardVector().x;
            SetPosition(pos);
        }
        break;
    }


    case MonsterAIState::Turn:
    {
        SetForwardVector(-GetForwardVector());
        m_AIState = MonsterAIState::Wait;
        m_currentWaitTimer = m_waitTime;
        break;
    }


    case MonsterAIState::Attack:
    {
        if (!m_isAttacking) {
            m_anim->SetState(CentipedeAnimState::Attack2);
            m_isAttacking = true;
        }

        // ���� ������ ���, Ư�� �����ӿ� �����ϸ� �� ���� Attack() ȣ��
        if (m_isAttacking) {
            if (!m_attack2OverlapBoxSpawned && m_anim->GetCurrentFrame() >= m_attack2EndFrame - 3) {
                Attack();           // overlapBox �������� ����
                m_attack2OverlapBoxSpawned = true;
            }

            // ���� ���� �����ӿ� ���� ��
            if (m_anim->GetCurrentFrame() >= m_attack2EndFrame) {
                m_isAttacking = false;
                m_attack2OverlapBoxSpawned = false;
                m_attackDelayTimer = m_attackDelay;  // ���� �� ������ Ÿ�̸� �缳��
                m_AIState = MonsterAIState::Chase;    // �߰� ���·� ��ȯ
            }
        }
        break;
    }


    case MonsterAIState::Chase:
    {
        Chase();        // Chase �Լ����� AI FSM ��ȯ���� ����
        break;
    }

    default:
        break;
    }

    /*
    std::wstring AIStateMessage = L"MonsterState: " + MonsterAIStateToWString(m_AIState);
    DebugManager::GetInstance()->AddOnScreenMessage(AIStateMessage, 0.03f);
    */

}



void Centipede::Attack() {
    AABBCollider* curCollider = dynamic_cast<AABBCollider*>(GetCollider());
    if (!curCollider)
        return;

    Vector2<float> ColliderCenterPos = curCollider->GetCenterPosition();
    
    // OBB �ڽ�
    Vector2<float> boxCenter = ColliderCenterPos + Vector2<float>{85.0f * GetForwardVector().x, -35.0f};
    float boxWidth = 120.0f;
    float boxHeight = 80.0f;
    float boxRotation = 0.0f;

    // ����� �� �ڽ�
    DebugBox debugBox(boxCenter, boxWidth, boxHeight, boxRotation, RGB(255, 255, 0), 2.0f);
    DebugManager::GetInstance()->AddDebugBox(debugBox);

    CollisionLayer queryMask = CHARACTER_COLLISION_LAYER & ~MONSTER_COLLISION_LAYER;
    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
    for (Collider* collider : colliders) {
        if (collider == curCollider)
            continue;

        if (CollisionQuery::OverlapBox(boxCenter, boxWidth, boxHeight, boxRotation, collider, queryMask)) {
            GameObject* targetObject = collider->GetOwner();
            if (targetObject) {
                DamageInfo damageInfo(25.0f, this, boxCenter);
                targetObject->TakeDamage(damageInfo);
            }
        }
    }
}

void Centipede::Chase()
{
    if (!m_chaseTarget) {
        // ����� ������ Wait ���·� ��ȯ
        m_AIState = MonsterAIState::Wait;
        return;
    }

    // Collider �߽��� �������� ��ġ ���
    Vector2<float> curPos = GetCollider()->GetCenterPosition();
    Vector2<float> targetPos = m_chaseTarget->GetCollider()->GetCenterPosition();
    Vector2<float> toTargetVec = targetPos - curPos;
    float distance = toTargetVec.Length();

    m_attackRange = 150.0f;
    m_chaseStopDistance = 600.0f;

    // ���� ���� ��ȯ ����
    if (m_attackDelayTimer <= 0.0f  && distance < m_attackRange) {
        m_AIState = MonsterAIState::Attack;
        return;
    }


    // Chase ���� ��, ���� �÷��̾���� �Ÿ��� �ʹ� �ָ�(m_chaseStopDistance �̻�) ��� ���·� ��ȯ
    if (distance >= m_chaseStopDistance) {
        m_AIState = MonsterAIState::Wait;
        m_currentWaitTimer = m_waitTime;
        return;
    }

    /*
    �÷��̾ ���� �ٷ� ���� ���� �� ���� ������ ���� �¿�� ��ӿԴ� ���� �Ÿ�
        -> ���Ϳ� �÷��̾���� x��Ÿ��� �ʹ����� ����� �� ������ȯ�� ���ϵ���
    */

    float xDiff = std::abs(targetPos.x - curPos.x);
    const float xDiffThreshold = 20.0f;

    // �÷��̾� �������� �̵�: ���� Forward ���Ϳ� �÷��̾� ����(toTargetVec)�� �������� �Ǵ�
    Vector2<float> forwardVec = GetForwardVector();
    Vector2<float> normalizedToTargetVec = Normalize(toTargetVec);
    if (Dot(normalizedToTargetVec, forwardVec) < 0) {
        // �÷��̾ ���� �ڿ� �ִٸ�, forward ���� ����

        // �÷��̾���� �Ÿ��� �ʹ� ������ ������ȯ�� ���ϵ���
        if (xDiff > xDiffThreshold)
        {
            SetForwardVector(-forwardVec);
        }
    }



    m_speed = m_baseSpeed;
    
    // �÷��̾��� ��ġ�� pos�� �������� ������Ʈ
    Vector2<float> pos = GetPosition();

    if (CheckForwardGround() && !CheckForwardObject())
    {
        pos += (m_speed * GetForwardVector());
        SetPosition(pos);
    }


    // �����: �÷��̾���� �Ÿ� ���
    std::wstring chaseMsg = L"Chasing: Distance = " + std::to_wstring(distance);
    DebugManager::GetInstance()->AddOnScreenMessage(chaseMsg, 0.03f);
}


void Centipede::Dead()
{
    if (m_isDead)
        return;

    m_isDead = true;

    m_anim->SetState(CentipedeAnimState::Death);
    GetCollider()->SetCollisionResponse(CollisionResponse::Ignore);
    DebugManager::GetInstance()->AddOnScreenMessage(L"Centipede Dead!", 2.0f);
}
