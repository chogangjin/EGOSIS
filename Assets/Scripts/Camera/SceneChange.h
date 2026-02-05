#pragma once

#include "Runtime/Scripting/IScript.h"
#include "Runtime/Scripting/ScriptReflection.h"

namespace Alice
{
    // 간단한 예제 스크립트입니다. 필요에 맞게 수정해서 사용하세요.
    class SceneChange : public IScript
    {
        ALICE_BODY(SceneChange);

    public:
        void Start() override;
        void Update(float deltaTime) override;

        // --- 변수 리플렉션 예시 (에디터에서 수정 가능) ---
        ALICE_PROPERTY(std::string, m_chaneTrigger, "");
        ALICE_PROPERTY(std::string,  m_nextScenePath, "");
        // --- 함수 리플렉션 예시 ---
    };
}
