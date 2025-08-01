﻿
#pragma once

// ============ BehaviourTree_Component.h ============
NS_BEGIN(Engine)

class ENGINE_DLL CBehaviorTree abstract : public CBase
{
public:
    typedef struct tagBTDesc
    {
        
    } BT_DESC;

protected:
    CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBehaviorTree(const CBehaviorTree& Prototype);
    virtual ~CBehaviorTree() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Update(_float fTimeDelta);

public:
    void Set_Root_Node(CBTNode* pRootNode) {
        if (m_pRootNode) {
            Safe_Release(m_pRootNode);
        }
        m_pRootNode = pRootNode;
        if (m_pRootNode) {
            Safe_AddRef(m_pRootNode);
        }
    }

    CBTNode* Get_Root_Node() const { return m_pRootNode; }



protected:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };
    CBTNode* m_pRootNode = nullptr;
    

public:
    //static CBehaviorTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    //virtual CComponent* Clone(void* pArg) PURE;
    virtual void Free() override;
};

NS_END