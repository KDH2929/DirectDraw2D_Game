#include "stdafx.h"
#include "CollisionQuery.h"
#include <algorithm>
#include <climits>

namespace CollisionQuery {

    // queryMask: �浹 �˻� ��� ���̾���� ��Ʈ����ũ
    // ���� (queryMask & collider->GetLayer())�� 0�̸�, �� collider�� �˻� ����� �ƴ�
    bool Raycast(const Ray& ray, const Collider* collider, float maxDistance, float& outDistance, CollisionLayer queryMask) {
        // Bitwise AND ������ ���� collider�� queryMask�� ���ԵǾ� �ִ��� Ȯ��
        if ((queryMask & collider->GetCollisionLayer()) == 0) {
            return false;
        }

        if (collider->GetCollisionResponse() == CollisionResponse::Ignore) {
            return false;
        }

        switch (collider->GetType()) {
        case ColliderType::AABB:
            return RaycastAABB(ray, static_cast<const AABBCollider*>(collider), maxDistance, outDistance);
        case ColliderType::OBB:
            return RaycastOBB(ray, static_cast<const OBBCollider*>(collider), maxDistance, outDistance);
        default:
            return false;
        }
    }

    bool RaycastAABB(const Ray& ray, const AABBCollider* aabb, float maxDistance, float& outDistance) {
        // AABB�� �»�� ���� ��ǥ�� ũ�⸦ �̿��� �ּ�/�ִ� ��ǥ ���
        Vector2<float> aabbMin(static_cast<float>(aabb->GetX()), static_cast<float>(aabb->GetY()));
        Vector2<float> aabbMax(static_cast<float>(aabb->GetX() + aabb->GetWidth()),
            static_cast<float>(aabb->GetY() + aabb->GetHeight()));

        // ������ ������ AABB ���ο� ������ ��� �浹�� ó��
        if (ray.origin.x >= aabbMin.x && ray.origin.x <= aabbMax.x &&
            ray.origin.y >= aabbMin.y && ray.origin.y <= aabbMax.y) {
            outDistance = 0.0f;
            return true;
        }


        /*
        Slab Test
        �ٽ� ����: �� ��(x, y)���� �ڽ��� �ּ�/�ִ� ���� ������ �����ϴ� �Ű����� t�� ���ϴ� ��
        ������ �Ű����� ������ : P(t) = O + tD
         - P(t): ���� ���� �� ��
         - O: ������ ������ (ray.origin)
         - D: ������ ���� ���� (ray.direction)
         - t: ������ ���� �Ű����� (ray�� �����ϴ� �Ÿ� ����)

        �� ���� AABB ���� �����ϴ� t���� ���ϱ�   (���� ������ t�� ���� ����)
            tmin = (AABB_Min - ray.origin) / ray.direction
            tmax = (AABB_Max - ray.origin) / ray.direction
        tmin�� tmax�� ���� ������ �����ϸ�, ��� �࿡�� ���� ������ �����ϸ� �浹.

        �������� t�� �� ���Ŀ����� �Ű����� t�� �ǹ�.

        */



        float txmin, txmax, tymin, tymax;


        // x �� ó��: Slab Test�� ���� ������ AABB�� x�� ��� ���� ���θ� ��� (Kay-Kajiya test)
        // ���� ������ x ������ 0�� ���, slab ���� ���θ� ���� ����� �� �����Ƿ� ������ ó��

        if (ray.direction.x != 0.0f) {
            // P(t) = O + tD ��  t���� ����
            // t = ( P(t) - O ) / D    ������ �״�� ���� ��
            
            txmin = (aabbMin.x - ray.origin.x) / ray.direction.x;       // AABB Box�� x�� �ּڰ��� ���� t�� 
            txmax = (aabbMax.x - ray.origin.x) / ray.direction.x;      // AABB Box�� y�� �ּڰ��� ���� t��

            // swap�� ���� �� �κ��� t_enter�� �׻� ���ʿ��� �����ϴ� ������ ����ϸ� ��
            if (txmin > txmax)
            {
                std::swap(txmin, txmax);
            }
        }

        else {
            // x�� ������ 0�ε� �������� x�� AABB �ڽ��� x������ ����ٸ� false
            // �ֳ��ϸ� y�༺���� �󸶵��� ���� �����ϰ� ��
            if (ray.origin.x < aabbMin.x || ray.origin.x > aabbMax.x)
                return false;

            // ���� ���� ���� ���Ѵ�, ��� ���Ѵ�� �ξ� �ش� ���� ������� �ʵ��� ��
            // �� �ش� �࿡ ���ؼ��� Slab �������θ� �������� ����
            txmin = -FLT_MAX;
            txmax = FLT_MAX;
        }

        // y �� ó��
        if (ray.direction.y != 0.0f) {

            // P(t) = O + tD ��  t���� ����
            // t = ( P(t) - O ) / D    ������ �״�� ���� ��

            tymin = (aabbMin.y - ray.origin.y) / ray.direction.y;       // AABB Box�� y�� �ּڰ��� ���� t�� 
            tymax = (aabbMax.y - ray.origin.y) / ray.direction.y;      // AABB Box�� y�� �ִ��� ���� t�� 

            // swap�� ���� �� �κ��� t_enter�� �׻� ���ʿ��� �����ϴ� ������ ����ϸ� ��
            if (tymin > tymax) {
                std::swap(tymin, tymax);
            }
        }
        else {

            // y�� ������ 0�ε� �������� y�� AABB �ڽ� ������ y�� ����ٸ� false
            // �ֳ��ϸ� x�༺���� �󸶵��� ���� �����ϰ� ��
            if (ray.origin.y < aabbMin.y || ray.origin.y > aabbMax.y)
                return false;

            // ���� ���� ���� ���Ѵ�, ��� ���Ѵ�� �ξ� �ش� ���� ������� �ʵ��� ��
            // �� �ش� �࿡ ���ؼ��� Slab �������θ� �������� ����
            tymin = -FLT_MAX;
            tymax = FLT_MAX;
        }

        // �� ���� ���� ������ ��ġ�� ������ �浹 ����
        if ((txmin > tymax) || (tymin > txmax))
            return false;


        // ������ swap ó���� �Ͽ��⿡ �Ʒ� ������ �׻� ����
        
        // tmin: AABB�� �����ϴ� ������ �Ű����� t�� (��, AABB�� ó������ ������ ����)
        // tmax: AABB���� ��Ż�ϴ� ������ �Ű����� t�� (��, AABB�� ���������� ������ ����)

        float t_enter, t_exit;

        // ���� tmin, tmax ����
        t_enter = std::max<float>(txmin, tymin);
        t_exit = std::min<float>(txmax, tymax);


        // Ray�� AABB ���ʿ��� �����ϴ� ���
        if (t_exit < 0)
            return false;


        // �浹�� maxDistance �̳����� Ȯ��
        if (t_enter <= maxDistance) {
            outDistance = t_enter;
            return true;
        }

        return false;
    }

