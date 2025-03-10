#include "stdafx.h"
#include "Monster.h"
#include "CollisionQuery.h"
#include "Character.h"
#include "DebugManager.h"

Monster::Monster() : GameObject() {

}

Monster::~Monster() {

}

void Monster::Update(float deltaTime)
{
    GameObject::Update(deltaTime);
}

void Monster::Render(CDDrawDevice* pDevice)
{
    GameObject::Render(pDevice);
}


const Vector2<float> Monster::GetForwardVector()
{
    return m_forwardVec;
}

void Monster::SetForwardVector(Vector2<float> forwardVec)
{
    m_forwardVec = forwardVec;
}

bool Monster::CheckLeftObject()
{
    // �� Raycast �˻縦 ���� ����
    const float ObjectCheckDistance = 0.3f;
    float ObjectHitDistance = 0.0f;
    const float ObjectCheckRayOffsetY = 5.0f;

    Collider* curCollider = GetCollider();

    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // �÷��̾� �ݶ��̴��� ��� ���ϱ�
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();

            Vector2<float> topLeft(static_cast<float>(left), static_cast<float>(top) + ObjectCheckRayOffsetY);
            Vector2<float> bottomLeft(static_cast<float>(left), static_cast<float>(bottom) - ObjectCheckRayOffsetY);


            // ������ ���, �»�ܰ� ���ϴܿ��� �������� Ray�� ��
            Ray leftRayFromTop(topLeft, Vector2<float>(-1.0f, 0.0f));
            Ray leftRayFromBottom(bottomLeft, Vector2<float>(-1.0f, 0.0f));

            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // �ڱ��ڽ� Collider�� ����
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }

                if (CollisionQuery::Raycast(leftRayFromTop, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(leftRayFromBottom, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Monster::CheckRightObject()
{
    Collider* curCollider = GetCollider();

    const float ObjectCheckDistance = 0.3f;
    const float ObjectCheckRayOffsetY = 5.0f;
    float ObjectHitDistance = 0.0f;


    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // �÷��̾� �ݶ��̴��� ��� ���ϱ�
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();


            Vector2<float> topRight(static_cast<float>(right), static_cast<float>(top) + ObjectCheckRayOffsetY);
            Vector2<float> bottomRight(static_cast<float>(right), static_cast<float>(bottom) - ObjectCheckRayOffsetY);


            Ray rightRayFromTop(topRight, Vector2<float>(1.0f, 0.0f));
            Ray rightRayFromBottom(bottomRight, Vector2<float>(1.0f, 0.0f));


            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // �ڱ��ڽ� Collider�� ����
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }


                if (CollisionQuery::Raycast(rightRayFromTop, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(rightRayFromBottom, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Monster::CheckForwardObject()
{
    const Vector2<float> forward = GetForwardVector();

    if (forward.x < 0)
    {
        return CheckLeftObject();
    }
    else if (forward.x > 0)
    {
        return CheckRightObject();
    }

    return false;
}

bool Monster::CheckGround()
{
    // Raycast�� ���� ���� ���� (AABB �ݶ��̴� ����, ���� �ϴܿ��� �˻�)
    const float groundCheckRayOffsetX = 5.0f;
    const float groundThreshold = 0.3f;
    float groundHitDistance = 0.0f;

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
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

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


bool Monster::CheckForwardGround() {

    Collider* curCollider = GetCollider();
    if (!curCollider)
        return false;
    
    AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider);
    if (!aabb)
        return false;

    // ������ Collider ��� ���ϱ�
    int left = aabb->GetX();
    int right = aabb->GetX() + aabb->GetWidth();
    int bottom = aabb->GetY() + aabb->GetHeight();

    // �̵� ���� (��: ������ �̵��̸� (1,0), ���� �̵��̸� (-1,0))
    Vector2<float> forward = GetForwardVector();

    const float forwardOffset = 20.0f;
    const float groundRayOffsetY = 5.0f; 


    Vector2<float> origin;
    
    // ������ �ٶ󺸴� ���
    if (forward.x > 0) {
        origin = Vector2<float>(right + forwardOffset, static_cast<float>(bottom));
        origin.y -= groundRayOffsetY;
    }

    // ������ �ٶ󺸴� ���
    else {
        origin = Vector2<float>(left - forwardOffset, static_cast<float>(bottom));
        origin.y -= groundRayOffsetY;
    }

    // �Ʒ� �������� Raycast (����: (0,1))
    Ray forwardGroundRay(origin, Vector2<float>(0.0f, 1.0f));

    const float groundThreshold = 500.0f;
    float groundHitDistance = 0.0f;
    CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

    queryMask &= ~OVERLAP_COLLISION_LAYER;

    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

    for (Collider* collider : colliders) {
        if (collider == curCollider)
            continue;
        if (CollisionQuery::Raycast(forwardGroundRay, collider, groundThreshold, groundHitDistance, queryMask)) {
            return true;
        }
    }
    return false;
}



Character* Monster::DetectPlayer(float rayDistance) {

    // ���ʹ� ���� AABB Collider
    Vector2<float> origin = static_cast<AABBCollider*>(GetCollider())->GetCenterPosition();
    Vector2<float> forward = GetForwardVector();
    float hitDistance = 0.0f;

    // Ray ����
    Ray detectRay(origin, forward);
    CollisionLayer queryMask = CHARACTER_COLLISION_LAYER & ~MONSTER_COLLISION_LAYER;

    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
    for (Collider* collider : colliders) {
        // �ڱ� �ڽ��� Collider�� ����
        if (collider == GetCollider())
            continue;


        Vector2<float> hitPoint = origin + forward * rayDistance;

        // DebugLine ����: �������� origin, ������ hitPoint, ������ ������, �β� 2, ���ӽð� 0.5��
        DebugLine debugLine(origin, hitPoint, RGB(255, 0, 0), 2, 0.03f);
        DebugManager::GetInstance()->AddDebugLine(debugLine);


        if (CollisionQuery::Raycast(detectRay, collider, rayDistance, hitDistance, queryMask)) {


            // �ݶ��̴��� Owner�� Character Ÿ������ Ȯ��
            Character* DetectedObject = dynamic_cast<Character*>(collider->GetOwner());
            if (DetectedObject != nullptr) {
              
                // Dynamic cast�� ĳ���� ���� �� nullptr �̶�� ���� Ȱ��

                DebugManager::GetInstance()->AddOnScreenMessage(L"Player Detected!", 1.0f);


                DebugLine debugLineHit(origin, hitPoint, RGB(0, 255, 0), 3, 2.0f);
                DebugManager::GetInstance()->AddDebugLine(debugLineHit);

                return DetectedObject;
            }
        }
    }
    return nullptr;
}


std::wstring Monster::MonsterAIStateToWString(MonsterAIState state) {
    switch (state) {
    case MonsterAIState::Wait:
        return L"Wait";
    case MonsterAIState::Move:
        return L"Move";
    case MonsterAIState::Turn:
        return L"Turn";
    case MonsterAIState::Attack:
        return L"Attack";
    case MonsterAIState::Chase:
        return L"Chase";
    case MonsterAIState::Dead:
        return L"Dead";
    default:
        return L"Unknown";
    }
}