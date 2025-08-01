﻿#include "PlayerState.h"

HRESULT CPlayerState::Initialize(_uint iStateNum, void* pArg)
{
    PLAYER_STATE_DESC* pDesc = static_cast<PLAYER_STATE_DESC*>(pArg);
    m_pFsm = pDesc->pFsm;
    m_pPlayer = dynamic_cast<CPlayer*>(pDesc->pOwner);
    m_pModelCom = dynamic_cast<CLoad_Model*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
    m_iStateNum = iStateNum;
    if (nullptr == m_pModelCom)
    {
        CRASH("Create Failed Player State");
        return E_FAIL;
    }

    if (nullptr == m_pPlayer)
    {
        CRASH("Create Failed Player State");
        return E_FAIL;
    }
    
    return S_OK;
}

void CPlayerState::Handle_Input()
{
    m_eDir = m_pPlayer->Get_Direction();
    m_KeyInput = m_pPlayer->Get_KeyInput();

    if (m_pPlayer->Is_MovementKeyPressed())
        m_eDir = m_pPlayer->Calculate_Direction();
    else
        m_eDir = ACTORDIR::END;
}

void CPlayerState::Enter(void* pArg)
{
    m_pFsm->Set_StateCoolTime(m_iStateNum);
    m_pFsm->Set_StateExitCoolTime(m_iStateNum);
}

_vector CPlayerState::Calculate_Unified_Attack_Direction()
{
    // 1. 키 입력 방향 계산 (카메라 기준)
    _vector vInputDirection = XMVectorZero();
    if (m_pPlayer->Is_MovementKeyPressed())
    {
        ACTORDIR eInputDir = m_pPlayer->Calculate_Direction();
        if (eInputDir != ACTORDIR::END)
        {
            vInputDirection = m_pPlayer->Calculate_Move_Direction(eInputDir);
        }
    }

    // 2. LockOn 타겟 방향 계산
    _vector vLockOnDirection = XMVectorZero();
    if (m_pPlayer->Has_LockOn_Target())
    {
        vLockOnDirection = m_pPlayer->Get_LockOn_Attack_Direction();
    }

    // 3. 우선순위에 따른 최종 방향 결정
    _vector vFinalDirection = Determine_Final_Direction(vInputDirection, vLockOnDirection);

    return vFinalDirection;
}

_vector CPlayerState::Determine_Final_Direction(_vector vInputDirection, _vector vLockOnDirection)
{
    // 키 입력이 있으면 항상 키 입력 우선 (플레이어 의도 존중)
    if (!XMVector3Equal(vInputDirection, XMVectorZero()))
    {
        // LockOn 중이라도 키 입력 방향을 사용 (스트레이핑 효과)
        return vInputDirection;
    }

    // 키 입력이 없고 LockOn 타겟이 있으면 타겟 방향
    if (!XMVector3Equal(vLockOnDirection, XMVectorZero()))
    {
        return vLockOnDirection;
    }

    // 둘 다 없으면 플레이어의 현재 Look 방향
    _vector vPlayerLook = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
    vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
    return XMVector3Normalize(vPlayerLook);
}

void CPlayerState::Handle_Attack_Movement(_vector vDirection, _float fTimeDelta)
{
    if (XMVector3Equal(vDirection, XMVectorZero()))
        return;

    // LockOn 상태와 키 입력에 따른 이동 속도 조정
    _float fMoveSpeed = Calculate_Attack_Move_Speed();

    // 방향으로 이동
    m_pPlayer->Get_Transform()->Move_Direction(vDirection, fTimeDelta * fMoveSpeed);
}

_float CPlayerState::Calculate_Attack_Move_Speed()
{
    _float fBaseSpeed = 0.4f; // 기본 공격 중 이동 속도

    // LockOn 중이고 키 입력이 있으면 스트레이핑 속도 (약간 느리게)
    if (m_pPlayer->Has_LockOn_Target() && m_pPlayer->Is_MovementKeyPressed())
    {
        return fBaseSpeed * 0.7f; // 스트레이핑은 더 느리게
    }

    // LockOn 중이고 키 입력이 없으면 전진 속도
    if (m_pPlayer->Has_LockOn_Target())
    {
        return fBaseSpeed * 0.5f; // 타겟으로 살짝 전진
    }

    return fBaseSpeed;
}

void CPlayerState::Handle_Unified_Direction_Input(_float fTimeDelta)
{
    m_fCurrentLockTime += fTimeDelta;

    // 방향 고정 시간 체크
    if (m_fCurrentLockTime >= m_fDirectionLockTime)
    {
        m_bIsDirectionLocked = true;
        m_bCanChangeDirection = false;
    }

    // 방향이 고정되기 전에만 방향 처리
    if (!m_bIsDirectionLocked)
    {
        // ⭐ 부모 클래스의 통합된 방향 계산 함수 사용
        _vector vFinalDirection = Calculate_Unified_Attack_Direction();

        if (!XMVector3Equal(vFinalDirection, XMVectorZero()))
        {
            // 플레이어를 최종 방향으로 회전
            Rotate_Player_To_Direction(vFinalDirection, fTimeDelta);

            // ⭐ 부모 클래스의 통합된 이동 처리 함수 사용
            Handle_Attack_Movement(vFinalDirection, fTimeDelta);
        }
    }
}

void CPlayerState::Rotate_Player_To_Direction(_vector vTargetDirection, _float fTimeDelta)
{
    if (XMVector3Equal(vTargetDirection, XMVectorZero()))
        return;

    // 목표 방향 계산
    _float x = XMVectorGetX(vTargetDirection);
    _float z = XMVectorGetZ(vTargetDirection);
    _float fTargetYaw = atan2f(x, z);

    // 현재 회전 상태
    _float fCurrentYaw = m_pPlayer->Get_Transform()->GetYawFromQuaternion();
    _float fYawDiff = fTargetYaw - fCurrentYaw;

    // 최단 경로 계산
    while (fYawDiff > XM_PI) fYawDiff -= XM_2PI;
    while (fYawDiff < -XM_PI) fYawDiff += XM_2PI;

    // ⭐ LockOn 상태에 따른 적응형 회전 속도
    _float fRotationSpeed = Get_Adaptive_Rotation_Speed();
    _float fMaxRotation = fRotationSpeed * fTimeDelta;

    if (fabsf(fYawDiff) > fMaxRotation)
    {
        fYawDiff = (fYawDiff > 0) ? fMaxRotation : -fMaxRotation;
    }

    // 새로운 회전 적용
    _float fNewYaw = fCurrentYaw + fYawDiff;
    _vector qNewRot = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fNewYaw);
    m_pPlayer->Get_Transform()->Set_Quaternion(qNewRot);
}

_float CPlayerState::Get_Adaptive_Rotation_Speed()
{
    _float fBaseRotationSpeed = 12.0f; // 기본 공격 회전 속도

    if (m_pPlayer->Has_LockOn_Target())
    {
        // LockOn 중일 때는 더 빠른 회전 (타겟 지향성 강화)
        if (m_pPlayer->Is_MovementKeyPressed())
        {
            return fBaseRotationSpeed * 1.2f; // 스트레이핑 시 약간 빠르게
        }
        else
        {
            return fBaseRotationSpeed * 1.5f; // 타겟 방향으로 빠르게 회전
        }
    }
    else
    {
        // 일반 상태에서는 기본 속도
        return fBaseRotationSpeed;
    }
}


void CPlayerState::Free()
{
    __super::Free();
    m_pPlayer = nullptr;
    m_pModelCom = nullptr;
    
}
