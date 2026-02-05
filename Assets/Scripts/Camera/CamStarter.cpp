#include "CamStarter.h"
#include "Runtime/Scripting/ScriptFactory.h"
#include "Runtime/Foundation/Logger.h"
#include "Runtime/ECS/World.h"
#include "Runtime/ECS/GameObject.h"
#include "Runtime/Rendering/Components/CameraComponent.h"
#include "../Physics/Gimmick.h"


namespace Alice
{
    // 이 스크립트를 리플렉션/팩토리 시스템에 등록합니다.
    REGISTER_SCRIPT(CamStarter);

    void CamStarter::Start()
    {
        // 초기화 로직을 여기에 작성하세요.
        auto* world = GetWorld();
        if (!world) return;

		auto go = world->FindGameObject(m_startCameraName);
		if (go.IsValid() && go.GetComponent<CameraComponent>())
		{
			go.GetComponent<CameraComponent>()->primary = true;
		}

        // Gimmick 스크립트 미리 찾아서 캐시 (성능 최적화)
        m_cachedGimmick = nullptr;
        m_hasCalledSetBreak = false;

        // Camera2의 초기 primary 상태 저장
        auto camera2Go = world->FindGameObject("Camera2");
        if (camera2Go.IsValid())
        {
            auto* cam = camera2Go.GetComponent<CameraComponent>();
            if (cam)
            {
                m_prevCamera2Primary = cam->primary;
            }
        }

        // Gimmick GameObject 이름이 지정된 경우
        if (!m_gimmickGameObjectName.empty())
        {
            auto gimmickGo = world->FindGameObject(m_gimmickGameObjectName);
            if (gimmickGo.IsValid())
            {
                m_cachedGimmick = gimmickGo.GetComponent<Gimmick>();
            }
        }
        else
        {
            // 이름이 지정되지 않은 경우, 모든 엔티티에서 Gimmick 찾기
            const auto& allScripts = world->GetAllScriptsInWorld();
            for (const auto& [entityId, scripts] : allScripts)
            {
                for (const auto& scriptComp : scripts)
                {
                    if (scriptComp.instance)
                    {
                        Gimmick* gimmick = dynamic_cast<Gimmick*>(scriptComp.instance.get());
                        if (gimmick)
                        {
                            m_cachedGimmick = gimmick;
                            break;
                        }
                    }
                }
                if (m_cachedGimmick) break;
            }
        }

        if (m_cachedGimmick)
        {
            ALICE_LOG_INFO("[CamStarter] Gimmick script found and cached");
        }
        else
        {
            ALICE_LOG_WARN("[CamStarter] Gimmick script not found");
        }
    }

    void CamStarter::Update(float deltaTime)
    {
        // 이미 SetBreak를 호출했다면 리턴
        if (m_hasCalledSetBreak) return;

        auto* world = GetWorld();
        if (!world) return;

        auto go = world->FindGameObject("Camera2");
        if (!go.IsValid()) return;

        auto* cam = go.GetComponent<CameraComponent>();
        if (!cam) return;

        // Camera2의 현재 primary 상태
        bool currentPrimary = cam->primary;

        // 이전 상태가 true였고 현재 상태가 false로 변경되었을 때만 SetBreak 호출
        if (m_prevCamera2Primary && !currentPrimary)
        {
            // 캐시된 Gimmick이 있으면 사용
            if (m_cachedGimmick)
            {
                m_cachedGimmick->SetBreak();
                m_hasCalledSetBreak = true;
                ALICE_LOG_INFO("[CamStarter] Camera2 primary changed from true to false, Called Gimmick::SetBreak()");
            }
            else
            {
                // 캐시가 없으면 다시 찾기 (Start에서 찾지 못한 경우)
                const auto& allScripts = world->GetAllScriptsInWorld();
                for (const auto& [entityId, scripts] : allScripts)
                {
                    for (const auto& scriptComp : scripts)
                    {
                        if (scriptComp.instance)
                        {
                            Gimmick* gimmick = dynamic_cast<Gimmick*>(scriptComp.instance.get());
                            if (gimmick)
                            {
                                gimmick->SetBreak();
                                m_cachedGimmick = gimmick;
                                m_hasCalledSetBreak = true;
                                ALICE_LOG_INFO("[CamStarter] Camera2 primary changed from true to false, Called Gimmick::SetBreak()");
                                break;
                            }
                        }
                    }
                    if (m_hasCalledSetBreak) break;
                }
            }
        }

        // 현재 상태를 이전 상태로 저장 (다음 프레임을 위해)
        m_prevCamera2Primary = currentPrimary;
    }
}
