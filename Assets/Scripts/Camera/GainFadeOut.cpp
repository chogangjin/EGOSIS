#include "GainFadeOut.h"
#include "ScreenFade.h"
#include "Runtime/Scripting/ScriptFactory.h"
#include "Runtime/Foundation/Logger.h"
#include "Runtime/ECS/World.h"
#include "Runtime/ECS/GameObject.h"
#include "Runtime/Rendering/Components/CameraComponent.h"

namespace Alice
{
    // 이 스크립트를 리플렉션/팩토리 시스템에 등록합니다.
    REGISTER_SCRIPT(GainFadeOut);

    ScreenFade* GainFadeOut::FindScreenFade()
    {
        auto* world = GetWorld();
        if (!world) return nullptr;

        // GameObject 이름으로 찾기
        if (!m_screenFadeGameObjectName.empty())
        {
            auto screenFadeGo = world->FindGameObject(m_screenFadeGameObjectName);
            if (screenFadeGo.IsValid())
            {
                ScreenFade* screenFade = screenFadeGo.GetComponent<ScreenFade>();
                if (screenFade)
                {
                    return screenFade;
                }
            }
        }
        else
        {
            // 이름이 지정되지 않은 경우, 모든 엔티티에서 ScreenFade 찾기
            const auto& allScripts = world->GetAllScriptsInWorld();
            for (const auto& [entityId, scripts] : allScripts)
            {
                for (const auto& scriptComp : scripts)
                {
                    if (scriptComp.instance)
                    {
                        ScreenFade* screenFade = dynamic_cast<ScreenFade*>(scriptComp.instance.get());
                        if (screenFade)
                        {
                            return screenFade;
                        }
                    }
                }
            }
        }

        return nullptr;
    }

    void GainFadeOut::Start()
    {
        m_cachedScreenFade = nullptr;
        m_hasProcessedCamera3 = false;
        m_screenFadeSearchCooldown = 0.0f;

        // ScreenFade 스크립트 찾기
        m_cachedScreenFade = FindScreenFade();

        if (m_cachedScreenFade)
        {
            ALICE_LOG_INFO("[GainFadeOut] ScreenFade script found and cached");
        }
        else
        {
            ALICE_LOG_WARN("[GainFadeOut] ScreenFade script not found (will retry later)");
        }
    }

    void GainFadeOut::Update(float deltaTime)
    {
        auto* world = GetWorld();
        if (!world) return;

        // ScreenFade가 없거나 무효화되었을 수 있으므로 주기적으로 재검색
        m_screenFadeSearchCooldown -= deltaTime;
        if (m_screenFadeSearchCooldown <= 0.0f)
        {
            // 캐시가 없거나 유효하지 않으면 다시 찾기
            if (!m_cachedScreenFade)
            {
                m_cachedScreenFade = FindScreenFade();
                if (m_cachedScreenFade)
                {
                    ALICE_LOG_INFO("[GainFadeOut] ScreenFade found and cached");
                }
            }
            m_screenFadeSearchCooldown = 1.0f; // 1초마다 재검색
        }

        // 이미 처리했다면 리턴
        if (m_hasProcessedCamera3) return;

        // Camera3 찾기
        auto camera3Go = world->FindGameObject("Camera3");
        if (!camera3Go.IsValid()) return;

        auto* cam = camera3Go.GetComponent<CameraComponent>();
        if (!cam) return;

        // Camera3의 primary가 false가 되었을 때
        if (!cam->primary)
        {
            // ScreenFade가 없으면 다시 찾기 시도
            if (!m_cachedScreenFade)
            {
                m_cachedScreenFade = FindScreenFade();
            }

            if (m_cachedScreenFade)
            {
                // ScreenFade의 OnFade를 켜기
                m_cachedScreenFade->SetOnFade(true);
                ALICE_LOG_INFO("[GainFadeOut] Camera3 primary became false, ScreenFade OnFade set to true");
                m_hasProcessedCamera3 = true;
            }
            else
            {
                ALICE_LOG_WARN("[GainFadeOut] Camera3 primary became false, but ScreenFade not found");
            }
        }
    }

    void GainFadeOut::FadeOut()
    {
        // ScreenFade가 없으면 다시 찾기 시도
        if (!m_cachedScreenFade)
        {
            m_cachedScreenFade = FindScreenFade();
        }

        if (m_cachedScreenFade)
        {
			m_cachedScreenFade->SetOnFade(true);
            ALICE_LOG_INFO("[GainFadeOut] FadeOut() called, ScreenFade OnFade set to true");
        }
        else
        {
            ALICE_LOG_WARN("[GainFadeOut] FadeOut() called, but ScreenFade not found");
        }
    }

    void GainFadeOut::TurnOnScreenFade()
    {
        // ScreenFade가 없으면 다시 찾기 시도
        if (!m_cachedScreenFade)
        {
            m_cachedScreenFade = FindScreenFade();
        }

        if (m_cachedScreenFade)
        {
            m_cachedScreenFade->SetOnFade(true) ;
            ALICE_LOG_INFO("[GainFadeOut] TurnOnScreenFade() called, ScreenFade OnFade set to true");
        }
        else
        {
            ALICE_LOG_WARN("[GainFadeOut] TurnOnScreenFade() called, but ScreenFade not found");
        }
    }

    void GainFadeOut::TurnOffScreenFade()
    {
        // ScreenFade가 없으면 다시 찾기 시도
        if (!m_cachedScreenFade)
        {
            m_cachedScreenFade = FindScreenFade();
        }

        if (m_cachedScreenFade)
        {
            m_cachedScreenFade->SetOnFade(false);
            ALICE_LOG_INFO("[GainFadeOut] TurnOffScreenFade() called, ScreenFade OnFade set to false");
        }
        else
        {
            ALICE_LOG_WARN("[GainFadeOut] TurnOffScreenFade() called, but ScreenFade not found");
        }
    }
}
