#include "stdafx.h"
#include "../Common/typedef.h"
#include "../Util/Util.h"
#include "../Util/TGAImage.h"
#include "../DDrawLib/DDrawDevice.h"
#include "../Util/QueryPerfCounter.h"
#include "../ImageData/ImageData.h"
#include "GameObject.h"
#include "Character.h"
#include "Game.h"
#include "MathUtils.h"
#include "AABBCollider.h"
#include "InputManager.h"
#include "ColliderManager.h"
#include "Centipede.h"
#include "TileMap2D.h"
#include "Stage1Scene.h"

#include <algorithm>
#include <Windows.h>
#include <memory>


Stage1Scene::Stage1Scene(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice), m_hWnd(hWnd)
{
}

Stage1Scene::~Stage1Scene()
{
    Cleanup();
}

void Stage1Scene::Initialize()
{
    ColliderManager::GetInstance()->Initialize();


    // ȭ�� ũ�� ��������
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());
    
    LOG_MESSAGE(L"Character Image Loading Start");

    // �÷��̾� �̹��� �ε�
    CTGAImage* pPlayerImage = new CTGAImage;
    if (pPlayerImage->Load24BitsTGA("./data/Warrior_Sheet-Effect.tga", 4))
    {
        DWORD dwColorKey = pPlayerImage->GetPixel(0, 0);
        m_pPlayerImgData = new CImageData;
        m_pPlayerImgData->Create(pPlayerImage->GetRawImage(),
            pPlayerImage->GetWidth(),
            pPlayerImage->GetHeight(),
            dwColorKey);
    }
    delete pPlayerImage;
    pPlayerImage = nullptr;


    LOG_MESSAGE(L"Character Image Loading Complete");


    LOG_MESSAGE(L"TileMap Image Loading Start");

    // Ÿ�ϸ� ���� �� �ε�
    m_TileMap = new TileMap2D(0, 0);
    m_TileMap->ReadTileMap("./data/TileMap/level.tilemap2");
    m_bgWidth = m_TileMap->GetTileMapWidth();
    m_bgHeight = m_TileMap->GetTileMapHeight();

    LOG_MESSAGE(L"TileMap Image Loading Finished");



    LOG_MESSAGE(L"Monster Image Loading Start");

    // ����(centipede) �̹��� �ε�
    CTGAImage* pCentipedeImage = new CTGAImage;
    if (pCentipedeImage->Load24BitsTGA("./data/Centipede.tga", 4))
    {
        DWORD dwColorKey = pCentipedeImage->GetPixel(0, 0);
        m_pCentipedeImgData = new CImageData;
        m_pCentipedeImgData->Create(pCentipedeImage->GetRawImage(),
            pCentipedeImage->GetWidth(),
            pCentipedeImage->GetHeight(),
            dwColorKey);
    }
    delete pCentipedeImage;
    pCentipedeImage = nullptr;

    LOG_MESSAGE(L"Monster Image Loading Completed");

    LOG_MESSAGE(L"Portal Image Loading Start");

    // ��Ż �̹��� �ε�
    CTGAImage* pPortalImage = new CTGAImage;
    if (pPortalImage->Load24BitsTGA("./data/Portal_sprite.tga", 4))
    {
        DWORD dwColorKey = pPortalImage->GetPixel(0, 0);
        m_pPortalImgData = std::make_unique<CImageData>();
        m_pPortalImgData->Create(pPortalImage->GetRawImage(),
            pPortalImage->GetWidth(),
            pPortalImage->GetHeight(),
            dwColorKey);
    }
    delete pPortalImage;
    pPortalImage = nullptr;
    
    LOG_MESSAGE(L"Portal Image Loading Completed");



    // �÷��̾� �ʱ� ��ġ ����
    int playerPosX = 50;
    int playerPosY = 800;
    m_pPlayer = new Character(m_pPlayerImgData, static_cast<float>(playerPosX), static_cast<float>(playerPosY));

    // ���� ����
    m_vMonsters.push_back(new Centipede(m_pCentipedeImgData, 700.0f, 800.0f));

    m_pPortal = std::make_unique<Portal>(m_pPortalImgData.get(), 2500, 955);
    m_pPortal->SetNextSceneType(SceneType::Stage2);

    m_pDeathOverlapBox = std::make_unique<DeathOverlapBox>(-5000, 1500, 10000, 20);
    m_pDeathOverlapBox->SetNextSceneType(SceneType::GameOver);

}

