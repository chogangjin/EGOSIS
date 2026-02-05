#pragma once

#include <DirectXMath.h>
#include "Runtime/Scripting/IScript.h"
#include "Runtime/Scripting/ScriptReflection.h"

namespace Alice
{
    /// 카메라의 시작 위치와 끝 위치를 저장하고 제어하는 스크립트
    /// Editor 모드에서 현재 카메라 위치를 설정할 수 있습니다.
    /// Play 모드에서 Start 위치부터 End 위치까지 부드럽게 이동합니다.
    class CameraPathController : public IScript
    {
        ALICE_BODY(CameraPathController);

    public:
        void Awake() override;
        void OnEnable() override;
        void OnDisable() override;
        void Start() override;
        void Update(float deltaTime) override;

        /// 시작 위치를 현재 카메라 위치로 설정
        void SetStartPosition();
        ALICE_FUNC(SetStartPosition);

        /// 끝 위치를 현재 카메라 위치로 설정
        void SetEndPosition();
        ALICE_FUNC(SetEndPosition);

    private:
        // 시작 위치와 끝 위치 (에디터에서 설정 가능)
        ALICE_PROPERTY(DirectX::XMFLOAT3, m_startPosition, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
        ALICE_PROPERTY(DirectX::XMFLOAT3, m_endPosition, DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f));

        // 시작 회전과 끝 회전 (오일러 각, 라디안, 에디터에서 설정 가능)
        ALICE_PROPERTY(DirectX::XMFLOAT3, m_startRotation, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
        ALICE_PROPERTY(DirectX::XMFLOAT3, m_endRotation, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

        // 이동 속도 및 시간
        ALICE_PROPERTY(float, m_moveDuration, 5.0f);  // Start -> End 이동 시간 (초)

        // 버튼 트리거용 bool 프로퍼티 (Inspector에서 체크박스로 표시되지만 버튼처럼 사용)
        ALICE_PROPERTY(bool, m_setStartPositionButton, false);
        ALICE_PROPERTY(bool, m_setEndPositionButton, false);

		ALICE_PROPERTY(std::string, m_nextCameraName, std::string(""));


        // 이전 버튼 상태 추적 (false -> true 변화 감지)
        bool m_prevSetStartButton = false;
        bool m_prevSetEndButton = false;

        // 이동 상태
        bool m_isEnabled = false;
        bool m_isMoving = false;
        float m_moveElapsed = 0.0f;
        DirectX::XMFLOAT3 m_startMovePosition{};
        DirectX::XMFLOAT3 m_startMoveRotation{};
    };
}
