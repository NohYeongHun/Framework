﻿#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize_Clone() override;
	virtual void Update(_float fTimeDelta) override;
	
	virtual HRESULT Render() override;

public:
	void Open_Level();

private:
	_bool m_IsLogoEnd = { false };
	vector<pair<EventType, uint32_t>> m_Events = {};

private:
	HRESULT Ready_Layer_Title(const _wstring& strLayerTag);
	HRESULT Ready_Events();


public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END