void Stage1Scene::Cleanup()
{
    // Portal, Portal �̹����� unique_ptr�� �ڵ�����

    if (m_pPlayer)
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
    }


    for (auto monster : m_vMonsters)
    {
        if (monster)
        {
            delete monster;
        }
    }

    m_vMonsters.clear();


    if (m_pPlayerImgData)
    {
        delete m_pPlayerImgData;
        m_pPlayerImgData = nullptr;
    }

    if (m_pCentipedeImgData)
    {
        delete m_pCentipedeImgData;
        m_pCentipedeImgData = nullptr;
    }


    if (m_pBackgroundImage)
    {
        delete m_pBackgroundImage;
        m_pBackgroundImage = nullptr;
    }


    if (m_TileMap)
    {
        delete m_TileMap;
        m_TileMap = nullptr;
    }


    if (ColliderManager::GetInstance())
    {
        ColliderManager::GetInstance()->DestroyInstance();
    }

    if (DebugManager::GetInstance())
    {
        DebugManager::GetInstance()->ClearAllDebugData();
    }

}

void Stage1Scene::Update(float deltaTime)
{
    // ȭ�� ũ�Ⱑ ����� �� �����Ƿ� ���⼭ �ٽ� ȭ��ũ�⸦ �ٽ� ����������
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    Vector2<float> playerPos = Vector2<float>(0.0f, 0.0f);

    if (m_pPlayer)
    {
        m_pPlayer->Update(deltaTime);
        playerPos = m_pPlayer->GetPosition();
    }

    DebugManager::GetInstance()->AddOnScreenMessage(L"PlayerPos: " + std::to_wstring(static_cast<int>(playerPos.x)) +
        L", " + std::to_wstring(static_cast<int>(playerPos.y)), 0.03f);


    // ��� ���� ������Ʈ
    for (auto monster : m_vMonsters)
    {
        monster->Update(deltaTime);
    }

    if (m_TileMap)
    {
        m_TileMap->Update(deltaTime);
    }

    if (m_pPortal)
    {
        m_pPortal->Update(deltaTime);
    }

    UpdateCamera(screenWidth, screenHeight);        // UpdateCamera �Լ����� ī�޶� ��ġ�� ���� ��, �׿� ���� RenderPosition ������ ��

    ColliderManager::GetInstance()->ProcessCollisions();
    DebugManager::GetInstance()->Update(deltaTime);
    InputManager::GetInstance().Update(deltaTime);

}

void Stage1Scene::Render()
{
    // DirectDraw Device�� Bitmap �̹����� Render
    // Lock, UnLock�� ���� BackBuffer�� ���� �����Ͽ� �ȼ������� �ϳ��� �����Ͽ� Render
    m_pDrawDevice->BeginDraw();


    if (m_TileMap)
    {
        m_TileMap->Render(m_pDrawDevice);
    }

    if (m_pPortal)
    {
        m_pPortal->Render(m_pDrawDevice);
    }


    for (auto monster : m_vMonsters)
    {
        if (monster)
        {
            monster->Render(m_pDrawDevice);
        }
    }


    if (m_pPlayer)
    {
        m_pPlayer->Render(m_pDrawDevice);
    }




    m_pDrawDevice->EndDraw();


    // hDC�� ����� ���û��׵� ������
    // GDI(Device Context)�� ���� ���������� winAPI ������� ������ �ϱ�����
    HDC hDC = nullptr;

    // BackBuffer�� �׸��� ���� BeginGDI ����
    // BackBuffer�� �׷��� ȭ�� ������ ������ �� ����
    if (m_pDrawDevice->BeginGDI(&hDC))
    {
        m_pDrawDevice->DrawInfo(hDC);
        DebugManager::GetInstance()->Render(hDC);

        // ���Ϳ� ĳ���ʹ� ���� �� �� AABB Collider

        if (m_pPlayer)
        {
            float colliderLocalX = m_pPlayer->GetColliderLocalPosition().x;
            float colliderLocalY = m_pPlayer->GetColliderLocalPosition().y;

            m_pPlayer->GetCollider()->Render(hDC, static_cast<int>(m_playerRenderX + colliderLocalX), static_cast<int>(m_playerRenderY + colliderLocalY));
        }


        for (auto monster : m_vMonsters)
        {
            if (monster && monster->GetCollider())
            {
                // ������ ȭ��� ������ ��ġ�� ������
                Vector2<float> monsterColliderRenderPos = monster->GetRenderPosition() + monster->GetColliderLocalPosition();
                monster->GetCollider()->Render(hDC, static_cast<int>(monsterColliderRenderPos.x), static_cast<int>(monsterColliderRenderPos.y));
            }
        }

        if (m_TileMap)
        {
            const std::vector<Tile*>& blockLayer = m_TileMap->GetBlockLayer();
            for (Tile* tile : blockLayer)
            {
                if (tile && tile->GetCollider())
                {
                    Vector2<float> tileRenderPos = tile->GetCollider()->GetRenderPosition();
                    tile->GetCollider()->Render(hDC, static_cast<int>(tileRenderPos.x), static_cast<int>(tileRenderPos.y));
                }
            }
        }

        m_pDrawDevice->EndGDI(hDC);
    }

    m_pDrawDevice->OnDraw();
    m_pDrawDevice->CheckFPS();
}

