#include "CameraPathController.h"
#include "FreeLookCameraController.h"
#include "Runtime/Scripting/ScriptFactory.h"
#include "Runtime/ECS/GameObject.h"
#include "Runtime/ECS/Components/TransformComponent.h"
#include "Runtime/Rendering/Components/CameraComponent.h"
#include "Runtime/Foundation/Logger.h"
#include <DirectXMath.h>
#include <algorithm>

namespace Alice
{
    REGISTER_SCRIPT(CameraPathController);

    void CameraPathController::Awake()
    {
        // 초기화
        m_prevSetStartButton = false;
        m_prevSetEndButton = false;
        m_isEnabled = false;
        m_isMoving = false;
        m_moveElapsed = 0.0f;
		gameObject().GetComponent<CameraComponent>()->primary = false;

    }

    void CameraPathController::OnEnable()
    {
        m_isEnabled = true;
        m_isMoving = true;
        m_moveElapsed = 0.0f;
        
        // Start 위치와 회전을 시작점으로 설정
        m_startMovePosition = m_startPosition;
        m_startMoveRotation = m_startRotation;
        
        // 현재 위치를 Start 위치로 즉시 이동
        auto go = gameObject();
        if (go.IsValid())
        {
            auto* transform = go.GetComponent<TransformComponent>();
            if (transform)
            {
                transform->position = m_startPosition;
                transform->rotation = m_startRotation;
            }
        }
    }

    void CameraPathController::OnDisable()
    {
        m_isEnabled = false;
        m_isMoving = false;
    }

    void CameraPathController::Start()
    {
        // OnEnable에서 초기화됨
    }

    void CameraPathController::Update(float deltaTime)
    {
        auto go = gameObject();
        if (!go.IsValid()) return;
        auto* transform = go.GetComponent<TransformComponent>();
        if (!transform) return;
        auto* cam = go.GetComponent<CameraComponent>();
        if (!cam->GetPrimary()) return;

        // Set Start Position 버튼 처리
        // Inspector에서 체크박스를 체크하면 (false -> true) 메서드 호출
        if (m_setStartPositionButton && !m_prevSetStartButton)
        {
            SetStartPosition();
            // 버튼 상태를 즉시 false로 리셋하여 다음 클릭을 감지할 수 있게 함
            m_setStartPositionButton = false;
        }
        m_prevSetStartButton = m_setStartPositionButton;

        // Set End Position 버튼 처리
        if (m_setEndPositionButton && !m_prevSetEndButton)
        {
            SetEndPosition();
            // 버튼 상태를 즉시 false로 리셋하여 다음 클릭을 감지할 수 있게 함
            m_setEndPositionButton = false;
        }
        m_prevSetEndButton = m_setEndPositionButton;

        // Enable 상태일 때 Start -> End로 부드럽게 이동
        if (m_isEnabled && m_isMoving && m_moveDuration > 0.0f)
        {
            m_moveElapsed += deltaTime;
            float t = std::min(m_moveElapsed / m_moveDuration, 1.0f);

            // SmoothStep을 사용하여 부드러운 보간
            float smoothT = t * t * (3.0f - 2.0f * t);

            // 위치 보간
            DirectX::XMVECTOR startPosVec = DirectX::XMLoadFloat3(&m_startMovePosition);
            DirectX::XMVECTOR endPosVec = DirectX::XMLoadFloat3(&m_endPosition);
            DirectX::XMVECTOR currentPosVec = DirectX::XMVectorLerp(startPosVec, endPosVec, smoothT);
            DirectX::XMStoreFloat3(&transform->position, currentPosVec);

            // 회전 보간 (쿼터니언으로 변환하여 보간)
            DirectX::XMVECTOR startRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(
                m_startMoveRotation.x,  // roll (rotation.z)
                m_startMoveRotation.y,   // pitch (rotation.x)
                m_startMoveRotation.z);  // yaw (rotation.y)
            DirectX::XMVECTOR endRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(
                m_endRotation.x,  // roll (rotation.z)
                m_endRotation.y,   // pitch (rotation.x)
                m_endRotation.z);  // yaw (rotation.y)
            DirectX::XMVECTOR currentRotQuat = DirectX::XMQuaternionSlerp(startRotQuat, endRotQuat, smoothT);

            // 쿼터니언을 오일러 각으로 변환 (TransformComponent의 SetRotation 사용)
            // SetRotation은 쿼터니언(XMFLOAT4)을 받아서 내부적으로 (pitch, yaw, roll) 오일러로 변환
            DirectX::XMFLOAT4 quatFloat4;
            DirectX::XMStoreFloat4(&quatFloat4, currentRotQuat);
            transform->SetRotation(quatFloat4);
            
            // 디버깅 로그
            ALICE_LOG_INFO("[CameraPathController] Interpolation - smoothT: %f, Quat: (%.4f, %.4f, %.4f, %.4f), Result Rotation: (%.4f, %.4f, %.4f)",
                smoothT, quatFloat4.x, quatFloat4.y, quatFloat4.z, quatFloat4.w,
                transform->rotation.x, transform->rotation.y, transform->rotation.z);
            // 이동 완료
            if (t >= 1.0f)
            {
                m_isMoving = false;
                transform->position = m_endPosition;
                transform->rotation = m_endRotation;
                ALICE_LOG_INFO("[CameraPathController] Movement completed - Final Position: (%.4f, %.4f, %.4f), Final Rotation: (%.4f, %.4f, %.4f)",
                    transform->position.x, transform->position.y, transform->position.z,
                    transform->rotation.x, transform->rotation.y, transform->rotation.z);

                auto* camera = gameObject().GetComponent<CameraComponent>();
                camera->primary = false;
                
                auto go = GetWorld()->FindGameObject(m_nextCameraName);
                if(go.GetComponent<CameraComponent>())
                {
                    go.GetComponent<CameraComponent>()->primary = true;
                }
            }
        }
    }

