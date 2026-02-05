#pragma once

#include "Runtime/Scripting/IScript.h"
#include "Runtime/Scripting/ScriptReflection.h"
//#include "../Physics/Gimmick.h"

namespace Alice
{
    class Gimmick;
    // 간단한 예제 스크립트입니다. 필요에 맞게 수정해서 사용하세요.
    class CamStarter : public IScript
    {
        ALICE_BODY(CamStarter);

    public:
        void Start() override;
        void Update(float deltaTime) override;

		ALICE_PROPERTY(std::string, m_startCameraName, std::string(""));
        ALICE_PROPERTY(std::string, m_gimmickGameObjectName, std::string("")); // Gimmick이 붙어있는 GameObject 이름 (선택사항)

    private:
        bool m_hasCalledSetBreak = false; // SetBreak가 이미 호출되었는지 추적
        Gimmick* m_cachedGimmick = nullptr; // 캐시된 Gimmick 포인터 (성능 최적화)
        bool m_prevCamera2Primary = true; // Camera2의 이전 primary 상태 (true -> false 변화 감지용)
    };
}
