#include "SceneChange.h"
#include "Runtime/Scripting/ScriptFactory.h"
#include "Runtime/Foundation/Logger.h"
#include "Runtime/ECS/World.h"
#include "Runtime/ECS/GameObject.h"
//#include "Runtime/ECS//*GameObject*/.h"
#include "Runtime/Rendering/Components/CameraComponent.h"
namespace Alice
{
    // 이 스크립트를 리플렉션/팩토리 시스템에 등록합니다.
    REGISTER_SCRIPT(SceneChange);

    void SceneChange::Start()
    {
        // 초기화 로직을 여기에 작성하세요.
    }

    void SceneChange::Update(float deltaTime)
    {
        auto go = GetWorld()->FindGameObject(m_chaneTrigger);
        auto* cam = go.GetComponent<CameraComponent>();
        if (cam->GetPrimary())
        {
            Scenes()->LoadSceneFileRequest(m_nextScenePath.c_str());
        }
    }
}