void Stage1Scene::ResetInterpolation()
{
    if (m_pPlayer)
        m_pPlayer->ResetInterpolation();

    if (m_pPortal)
        m_pPortal->ResetInterpolation();

    for (auto monster : m_vMonsters)
    {
        if (monster)
            monster->ResetInterpolation();
    }
}

void Stage1Scene::InterpolatePosition(float alpha)
{
    if (m_pPlayer)
        m_pPlayer->UpdateInterpolation(alpha);

    if (m_pPortal)
        m_pPortal->UpdateInterpolation(alpha);

    for (auto monster : m_vMonsters)
    {
        if (monster)
            monster->UpdateInterpolation(alpha);
    }
}

const SceneType Stage1Scene::GetSceneType()
{
    return SceneType::Stage1;
}


void Stage1Scene::UpdateCamera(int screenWidth, int screenHeight)
{
    Vector2<float> playerPos = m_pPlayer->GetPosition();

    float screenCenterX = static_cast<float>(screenWidth) / 2;
    float screenCenterY = static_cast<float>(screenHeight) / 2;
    float cameraOffsetX = -200;
    float cameraOffsetY = 20;
    float cameraCenterX = screenCenterX + cameraOffsetX;
    float cameraCenterY = screenCenterY + cameraOffsetY;


    float computedOffsetX = playerPos.x - cameraCenterX;
    float computedOffsetY = playerPos.y - cameraCenterY;

    // ī�޶� ���ȭ�� �ٱ��� ����� ���ϵ��� clamp ó��
    float clampedOffsetX = std::max<float>(0, std::min<float>(computedOffsetX, static_cast<float>(m_bgWidth) - screenWidth));
    float clampedOffsetY = std::max<float>(0, std::min<float>(computedOffsetY, static_cast<float>(m_bgHeight) - screenHeight));

    m_backgroundPosX = -clampedOffsetX;
    m_backgroundPosY = -clampedOffsetY;

    // ī�޶� �� ���� �����Ͽ� �� �̻� �������� ���� ��, �÷��̾ �����̰� ó��
    m_playerRenderX = cameraCenterX - (clampedOffsetX - computedOffsetX);
    m_playerRenderY = cameraCenterY - (clampedOffsetY - computedOffsetY);

    m_pPlayer->SetRenderPosition(Vector2<float>{ static_cast<float>(m_playerRenderX), static_cast<float>(m_playerRenderY) });

    m_pPlayer->SetCameraOffset(Vector2<float>(-clampedOffsetX, -clampedOffsetY));
    DebugManager::GetInstance()->SetCameraOffset(Vector2<float>(-clampedOffsetX, -clampedOffsetY));

    for (auto monster : m_vMonsters)
    {
        // ������ ���� ��ǥ�� ��� �������� ���� ���� ������ ��ǥ ���
        Vector2<float> monsterWorldPos = monster->GetPosition();
        Vector2<float> monsterRenderPos = monsterWorldPos + Vector2<float>(m_backgroundPosX, m_backgroundPosY);

        monster->SetRenderPosition(monsterRenderPos);
    }


    DebugManager::GetInstance()->AddOnScreenMessage(L"Camera Offset (" + std::to_wstring(static_cast<int>(clampedOffsetX)) +
        L", " + std::to_wstring(static_cast<int>(clampedOffsetY)) + L")", 0.03f);


    // ���� TileMap�� Render�Լ����� CameraOffset�� ����Ͽ� Render ����
    m_TileMap->SetCameraOffset(clampedOffsetX, clampedOffsetY);


    const std::vector<Tile*>& blockLayer = m_TileMap->GetBlockLayer();
    for (Tile* tile : blockLayer)
    {
        if (tile && tile->GetCollider())
        {
            Vector2<float> colliderPos = tile->GetPosition();
            Vector2<float> newRenderPos = colliderPos + Vector2<float>({ -clampedOffsetX, -clampedOffsetY });

            tile->GetCollider()->SetRenderPosition(newRenderPos);
        }
    }

    if (m_pPortal)
    {
        m_pPortal->SetRenderPosition(m_pPortal->GetPosition() + Vector2<float>(m_backgroundPosX, m_backgroundPosY));
    }
}
