﻿#include "GameObject.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_strObjTag { Prototype.m_strObjTag }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent* CGameObject::Get_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize_Clone(void* pArg)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize_Clone(pArg)))
		return E_FAIL;	

	m_Components.emplace(TEXT("Com_Transform"), m_pTransformCom);		

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{

	m_pTransformCom->Update_WorldMatrix();
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::On_Collision_Enter(CGameObject* pOther)
{

}

void CGameObject::On_Collision_Stay(CGameObject* pOther)
{

}

void CGameObject::On_Collision_Exit(CGameObject* pOther)
{

}

const _bool CGameObject::Is_Ray_LocalHit(_float3* pOutLocalPos, _float3* pOutLocalNormal, _float* pOutDist)
{
	return false;
}



HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Get_Component(strComponentTag))
		return E_FAIL;

	CComponent*		pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

// 여기서 바인딩한 Component에 AddRef 설정.
HRESULT CGameObject::Change_Component(const _wstring& strComponentTag, CComponent** ppOut, CComponent* pChangeComponent)
{
	CComponent* pComponent = Get_Component(strComponentTag);

	if (nullptr != pComponent)
	{
		// 기존 컴포넌트를 지우기?
		Safe_Release(pComponent);
		m_Components.erase(strComponentTag);
	}

	m_Components.emplace(strComponentTag, pChangeComponent);

	*ppOut = pChangeComponent;
	Safe_AddRef(pChangeComponent);
	

	return S_OK;
}

/* 이동량 만큼을 더해줍니다. */
void CGameObject::Translate(_fvector vTranslate)
{
	if (nullptr == m_pTransformCom)
	{
		CRASH("Transform Component Not Exist");
		return;
	}
	
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	vPos += vTranslate;
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

const _wstring& CGameObject::Get_ObjectTag()
{
	return m_strObjTag;
}



void CGameObject::Destroy()
{
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
