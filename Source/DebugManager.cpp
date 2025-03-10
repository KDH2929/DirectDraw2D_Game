#include "stdafx.h"
#include "DebugManager.h"
#include <direct.h>     // _mkdir
#include <time.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

// �ʱ� ���� ������ ����
DebugManager* DebugManager::s_instance = nullptr;


// ������: �α� ���� �Ҵ� �� ǥ�� ��� �����̷�Ʈ (�ɼ�)
// Log ���丮�� ���� ��¥/�ð��� �̸����� �� �ؽ�Ʈ ���Ϸ� �����̷�Ʈ
DebugManager::DebugManager() {
    // �ܼ� ��� �ɼ� (�ʿ�� ���, �ּ� ó����)
    /*
    AllocConsole();
    FILE* fpOut = nullptr;
    if (freopen_s(&fpOut, "CONOUT$", "w", stdout) == 0) {
        //_setmode(_fileno(stdout), _O_U16TEXT);
    }
    */

    // "Log" ���丮 ���� (�������� ������)
    const char* logDir = "Log";
    _mkdir(logDir); // Windows���� ���丮 ����

    // ���� ��¥�� �ð��� ������� ���� �̸� ����
    time_t now = time(NULL);
    struct tm localTime;
    localtime_s(&localTime, &now); // thread-safe ����
    // localtime �Լ��� ���������� ���� �޸𸮸� ����Ͽ� ���� �����尡 �ش� ���� �޸�(�޸� �ڿ�)�� ������ �� ����
    // localtime_s�� ȣ����(caller)�� tm ����ü�� ���� �����ϹǷ�, ���������� ���� �޸𸮸� ������� �ʾ� thread-safe
    char fileName[256];
    // ��: Log/Log_2023-04-15_12-30-45.txt
    sprintf_s(fileName, sizeof(fileName), "%s/Log_%04d-%02d-%02d_%02d-%02d-%02d.txt",
        logDir,
        localTime.tm_year + 1900,
        localTime.tm_mon + 1,
        localTime.tm_mday,
        localTime.tm_hour,
        localTime.tm_min,
        localTime.tm_sec);

    // ǥ�� ����� ������ �α� ���Ϸ� �����̷�Ʈ
    FILE* fpOut = nullptr;
    if (freopen_s(&fpOut, fileName, "w", stdout) == 0) {
        // ���Ͽ� ���� ��쿡�� _setmode(_fileno(stdout), _O_U16TEXT)�� ȣ������ �ʴ� ���� ����
    }   
}


DebugManager::~DebugManager() {
    FreeConsole();
}

DebugManager* DebugManager::GetInstance()
{
    if (!s_instance) {
        s_instance = new DebugManager();
    }
    return s_instance;
}

void DebugManager::DestroyInstance()
{
    delete s_instance;
    s_instance = nullptr;
}

// ȭ�鿡 �޽����� ǥ���ϴ� �Լ�
void DebugManager::AddOnScreenMessage(const std::wstring& msg, float duration) {
    DebugMessage newMsg;
    newMsg.text = msg;
    newMsg.duration = duration * 1000.0f; // �� -> �и���
    newMsg.elapsed = 0.0f;
    m_messages.push_back(newMsg);
}

// �ܼ�/����� ��¿� �޽����� ����ϴ� �Լ�
void DebugManager::LogMessage(const std::wstring& msg) {
    std::lock_guard<std::mutex> lock(logMutex);  // �� ���� �ϳ��� �����常 ����

    OutputDebugStringW(msg.c_str());
    OutputDebugStringW(L"\n");

    std::wcout << msg << std::endl;

    // std::lock_guard<std::mutex>�� �Լ��� ���� �� �ڵ����� mutex.unlock()�� ȣ��
}

void DebugManager::SetCameraOffset(Vector2<float> cameraOffset)
{
    m_cameraOffset = cameraOffset;
}

void DebugManager::AddDebugBox(const DebugBox& box)
{
    m_debugBoxes.push_back(box);
}

void DebugManager::AddDebugLine(const DebugLine& line)
{
    m_debugLines.push_back(line);
}

void DebugManager::ClearAllDebugData()
{
    m_messages.clear();
    m_debugBoxes.clear();
    m_debugLines.clear();
}

void DebugManager::Update(float deltaTime) {
    // �޽��� ������Ʈ
    for (auto it = m_messages.begin(); it != m_messages.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_messages.erase(it);
        else
            ++it;
    }

    // ����� �ڽ� ������Ʈ
    for (auto it = m_debugBoxes.begin(); it != m_debugBoxes.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_debugBoxes.erase(it);
        else
            ++it;
    }

    // ����� �� ������Ʈ
    for (auto it = m_debugLines.begin(); it != m_debugLines.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_debugLines.erase(it);
        else
            ++it;
    }
}

void DebugManager::Render(HDC hDC) {
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0));

    int posY = 10;
    // �޽��� ������
    for (const auto& msg : m_messages) {
        RECT rect = { 10, posY, 500, posY + 20 };
        DrawTextW(hDC, msg.text.c_str(), -1, &rect, DT_LEFT | DT_SINGLELINE);
        posY += 25;
    }

    // ����� �ڽ� ������
    // �ڽ��� ȸ������ �����ϱ� ���ؼ���, �� �ڽ��� �������� ����ϰ� Polygon���� �׸�
    for (const auto& box : m_debugBoxes) {

        Vector2<float> renderBoxCenter = box.center + m_cameraOffset;

        // �ڽ��� �� ũ��
        float halfW = box.width * 0.5f;
        float halfH = box.height * 0.5f;
        float rad = box.rotation * PI / 180.0f;
        // ȸ���� ���� ��ǥ (�ð� ���� ����)
        Vector2<float> axisX(std::cos(rad), std::sin(rad));
        Vector2<float> axisY(-std::sin(rad), std::cos(rad));

        // �� ������ ���: center �� axisX*halfW �� axisY*halfH
        Vector2<float> corners[4];
        corners[0] = renderBoxCenter + axisX * halfW + axisY * halfH;
        corners[1] = renderBoxCenter - axisX * halfW + axisY * halfH;
        corners[2] = renderBoxCenter - axisX * halfW - axisY * halfH;
        corners[3] = renderBoxCenter + axisX * halfW - axisY * halfH;

        // GDI�� Polyline �Լ��� �ڽ��� �׵θ� �׸���
        POINT points[5];
        for (int i = 0; i < 4; ++i) {
            points[i].x = static_cast<LONG>(corners[i].x);
            points[i].y = static_cast<LONG>(corners[i].y);
        }
        points[4] = points[0];  // ���� Polygon�� ���� ���������� ����

        // ���� ����
        HPEN hPen = CreatePen(PS_SOLID, 2, box.color);
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

        Polyline(hDC, points, 5);

        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }


    // ����� �� ������
    for (const auto& line : m_debugLines) {
        // ī�޶� ������ ����
        Vector2<float> renderStart = line.start + m_cameraOffset;
        Vector2<float> renderEnd = line.end + m_cameraOffset;

        HPEN hPen = CreatePen(PS_SOLID, line.thickness, line.color);
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

        MoveToEx(hDC, static_cast<int>(renderStart.x), static_cast<int>(renderStart.y), NULL);
        LineTo(hDC, static_cast<int>(renderEnd.x), static_cast<int>(renderEnd.y));

        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }
}
