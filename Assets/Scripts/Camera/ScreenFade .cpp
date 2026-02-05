#include "ScreenFade.h"
#include "Runtime/Scripting/ScriptFactory.h"
#include "Runtime/Foundation/Logger.h"
#include "Runtime/ECS/World.h"
#include "Runtime/ECS/GameObject.h"
//#include "FadeOutController.h"
#include "Runtime/Rendering/Components/CameraComponent.h"

namespace Alice
{
    REGISTER_SCRIPT(ScreenFade);

    void ScreenFade::Start()
    {
		auto go = GetWorld()->FindGameObject(m_FadeObjectName);
        m_primary = go.GetComponent<CameraComponent>()->primary;
    }

    void ScreenFade::Update(float deltaTime)
    {
  //      auto go = GetWorld()->FindGameObject(m_FadeObjectName);
  //      if (go.GetComponent<CameraComponent>()->primary != m_primary)
		//{
		//	//auto* fo = go.GetComponent<FadeOutController>();
		//	auto* fo = go.GetComponent<FadeOutController>();
		//	fo->StartFadeOut();
		//}
    }
  //  void ScreenFade::OnOffFade()
  //  {
		//auto go = GetWorld()->FindGameObject(m_FadeObjectName);
		//auto* fo = go.GetComponent<FadeOutController>();
		//if (OnFade)
  //      {
		//	fo->StartFadeOut();
  //      }
  //      else
  //      {
  //          //fo->ResetFade();
  //      }
  //  }
}
