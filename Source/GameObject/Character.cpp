#include "stdafx.h"
#include "Character.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "../ImageData/ImageData.h"
#include "../DDrawLib/DDrawDevice.h"
#include "InputManager.h"
#include "DebugManager.h"
#include "SpriteManager.h"
#include "CharacterAnim.h"
#include "CollisionQuery.h"
#include "Monster.h"
#include "SceneManager.h"

#include <typeinfo>
#include <algorithm>


Character::Character(CImageData* pPlayerImgData, float startX, float startY)
    : m_health(100.0f), m_speed(8.0f), m_verticalVelocity(0.0f), m_isGrounded(false), 
    m_spriteManager(nullptr), m_anim(nullptr)
{
    SetPosition(Vector2<float>{ startX, startY });
    SetColliderLocalPosition(Vector2<float>{65, 20});
    SetPhysicsType(PhysicsType::Dynamic);
    SetSpeed(8.0f);
    SetHealth(100.0f);
    InitCollider(startX + GetColliderLocalPosition().x, startY + GetColliderLocalPosition().y, 85, 155);
    InitSpriteManager(pPlayerImgData, 276, 176);
    InitAnimation();
}


Character::~Character()
{
    if (m_spriteManager) {
        delete m_spriteManager;
        m_spriteManager = nullptr;
    }

    if (m_anim) {
        delete m_anim;
        m_anim = nullptr;
    }
}


