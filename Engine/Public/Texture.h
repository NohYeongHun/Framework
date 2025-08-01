﻿#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	HRESULT Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);

private:
	vector<ID3D11ShaderResourceView*> m_SRVs = {};
	_uint m_iNumTextures = {};

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual CTexture* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