    void CameraPathController::SetStartPosition()
    {
        auto go = gameObject();
        if (!go.IsValid())
        {
            ALICE_LOG_WARN("[CameraPathController] GameObject is invalid");
            return;
        }

        auto* transform = go.GetComponent<TransformComponent>();
        if (!transform)
        {
            ALICE_LOG_WARN("[CameraPathController] TransformComponent not found");
            return;
        }

        // 현재 카메라 위치와 회전을 시작 위치/회전으로 설정
        m_startPosition = transform->position;
        m_startRotation = transform->rotation;
        ALICE_LOG_INFO("[CameraPathController] Start Position set to: (%f, %f, %f)",
            m_startPosition.x, m_startPosition.y, m_startPosition.z);
        ALICE_LOG_INFO("[CameraPathController] Start Rotation set to: (%f, %f, %f)",
            (m_startRotation.x), m_startRotation.y, m_startRotation.z);
    }

    void CameraPathController::SetEndPosition()
    {
        auto go = gameObject();
        if (!go.IsValid())
        {
            ALICE_LOG_WARN("[CameraPathController] GameObject is invalid");
            return;
        }

        auto* transform = go.GetComponent<TransformComponent>();
        if (!transform)
        {
            ALICE_LOG_WARN("[CameraPathController] TransformComponent not found");
            return;
        }

        // 현재 카메라 위치와 회전을 끝 위치/회전으로 설정
        m_endPosition = transform->position;
        m_endRotation = transform->rotation;
        ALICE_LOG_INFO("[CameraPathController] End Position set to: (%f, %f, %f)",
            m_endPosition.x, m_endPosition.y, m_endPosition.z);
        ALICE_LOG_INFO("[CameraPathController] End Rotation set to: (%f, %f, %f)",
            m_endRotation.x, m_endRotation.y, m_endRotation.z);
    }
}
