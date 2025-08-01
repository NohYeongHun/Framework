﻿#pragma once
#include "PlayerState.h"


NS_BEGIN(Client)
/* 28. Animation Name: SK_HumanFemale_LOD0.ao|AS_TStdUnisex_TLSword_Guard_N_End
29. Animation Name: SK_HumanFemale_LOD0.ao|AS_TStdUnisex_TLSword_Guard_N_Loop
30. Animation Name: SK_HumanFemale_LOD0.ao|AS_TStdUnisex_TLSword_Guard_N_Start*/
/* index : 28 End, 29 Loop, 30 Start, 31 Hit*/
class CPlayer_GuardState final : public CPlayerState
{
public:


public:
	typedef struct tagPlayerGuardEnterDesc
	{
		_uint iAnimation_Idx = {};
	}GUARD_ENTER_DESC;

public:
	typedef struct tagPlayerGuardStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}GUARDSTATE_DESC;

private:
	explicit CPlayer_GuardState();
	virtual ~CPlayer_GuardState() = default;

public:
	// 시작 초기화
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;

	// State 시작 시.
	virtual void Enter(void* pArg) override;

	// State 실행 로직
	virtual void Update(_float fTimeDelta) override;

	// State Exit
	virtual void Exit() override;

	// State 초기값으로 설정
	virtual void Reset() override;

	void Change_State(_float fTimeDelta);
	

private:
	ACTORDIR			   m_eDir = { ACTORDIR::END };
	

public:
	static CPlayer_GuardState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
