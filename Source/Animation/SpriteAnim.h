#pragma once
#include <vector>
#include <algorithm>

// Template Ŭ������ ������Ͼȿ� ���Ǹ� ��
template <typename AnimState>
class SpriteAnim {
public:
    // Sprite �ִϸ��̼ǿ����� Frame�� SpriteSheet �迭�� Index�� ����

    struct AnimSequence {
        AnimState state;                // �ִϸ��̼� ���� (enum)
        int startFrame;                 // ���� ������
        int endFrame;                   // ���� ������
        float frameDuration;            // �� ������ ���� �ð� (ms ����)
        bool loop = true;              // true: ����, false: ������ �����ӿ� ����
    };


    SpriteAnim()
        : m_currentState(), m_currentFrame(0), m_timer(0.0f)
    {
    }

    ~SpriteAnim() {}

    // ��ü �ִϸ��̼� ������ ���� ����
    void SetSequences(const std::vector<AnimSequence>& sequences) {
        m_sequences = sequences;
    }

    // ���� �ִϸ��̼� ���� ���� (���� ��ȯ �� �����Ӱ� Ÿ�̸� ����)
    void SetState(AnimState state) {
        if (m_currentState != state) {
            m_currentState = state;
            for (const auto& seq : m_sequences) {
                if (seq.state == state) {
                    m_currentFrame = seq.startFrame;
                    m_timer = 0.0f;
                    break;
                }
            }
        }
    }


    // �־��� �ִϸ��̼� ���¿� �ش��ϴ� AnimSequence�� �˻��Ͽ� ��ȯ (������ nullptr ��ȯ)
    const AnimSequence* GetSequence(AnimState state) const {
        for (const auto& seq : m_sequences) {
            if (seq.state == state)
                return &seq;
        }
        return nullptr;
    }


    // deltaTime: ms ������ ���� (Ȥ�� ms�� ��ȯ�Ͽ� ���)
    void Update(float deltaTime) {

        // currentState�� �ش��ϴ� Sequence ã��
        const AnimSequence* currentSeq = nullptr;
        for (const auto& seq : m_sequences) {
            if (seq.state == m_currentState) {
                currentSeq = &seq;
                break;
            }
        }

        if (currentSeq != nullptr) {
            m_timer += deltaTime;

            // deltaTIme�� �ſ� ū ���, while���� ���� ���� �� ó���ϴ� ��
            while (m_timer >= currentSeq->frameDuration) {
                m_timer -= currentSeq->frameDuration;
                m_currentFrame++;
                if (currentSeq->loop) {
                    // ������ ���, �� �������� ������ ���� ����������
                    if (m_currentFrame > currentSeq->endFrame)
                        m_currentFrame = currentSeq->startFrame;
                }
                else {
                    // ������ �ƴ� ���, �� �����ӿ� ����
                    if (m_currentFrame > currentSeq->endFrame) {
                        m_currentFrame = currentSeq->endFrame;
                        break;
                    }
                }
            }
        }
    }

    // ���� �ִϸ��̼� ������ �ε��� ��ȯ
    int GetCurrentFrame() const { return m_currentFrame; }

    // ���� �ִϸ��̼� ���� ��ȯ
    AnimState GetState() const { return m_currentState; }

private:
    std::vector<AnimSequence> m_sequences;  // ��ü �ִϸ��̼� ������ ����
    AnimState m_currentState;                       // ���� �ִϸ��̼� ����
    int m_currentFrame;                                // ���� ������ �ε���
    float m_timer;                                         // ���� Ÿ�̸� (ms ����)
};
