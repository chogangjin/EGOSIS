#pragma once

#include <DirectXMath.h>
#include "Runtime/Scripting/IScript.h"
#include "Runtime/Scripting/ScriptReflection.h"

namespace Alice
{
    /// 자유 시점 카메라 컨트롤러
    /// - 우클릭으로 카메라 회전
    /// - W, A, S, D로 카메라가 바라보는 방향 기준 이동
    /// - Q, E로 카메라가 바라보는 기준 Up/-Up 방향 이동
    /// - Start -> End 위치로 부드럽게 이동하는 기능 포함
    class FreeLookCameraController : public IScript
    {
        ALICE_BODY(FreeLookCameraController);

    public:
        void Awake() override;
        void Start() override;
        void Update(float deltaTime) override;

        // 에디터에서 조절 가능한 속성
        ALICE_PROPERTY(float, m_moveSpeed, 10.0f);        // 이동 속도
        ALICE_PROPERTY(float, m_mouseSensitivity, 0.001f); // 마우스 감도 (회전)

    private:
        float m_yaw = 0.0f;     // 가로 회전 (Y축, 라디안)
        float m_pitch = 0.0f;   // 세로 회전 (X축, 라디안)

        // Start -> End 이동 상태
        bool m_isMovingOnPath = false;
        float m_pathMoveElapsed = 0.0f;
        DirectX::XMFLOAT3 m_pathStartPosition{};
        DirectX::XMFLOAT3 m_pathStartRotation{};
    };
}
