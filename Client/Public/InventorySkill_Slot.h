﻿#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*
* 스킬 슬롯 객체는 스킬 아이콘 객체를 소유하게.
*/
class CInventorySkill_Slot final : public CUIObject
{
public:
	typedef struct tagSkillSlotDesc : CUIObject::UIOBJECT_DESC
	{
		_uint iTextureIndex = {};
		_uint iPanelType = {};
		_uint iPanelIndex = {};
		_uint iSlotIndex = {};
	}SKILLSLOT_DESC;

private:
	CInventorySkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventorySkill_Slot(const CInventorySkill_Slot& Prototype);
	virtual ~CInventorySkill_Slot() = default;

public:
	void Set_Visibility();

public:
	void Change_Skill(class CSkillUI_Icon* pSkillIcon, _uint iTextureIndex);
	

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Ready_Childs();

private:
	class CInventorySkill_Icon* m_pSkill = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint m_iPanelType = {};
	_uint m_iTextureIndex = {};
	_uint m_iPanelIndex = {};
	_uint m_iSlotIndex = {};

	_bool m_IsVisibility = {};

private:
	HRESULT Ready_Components(SKILLSLOT_DESC* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CInventorySkill_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy() override;
	virtual void Free() override;

};
NS_END