    bool RaycastOBB(const Ray& ray, const OBBCollider* obb, float maxDistance, float& outDistance) {
        // OBB�� AABB�� ������ �����ϰ�, ��� ȸ���� ���� ������ǥ��� �Ͽ� ����ϸ� ��
        // �� Ray�� ������ ������ ȸ���� ���� �������� ���缭 ���
        
        // OBB�� AABB ��ǥ��� ��ȯ
        float rad = degToRad(obb->GetRotation());
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);

        // OBB�� �߽� (���� ��ǥ)
        Vector2<float> obbCenter(static_cast<float>(obb->GetX()), static_cast<float>(obb->GetY()));

        float halfWidth = obb->GetWidth() / 2.0f;
        float halfHeight = obb->GetHeight() / 2.0f;

        // ������ ������ OBB �߽� �������� ��ȯ
        Vector2<float> localOrigin = ray.origin - obbCenter;
        // ������ ������ OBB�� ���� ��ǥ��� ��ȯ
        Vector2<float> transformedOrigin(
            localOrigin.x * cosR + localOrigin.y * sinR,
            -localOrigin.x * sinR + localOrigin.y * cosR
        );

        // OBB�� ���� AABB ���� (�߽��� 0,0�� ���ĵ� AABB)
        Vector2<float> localMin(-halfWidth, -halfHeight);
        Vector2<float> localMax(halfWidth, halfHeight);

        // ������ �������� OBB ���ο� ������ �ٷ� �浹�� ó��
        if (transformedOrigin.x >= localMin.x && transformedOrigin.x <= localMax.x &&
            transformedOrigin.y >= localMin.y && transformedOrigin.y <= localMax.y) {
            outDistance = 0.0f;
            return true;
        }

        // ���� ���⵵ OBB�� ���� ��ǥ��� ��ȯ
        Vector2<float> transformedDirection(
            ray.direction.x * cosR + ray.direction.y * sinR,
            -ray.direction.x * sinR + ray.direction.y * cosR
        );

        float txmin, txmax, tymin, tymax;

        // X�� Slab Test
        if (transformedDirection.x != 0.0f) {
            txmin = (localMin.x - transformedOrigin.x) / transformedDirection.x;
            txmax = (localMax.x - transformedOrigin.x) / transformedDirection.x;
            if (txmin > txmax) std::swap(txmin, txmax);
        }
        else {
            if (transformedOrigin.x < localMin.x || transformedOrigin.x > localMax.x)
                return false;
            txmin = -FLT_MAX;
            txmax = FLT_MAX;
        }

        // Y�� Slab Test
        if (transformedDirection.y != 0.0f) {
            tymin = (localMin.y - transformedOrigin.y) / transformedDirection.y;
            tymax = (localMax.y - transformedOrigin.y) / transformedDirection.y;
            if (tymin > tymax) std::swap(tymin, tymax);
        }
        else {
            if (transformedOrigin.y < localMin.y || transformedOrigin.y > localMax.y)
                return false;
            tymin = -FLT_MAX;
            tymax = FLT_MAX;
        }

        // Slab ���� ���� Ȯ��
        if ((txmin > tymax) || (tymin > txmax))
            return false;

        // ���� ����/��Ż ���� ���
        float t_enter = std::max<float>(txmin, tymin);
        float t_exit = std::min<float>(txmax, tymax);

        // Ray�� OBB ���ʿ��� �����ϴ� ���
        if (t_exit < 0)
            return false;

        // �浹�� maxDistance �̳����� Ȯ��
        if (t_enter <= maxDistance) {
            outDistance = t_enter;
            return true;
        }

        return false;
    }




    bool OverlapBox(const Vector2<float>& center, float width, float height, float rotation, const Collider* collider, CollisionLayer queryMask)
    {
        // QueryMask ����
        if ((collider->GetCollisionLayer() & queryMask) == 0) {
            return false;
        }

        if (collider->GetCollisionResponse() == CollisionResponse::Ignore) {
            return false;
        }

        OBBCollider queryOBB(nullptr, center.x, center.y, width, height, rotation);

        return queryOBB.CheckCollision(collider);
    }

}
