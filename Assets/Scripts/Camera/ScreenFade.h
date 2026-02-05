#pragma once

#include "Runtime/Scripting/IScript.h"
#include "Runtime/Scripting/ScriptReflection.h"

namespace Alice
{
    // 간단한 예제 스크립트입니다. 필요에 맞게 수정해서 사용하세요.
    class ScreenFade : public IScript
    {
        ALICE_BODY(ScreenFade);

    public:
        void Start() override;
        void Update(float deltaTime) override;
        void SetOnFade(bool _onfade) { OnFade = _onfade; }
        bool GetOnFade() { return OnFade; }
        // --- 변수 리플렉션 예시 (에디터에서 수정 가능) ---
        ALICE_PROPERTY(bool, m_primary, false);
        ALICE_PROPERTY(bool, OnFade, false);
        ALICE_PROPERTY(std::string, m_FadeObjectName, "");
    };
}
