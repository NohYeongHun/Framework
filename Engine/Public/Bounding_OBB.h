﻿#pragma once
#include "Bounding.h"

NS_BEGIN(Engine)
class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUNDING_OBB_DESC;

	typedef struct tagOBBDesc
	{
		_float3		vCenterDir[3];
		_float3		vAlignDir[3];
	}OBB_DESC;


private:
	explicit CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	const BoundingOrientedBox* Get_Desc() const {
		return m_pDesc;
	}

	virtual const _float3 Get_WorldCenter() override;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);
	//virtual HRESULT Render(PrimitiveBatch)

public:
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pBounding) override;
	virtual void Change_BoundingDesc(BOUNDING_DESC* pBoundingDesc) override;
	virtual void Reset_Bounding() override;


private:
	BoundingOrientedBox* m_pLocalDesc = { nullptr };
	BoundingOrientedBox* m_pDesc = { nullptr };

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free();
};
NS_END

