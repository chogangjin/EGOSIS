#pragma once

#include "Runtime/Scripting/IScript.h"
#include "Runtime/Scripting/ScriptReflection.h"

namespace Alice
{
    // 카메라의 primary가 false가 되면 UI 컴포넌트에 fade out을 적용하는 스크립트
    class CameraFadeOutController : public IScript
    {
        ALICE_BODY(CameraFadeOutController);

    public:
        void Start() override;
        void Update(float deltaTime) override;

        // Inspector에서 설정 가능한 속성들
        // 감시할 카메라의 이름
        ALICE_PROPERTY(std::string, m_cameraName, std::string(""));

    private:
        bool m_prevCameraPrimary = true; // 카메라의 이전 primary 상태 (true -> false 변화 감지용)
        bool m_hasTriggeredFadeOut = false; // 이미 fade out을 트리거했는지 추적
        
        // 현재 스크립트가 붙어있는 GameObject에 UIAnimationComponent를 추가하고 fade out을 트리거하는 함수
        void TriggerFadeOut();
    };
}
