#include "FreeLookCameraController.h"
#include "Runtime/Scripting/ScriptFactory.h"
#include "Runtime/ECS/GameObject.h"
#include "Runtime/Input/Input.h"
#include "Runtime/ECS/Components/TransformComponent.h"
#include "Runtime/Foundation/Logger.h"
#include "Runtime/Rendering/Components/CameraComponent.h"
#include <DirectXMath.h>
#include <algorithm>

using namespace DirectX;

namespace Alice
{
    REGISTER_SCRIPT(FreeLookCameraController);

    namespace
    {
        // 쿼터니언을 오일러 각으로 변환하는 헬퍼 함수
        DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMFLOAT4& quat)
        {
            const float x = quat.x;
            const float y = quat.y;
            const float z = quat.z;
            const float w = quat.w;

            // Roll (X축 회전)
            const float sinr_cosp = 2.0f * (w * x + y * z);
            const float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
            const float roll = std::atan2(sinr_cosp, cosr_cosp);

            // Pitch (Y축 회전)
            const float sinp = 2.0f * (w * y - z * x);
            float pitch;
            if (std::abs(sinp) >= 1.0f)
                pitch = std::copysign(DirectX::XM_PIDIV2, sinp);
            else
                pitch = std::asin(sinp);

            // Yaw (Z축 회전)
            const float siny_cosp = 2.0f * (w * z + x * y);
            const float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
            const float yaw = std::atan2(siny_cosp, cosy_cosp);

            return DirectX::XMFLOAT3(pitch, yaw, roll);
        }
    }

    void FreeLookCameraController::Awake()
    {
        // 시작 시 현재 각도를 가져와서 초기화 (카메라가 튀는 것 방지)
        auto go = gameObject();
        if (!go.IsValid()) return;

        auto* tr = go.GetComponent<TransformComponent>();
        if (!tr) return;

        // Transform의 rotation은 오일러 각(라디안)으로 저장되어 있음
        m_pitch = tr->rotation.x;
        m_yaw = tr->rotation.y;

        // Start -> End 이동 상태 초기화
        m_isMovingOnPath = false;
        m_pathMoveElapsed = 0.0f;
    }

    void FreeLookCameraController::Start()
    {
        // Play 모드 시작 시 Start 위치로 이동 시작
        auto go = gameObject();
        if (!go.IsValid()) return;

        auto* transform = go.GetComponent<TransformComponent>();
        if (!transform) return;

        // Start 위치와 회전으로 즉시 이동
        m_isMovingOnPath = true;
        m_pathMoveElapsed = 0.0f;

        ALICE_LOG_INFO("[FreeLookCameraController] Starting movement from Start to End position/rotation");
    }

    void FreeLookCameraController::Update(float deltaTime)
    {
        auto go = gameObject();
        auto* input = Input();
        if (!go.IsValid() || !input) return;

        auto* tr = go.GetComponent<TransformComponent>();
        if (!tr) return;
        auto* cm = go.GetComponent<CameraComponent>();
        if (!cm->primary) return;


        // 1. 마우스 회전 (우클릭 상태일 때만)
        if (input->GetMouseButton(MouseCode::Right))
        {
            float dx = input->GetMouseDeltaX();
            float dy = input->GetMouseDeltaY();

            m_yaw += dx * m_mouseSensitivity;
            m_pitch += dy * m_mouseSensitivity;

            // 고개 너무 젖혀짐 방지 (-89도 ~ 89도)
            const float pitchLimit = XMConvertToRadians(89.0f);
            m_pitch = std::clamp(m_pitch, -pitchLimit, pitchLimit);
        }

        // 회전 적용 (오일러 각, 라디안)
        tr->rotation = DirectX::XMFLOAT3(m_pitch, m_yaw, 0.0f);

        // 2. 키보드 이동 (WASD + QE)
        float moveX = 0.0f;  // 좌우 (A/D)
        float moveY = 0.0f;  // 상하 (Q/E)
        float moveZ = 0.0f;  // 앞뒤 (W/S)

        if (input->GetKey(KeyCode::W)) moveZ += 1.0f;  // 앞
        if (input->GetKey(KeyCode::S)) moveZ -= 1.0f;  // 뒤
        if (input->GetKey(KeyCode::D)) moveX += 1.0f;  // 우
        if (input->GetKey(KeyCode::A)) moveX -= 1.0f;  // 좌
        if (input->GetKey(KeyCode::Q)) moveY += 1.0f;  // 위 (카메라 Up 방향)
        if (input->GetKey(KeyCode::E)) moveY -= 1.0f;  // 아래 (카메라 -Up 방향)

        // 입력이 없으면 리턴 (연산 절약)
        if (moveX == 0.0f && moveY == 0.0f && moveZ == 0.0f) return;

        // 3. 카메라가 바라보는 방향 기준으로 이동 벡터 계산
        // 회전 행렬 생성 (Pitch, Yaw만 사용)
        XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f);

        // 카메라의 로컬 축 추출
        XMVECTOR right = rotationMatrix.r[0];   // X축 (우측)
        XMVECTOR up = rotationMatrix.r[1];     // Y축 (위)
        XMVECTOR forward = rotationMatrix.r[2]; // Z축 (앞)

        // 이동 방향 벡터 합성 (카메라 기준)
        XMVECTOR moveDir = (right * moveX) + (up * moveY) + (forward * moveZ);

        // 정규화 (대각선 이동 시 속도 일정하게)
        float length = XMVectorGetX(XMVector3Length(moveDir));
        if (length > 0.0001f)
        {
            moveDir = XMVector3Normalize(moveDir);
        }
        else
        {
            moveDir = XMVectorZero();
        }

        // 4. 위치 적용
        XMVECTOR currentPos = XMLoadFloat3(&tr->position);
        XMVECTOR newPos = currentPos + (moveDir * m_moveSpeed * deltaTime);
        XMStoreFloat3(&tr->position, newPos);
    }
}