void Character::ProcessInput()
{
    if (m_isDead)
        return;

    Vector2<float> pos = GetTransform().position;

    // �¿� �̵� �� �޸��� ���� ����
    if (InputManager::GetInstance().IsKeyDown(VK_LEFT)) {

        m_isRunning = true;

        if (m_leftWallDetected == false)
        {
            pos.x -= GetSpeed();
        }

        if (!m_isDashing && !m_isAttacking)
        {
            m_forwardVec = Vector2<float>(-1.0f, 0.0f);
        }

    }
    if (InputManager::GetInstance().IsKeyDown(VK_RIGHT)) {

        m_isRunning = true;

        if (m_rightWallDetected == false)
        {
            pos.x += GetSpeed();
        }

        if (!m_isDashing && !m_isAttacking)
        {
            m_forwardVec = Vector2<float>(1.0f, 0.0f);
        }
    }

    if (InputManager::GetInstance().IsKeyDown(VK_UP))
    {
        //pos.y -= GetSpeed();
    }

    if (InputManager::GetInstance().IsKeyDown(VK_DOWN))
    {
        //pos.y += GetSpeed();
    }

    // ���� �Է� ó�� (C Ű)
    if (InputManager::GetInstance().IsKeyPressed('C')) {

        // ���� ��� ���̸� ���� �Է��� ��ø� ĵ���ϰ� ���� ��ȯ
        if (m_isDashing && !m_isDashAttacking) {
            m_isDashAttacking = true;
            m_anim->SetState(CharacterAnimState::DashAttack);
        }


        else if (!m_isAttacking && !m_isDashAttacking) {
            // ó�� ���� �Է�: ù ���� ����
            m_isAttacking = true;
            m_attackComboStage = 1;
            m_currentAttackEndFrame = m_firstAttackEndFrame;
            m_comboInputReceived = false;
            m_anim->SetState(CharacterAnimState::Attack);
        }

        else {
            // �̹� ���� ���̸� �߰� �Է� �÷��� ����
            m_comboInputReceived = true;
        }

    }


    if (InputManager::GetInstance().IsDoublePressedLeft() && !m_isDashing) {
        // ��� �Է��� �������¸� ĵ���ϰ� ��� ���·� ��ȯ

        DebugManager::GetInstance()->AddOnScreenMessage(L"LeftKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    if (InputManager::GetInstance().IsDoublePressedRight() && !m_isDashing) {
        // ��� �Է��� ���� ���¸� ĵ���ϰ� ��� ���·� ��ȯ

        DebugManager::GetInstance()->AddOnScreenMessage(L"RightKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    // ���� �Է� ó�� (X Ű, ���� ���� ���¿�����)
    if (InputManager::GetInstance().IsKeyPressed('X') && m_isGrounded) {
        //DebugManager::GetInstance()->AddOnScreenMessage(L"Key X is pressed", 5.0f);
        m_verticalVelocity = JUMP_VELOCITY;
        m_isGrounded = false;
    }

    SetPosition(pos);

}

void Character::Update(float deltaTime)
{

    if (m_isDead)
    {
        m_deathTimer += deltaTime;
        if (m_deathTimer >= GAME_OVER_DELAY * SECOND_TO_MS)
        {
            SceneManager::GetInstance()->AsyncChangeScene(SceneType::GameOver);
        }
    }


    // Raycast �� ���� �� �˻�
    m_leftWallDetected = CheckLeftWall();
    m_rightWallDetected = CheckRightWall();

    
    // Racycast�� ���� ����˻�
    float groundHitDistance = 0.0f;
    m_isGrounded = CheckGround(groundHitDistance);
    

    // �� ������ �޸��� ���� �ʱ�ȭ
    m_isRunning = false;


    ProcessInput();


    Vector2<float> pos = GetTransform().position;


    // ���� ������ ���� �κе��� �Ʒ��� ���� �Ĳ��� ó���� ��
    // ���� �ִ� ���¿��� ��ø� �ص� ������ �з����⿡ ������ ������ ��� ���� ��¦ ���� ���� ������ ���� 
    if (m_isDashing && !m_leftWallDetected && !m_rightWallDetected)
    {
        pos.x += m_forwardVec.x * m_dashSpeed;
    }


    // '�������� ����' ���� ������Ʈ: ���� �����̸� �ϰ� ���� ��, raycast hitDistance�� ���� ���� ���̸� true
    const float upToFallThreshold = 60.0f;
    if (!m_isGrounded && m_verticalVelocity > 0) {
        m_isUpToFall = (groundHitDistance <= upToFallThreshold);
    }
    else {
        m_isUpToFall = false;
    }


    // �߷� ���� (���� ���� ���� ���)
    if (!m_isGrounded) {
        m_verticalVelocity += GRAVITY_ACCEL * deltaTime;
        pos.y += m_verticalVelocity * deltaTime;
    }

    else {

        // DebugManager::GetInstance()->AddOnScreenMessage(L"Player is on ground", 5.0f);
        m_verticalVelocity = 0;
    }

    SetPosition(pos);


    // �ִϸ��̼� ������Ʈ
    if (m_anim)
    {
        m_anim->Update(deltaTime);
        UpdateAnimFSM();
    }




    // ���� �ִϸ��̼� ���¿��� �޺� ó��
    if (m_isAttacking && m_anim->GetState() == CharacterAnimState::Attack) {

        // ù��° ���Ӱ����� �������� 3�����ӿ� �������� ����
        if (!m_firstAttackOverlapBoxSpawned && m_anim->GetCurrentFrame() == m_firstAttackEndFrame - 3)
        {
            Attack(CharacterAttackType::ComboAttack1);

            m_firstAttackOverlapBoxSpawned = true;
        }

        // ���� ���� �ִϸ��̼��� ���� �����ӿ� ����������
        if (m_anim->GetCurrentFrame() >= m_currentAttackEndFrame) {
            if (m_comboInputReceived) {
                // �߰� �Է��� �־����� ���� �޺� �ܰ�� ��ȯ
                m_attackComboStage++;
      
                if (m_attackComboStage == 2) {
                    m_currentAttackEndFrame = m_secondAttackEndFrame;
                    m_comboInputReceived = false;

                    Attack(CharacterAttackType::ComboAttack2);
                }

                else if (m_attackComboStage >= 3)
                {
                    // ���Ӱ�������
                    m_isAttacking = false;
                    m_comboInputReceived = false;
                    m_attackComboStage = 0;
                    m_currentAttackEndFrame = m_firstAttackEndFrame;
                    m_firstAttackOverlapBoxSpawned = false;
                }
                

                // �߰� �޺� �ܰ谡 �� �ִٸ� else if (m_attackComboStage == 3) ... ���� �߰�
            }

            else {
                // �߰� �Է��� ������ ���� ����
                m_isAttacking = false;
                m_comboInputReceived = false;
                m_attackComboStage = 0;
                m_currentAttackEndFrame = m_firstAttackEndFrame;
                m_firstAttackOverlapBoxSpawned = false;
            }
        }
    }



    if (m_isDashing && m_anim->GetState() == CharacterAnimState::Dash) 
    {
        if (m_anim->GetCurrentFrame() >= m_DashEndFrame) {
            m_isDashing = false;
            m_isAttacking = false;
        }
    }

    if (m_isDashAttacking && m_anim->GetState() == CharacterAnimState::DashAttack)
    {
        // ��ð����� �������� 5�����ӿ� �������� ����
        if (!m_dashAttackOverlapBoxSpawned && m_anim->GetCurrentFrame() == m_DashAttackEndFrame - 5)
        {
            Attack(CharacterAttackType::DashAttack);

            m_dashAttackOverlapBoxSpawned = true;
        }

        if (m_anim->GetCurrentFrame() >= m_DashAttackEndFrame) {
            m_isDashAttacking = false;
            m_isAttacking = false;
            m_isDashing = false;

            m_dashAttackOverlapBoxSpawned = false;
        }
    }

    // �ڽ�Ŭ�������� �θ�Ŭ������ Update�� �������� ȣ���ϴ� ������ ������ġ�� ���� ��, 
    // ��ġ������ �θ�Ŭ�������� �����ϵ��� �����Ͽ��� ����
    
    GameObject::Update(deltaTime);
}



void Character::Render(CDDrawDevice* pDevice)
{
    if (pDevice && m_spriteManager && m_anim)
    {
        // ���� �ִϸ��̼� �����ӿ� �ش��ϴ� srcRect�� SpriteManager���� ������.
        int currentFrame = m_anim->GetCurrentFrame();
        const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);

        // ȭ����� ������ ��ġ
        Vector2<float> renderPos = GetRenderPosition();
        
        const float FLIP_RENDER_OFFSET_X = -50.0f;
        

        if (GetForwardVector().x >= 0) {
            pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }

        else {
            pDevice->DrawSpriteFlip(static_cast<int>(renderPos.x + FLIP_RENDER_OFFSET_X), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }
    }

    GameObject::Render(pDevice);
}


int Character::GetSpriteFrameWidth() const {
    return m_spriteManager ? m_spriteManager->GetFrameWidth() : 0;
}

int Character::GetSpriteFrameHeight() const {
    return m_spriteManager ? m_spriteManager->GetFrameHeight() : 0;
}

void Character::SetCameraOffset(Vector2<float> cameraOffset)
{
    m_cameraOffset = cameraOffset;
}

Vector2<float> Character::GetCameraOffset() const
{
    return m_cameraOffset;
}

Vector2<float> Character::GetForwardVector() const
{
    return m_forwardVec;
}

bool Character::CheckLeftWall()
{
    // �� Raycast �˻縦 ���� ����
    const float wallCheckDistance = 0.3f;
    float wallHitDistance = 0.0f;
    const float wallCheckRayOffsetY = 5.0f;

    Collider* curCollider = GetCollider();

    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // �÷��̾� �ݶ��̴��� ��� ���ϱ�
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();

            Vector2<float> topLeft(static_cast<float>(left), static_cast<float>(top) + wallCheckRayOffsetY);
            Vector2<float> bottomLeft(static_cast<float>(left), static_cast<float>(bottom) - wallCheckRayOffsetY);


            // ������ ���, �»�ܰ� ���ϴܿ��� �������� Ray�� ��
            Ray leftRayFromTop(topLeft, Vector2<float>(-1.0f, 0.0f));
            Ray leftRayFromBottom(bottomLeft, Vector2<float>(-1.0f, 0.0f));

            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // �ڱ��ڽ� Collider�� ����
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

            // Overlap ��ü�� ����
            queryMask = queryMask & ~OVERLAP_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }

                if (CollisionQuery::Raycast(leftRayFromTop, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(leftRayFromBottom, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Character::CheckRightWall()
{
    Collider* curCollider = GetCollider();

    const float wallCheckDistance = 0.3f;
    float wallHitDistance = 0.0f;
    const float wallCheckRayOffsetY = 5.0f;


    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // �÷��̾� �ݶ��̴��� ��� ���ϱ�
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();


            Vector2<float> topRight(static_cast<float>(right), static_cast<float>(top) + wallCheckRayOffsetY);
            Vector2<float> bottomRight(static_cast<float>(right), static_cast<float>(bottom) - wallCheckRayOffsetY);


            Ray rightRayFromTop(topRight, Vector2<float>(1.0f, 0.0f));
            Ray rightRayFromBottom(bottomRight, Vector2<float>(1.0f, 0.0f));


            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // �ڱ��ڽ� Collider�� ����
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

            // Overlap ��ü�� ����
            queryMask = queryMask & ~OVERLAP_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }


                if (CollisionQuery::Raycast(rightRayFromTop, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(rightRayFromBottom, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Character::CheckGround(float& groundHitDistance)
{
    // Raycast�� ���� ���� ���� (AABB �ݶ��̴� ����, ���� �ϴܿ��� �˻�)
    const float groundCheckRayOffsetX = 5.0f;
    const float groundThreshold = 0.3f;

    Collider* curCollider = GetCollider();

    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int bottom = aabb->GetY() + aabb->GetHeight();

            Vector2<float> leftOrigin(static_cast<float>(left), static_cast<float>(bottom));
            Vector2<float> rightOrigin(static_cast<float>(right), static_cast<float>(bottom));
            leftOrigin.x += groundCheckRayOffsetX;
            rightOrigin.x -= groundCheckRayOffsetX;

            Ray leftRay(leftOrigin, Vector2<float>(0.0f, 1.0f));
            Ray rightRay(rightOrigin, Vector2<float>(0.0f, 1.0f));

            // �ڱ��ڽ� Collider�� ����
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (CollisionQuery::Raycast(leftRay, collider, groundThreshold, groundHitDistance, queryMask)) {
                    return true;
                }
                if (CollisionQuery::Raycast(rightRay, collider, groundThreshold, groundHitDistance, queryMask)) {
                    return true;
                }
            }

        }
    }

    return false;
}


void Character::OnCollision(const CollisionInfo& collisionInfo)
{
    switch (collisionInfo.response) {
    case CollisionResponse::Block:
        break;
    case CollisionResponse::Overlap:
        break;
    default:
        break;
    }
}

void Character::TakeDamage(const DamageInfo& damageInfo)
{
    int currentHP = GetHealth();
    SetHealth(currentHP - damageInfo.damageAmount);

    CharacterAnimState currentState = m_anim->GetState();

    if (currentState != CharacterAnimState::DashAttack) {
        m_anim->SetState(CharacterAnimState::Hurt);
        m_isHurt = true;
    }


    if (GetHealth() <= 0) {
        Dead();
    }
}

void Character::Attack(CharacterAttackType attackType)
{

    float attackDamage = 0.0f;

    int attackTypeNum = static_cast<int>(attackType);

    DebugManager::GetInstance()->AddOnScreenMessage(L"AttackType: " + (std::to_wstring(attackTypeNum)), 0.8f);

    AABBCollider* curCollider = static_cast<AABBCollider*>(this->GetCollider());

    Vector2<float> ColliderCenterPos = curCollider->GetCenterPosition();

    // ���� Ÿ�Կ� ���� ��Ʈ�ڽ�(OBB) �Ķ����
    Vector2<float> boxCenter;
    float boxWidth = 0.0f, boxHeight = 0.0f, boxRotation = 0.0f;        // rotation: �� ����

    switch (attackType) {
    case CharacterAttackType::ComboAttack1:
        boxCenter = ColliderCenterPos + Vector2<float>{100.0f * GetForwardVector().x, -30.0f};
        boxWidth = 120.0f;
        boxHeight = 110.0f;
        boxRotation = -30.0f * GetForwardVector().x;
        attackDamage = 10.0f;
        break;

    case CharacterAttackType::ComboAttack2:
        boxCenter = ColliderCenterPos + Vector2<float>{100.0f * GetForwardVector().x, -30.0f};
        boxWidth = 125.0f;
        boxHeight = 112.0f;
        boxRotation = 30.0f * GetForwardVector().x;
        attackDamage = 10.0f;
        break;

    case CharacterAttackType::DashAttack:
        boxCenter = ColliderCenterPos + Vector2<float>{200.0f * GetForwardVector().x, 0.0f};
        boxWidth = 200.0f;
        boxHeight = 115.0f;
        boxRotation = 0.0f * GetForwardVector().x;
        attackDamage = 20.0f;
        break;

    default:
        break;
    }

    DamageInfo damageInfo(attackDamage, this, boxCenter);


    DebugBox debugBox(boxCenter, boxWidth, boxHeight, boxRotation, RGB(255, 0, 0), 2.0f);
    DebugManager::GetInstance()->AddDebugBox(debugBox);


    // �ڱ��ڽ��� Collider�� ����
    CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
    for (Collider* collider : colliders) {

        if (CollisionQuery::OverlapBox(boxCenter, boxWidth, boxHeight, boxRotation, collider, queryMask)) {
            GameObject* targetObject = collider->GetOwner();
            if (targetObject) {
                Monster* monster = dynamic_cast<Monster*>(targetObject);
                if (monster) {
                    monster->TakeDamage(damageInfo);
                }
            }
        }
    }
}

void Character::Dead()
{
    if (m_isDead)
        return;

    m_isDead = true;

    m_anim->SetState(CharacterAnimState::Death);
    GetCollider()->SetCollisionResponse(CollisionResponse::Ignore);
    DebugManager::GetInstance()->AddOnScreenMessage(L"Character Dead!", 2.0f);

}


void Character::InitCollider(float x, float y, float width, float height)
{
    if (m_pCollider)
        delete m_pCollider;

    Vector2<float> pos = GetTransform().position;
    m_pCollider = new AABBCollider(this, x, y, width, height);
    m_pCollider->SetCollisionLayer(CHARACTER_COLLISION_LAYER);
    ColliderManager::GetInstance()->AddCollider(GetCollider());
}


void Character::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight)
{
    if (m_spriteManager)
        delete m_spriteManager;

    m_spriteManager = new SpriteManager;
    m_spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
}


void Character::InitAnimation()
{
    if (!m_spriteManager)
    {
        DebugManager::GetInstance()->AddOnScreenMessage(L"SpriteManager is not initialized in Character::InitAnimation()", 20.0f);
    }

    if (m_anim)
        delete m_anim;

    m_anim = new CharacterAnim;

    std::vector<CharacterAnim::AnimSequence> sequences;

    int currentFrame = 0;
    // Idle : 6 frames
    sequences.push_back({ CharacterAnimState::Idle, currentFrame, (currentFrame + 6 - 1), 0.15f * SECOND_TO_MS });
    currentFrame += 6;

    // Run : 8 frames
    sequences.push_back({ CharacterAnimState::Run, currentFrame, (currentFrame + 8 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 8;

    // 2 Combo Attack : 12 frames
    sequences.push_back({ CharacterAnimState::Attack, currentFrame, (currentFrame + 12 - 1), 0.08f * SECOND_TO_MS });
    currentFrame += 12;

    // Death : 11 frames
    sequences.push_back({ CharacterAnimState::Death, currentFrame, (currentFrame + 11 - 1), 0.1f * SECOND_TO_MS, false });
    currentFrame += 11;

    // Hurt : 4 frames
    sequences.push_back({ CharacterAnimState::Hurt, currentFrame, (currentFrame + 4 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 4;

    // Jump : 3 frames
    sequences.push_back({ CharacterAnimState::Jump, currentFrame, (currentFrame + 3 - 1), 0.12f * SECOND_TO_MS });
    currentFrame += 3;

    // UpToFall : 2 frames
    sequences.push_back({ CharacterAnimState::UpToFall, currentFrame, (currentFrame + 2 - 1), 0.15f * SECOND_TO_MS, false });
    currentFrame += 2;

    // Fall : 3 frames
    sequences.push_back({ CharacterAnimState::Fall, currentFrame, (currentFrame + 3 - 1), 0.15f * SECOND_TO_MS });
    currentFrame += 3;

    // Edge Grab : 5 frames
    sequences.push_back({ CharacterAnimState::EdgeGrab, currentFrame, (currentFrame + 5 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 5;

    // Edge Idle : 6 frames
    sequences.push_back({ CharacterAnimState::EdgeIdle, currentFrame, (currentFrame + 6 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 6;

    // WallSlide : 3 frames
    sequences.push_back({ CharacterAnimState::WallSlide, currentFrame, (currentFrame + 3 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 3;

    // Crouch : 6 frames
    sequences.push_back({ CharacterAnimState::Crouch, currentFrame, (currentFrame + 6 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 6;

    // Dash : 7 frames
    sequences.push_back({ CharacterAnimState::Dash, currentFrame, (currentFrame + 7 - 1), 0.08f * SECOND_TO_MS });
    currentFrame += 7;

    // Dash-Attack : 10 frames
    sequences.push_back({ CharacterAnimState::DashAttack, currentFrame, (currentFrame + 10 - 1), 0.08f * SECOND_TO_MS });
    currentFrame += 10;

    // Slide : 5 frames
    sequences.push_back({ CharacterAnimState::Slide, currentFrame, (currentFrame + 5 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 5;

    // Ladder-Grab : 8 frames
    sequences.push_back({ CharacterAnimState::LadderGrab, currentFrame, (currentFrame + 8 - 1), 0.1f * SECOND_TO_MS });
    currentFrame += 8;

    m_anim->SetSequences(sequences);

    m_firstAttackEndFrame = m_anim->GetSequence(CharacterAnimState::Attack)->endFrame - 3;
    m_secondAttackEndFrame = m_firstAttackEndFrame + 2;
    m_DashEndFrame = m_anim->GetSequence(CharacterAnimState::Dash)->endFrame;
    m_DashAttackEndFrame = m_anim->GetSequence(CharacterAnimState::DashAttack)->endFrame;
    m_hurtEndFrame = m_anim->GetSequence(CharacterAnimState::Hurt)->endFrame;

    // �ʱ� ���¸� Idle�� ����
    m_anim->SetState(CharacterAnimState::Idle);
}



void Character::UpdateAnimFSM()
{
    CharacterAnimState currentState = m_anim->GetState();
    CharacterAnimState newState = currentState;

    // ������� �� ������ Death ����
    if (m_isDead) {
        m_anim->SetState(CharacterAnimState::Death);
        return;
    }

    if (m_isHurt) {
        if (m_anim->GetCurrentFrame() >= m_hurtEndFrame) {
            m_isHurt = false;
        }
        else {
            m_anim->SetState(CharacterAnimState::Hurt);
            return;
        }
    }



    // ��� ������ ��ÿ� ���ݺ��� �켱
    if (m_isDashAttacking) {
        newState = CharacterAnimState::DashAttack;
    }
    // ��ÿ� ������ ���� �Է� �� ���� ĵ���ϵ��� Update���� �����
    else if (m_isDashing) {
        newState = CharacterAnimState::Dash;
    }
    else if (m_isAttacking) {
        newState = CharacterAnimState::Attack;
    }
    else if (!m_isGrounded) {
        if (m_verticalVelocity < 0)         // ���� �ö󰡴� ��
            newState = CharacterAnimState::Jump;
        else
            newState = m_isUpToFall ? CharacterAnimState::UpToFall : CharacterAnimState::Fall;
    }
    else {
        newState = m_isRunning ? CharacterAnimState::Run : CharacterAnimState::Idle;
    }

    if (newState != currentState) {
        m_anim->SetState(newState);
    }
}