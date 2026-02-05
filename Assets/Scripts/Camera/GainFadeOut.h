#pragma once

#include "Runtime/Scripting/IScript.h"
#include "Runtime/Scripting/ScriptReflection.h"

namespace Alice
{
    class ScreenFade;

    // 간단한 예제 스크립트입니다. 필요에 맞게 수정해서 사용하세요.
    class GainFadeOut : public IScript
    {
        ALICE_BODY(GainFadeOut);

    public:
        void Start() override;
        void Update(float deltaTime) override;

        // --- 변수 리플렉션 예시 (에디터에서 수정 가능) ---
        ALICE_PROPERTY(bool, m_prevFinisthed, false)
        ALICE_PROPERTY(bool, m_currFinished, false);
        
        // ScreenFade GameObject 이름 (Inspector에서 설정)
        ALICE_PROPERTY(std::string, m_screenFadeGameObjectName, std::string(""));
        
        // --- 함수 리플렉션 예시 ---
        void FadeOut();
        ALICE_FUNC(FadeOut);
        
        // ScreenFade 제어 함수
        void TurnOnScreenFade();
        ALICE_FUNC(TurnOnScreenFade);
        
        void TurnOffScreenFade();
        ALICE_FUNC(TurnOffScreenFade);

    private:
        // ScreenFade를 찾는 헬퍼 함수
        ScreenFade* FindScreenFade();

        ScreenFade* m_cachedScreenFade = nullptr; // 캐시된 ScreenFade 포인터
        bool m_hasProcessedCamera3 = false; // Camera3 처리가 완료되었는지 추적
        float m_screenFadeSearchCooldown = 0.0f; // ScreenFade 재검색 쿨다운 (성능 최적화)
    };
}
