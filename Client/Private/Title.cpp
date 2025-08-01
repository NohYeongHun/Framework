﻿#include "Title.h"

CTitle::CTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject { pDevice, pContext }
{
}

CTitle::CTitle(const CTitle& Prototype)
    : CUIObject(Prototype )
{
}

HRESULT CTitle::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CTitle::Initialize_Clone(void* pArg)
{
    UIOBJECT_DESC               Desc{};
    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = 0;
    Desc.fSizeY = 0;
   
    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Childs()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;


    return S_OK;
}

void CTitle::Priority_Update(_float fTimeDelta)
{
    // 자식 객체들에게 모두 FadeOut 효과 부여 => Rendering Pass 변경.
    if (m_IsLogoFadeOut && m_fFadeTime >= 1.f)
    {
        m_pGameInstance->Publish<CLevel_Logo>(EventType::OPEN_GAMEPAY, nullptr);

        // Logo 에서 Loading에 호출해야함. => 이 이벤트가 아님.
        //m_pGameInstance->Publish<CLevel_Loading>(EventType::OPEN_LEVEL, nullptr);
        m_IsLogoFadeOut = false;

        return;
    }

    __super::Priority_Update(fTimeDelta);

    
}

void CTitle::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CTitle::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (m_fFadeTime < 1.f)
    {
        if (m_IsLogoFadeOut)
            m_fFadeTime += fTimeDelta;
    }
}

// Title 객체는 렌더링 안함.
HRESULT CTitle::Render()
{
    return S_OK;
}

void CTitle::Logo_End()
{
    m_IsLogoFadeOut = true;
    m_fFadeTime = { 0.f };

    // 자식 객체들에게 모두 FadeOut 효과 렌더링 패스 부여하고. 시간 부여.
    for (auto& pBackGround : m_TitleBackGruonds)
        pBackGround->Start_FadeOut();

    for (auto& pTitleText : m_TitleTexts)
        pTitleText->Start_FadeOut();    

}

HRESULT CTitle::Ready_Components()
{
    return S_OK;
}

HRESULT CTitle::Ready_Childs()
{
    m_TitleBackGruonds.resize(ENUM_CLASS(BackGround::BACKGROUND_END));

    // 0
    if (FAILED(Ready_Title_BackGround_Black()))
        return E_FAIL;

    // 1
    if (FAILED(Ready_Title_BackGround_White()))
        return E_FAIL;

    // 2.
    if (FAILED(Ready_Title_BackGround_Light()))
        return E_FAIL;
    
    // 3
    if (FAILED(Ready_Title_BackGround_Modify_Light()))
        return E_FAIL;

    // 4
    if (FAILED(Ready_Title_BackGround_White_Shade()))
        return E_FAIL;
    
    

    if (FAILED(Ready_Title_Text()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTitle::Ready_Events()
{
#pragma region FADE_OUT
    m_pGameInstance->Subscribe(EventType::LOGO_END, Get_ID(), [this](void* pData)
        {
            this->Logo_End();   // 멤버 함수 호출
        });

    // Event 목록 관리.
    m_Events.emplace_back(EventType::LOGO_END, Get_ID());

#pragma endregion

    return S_OK;
}


HRESULT CTitle::Ready_Title_BackGround_Black()
{
    CTitle_BackGround::TITLE_BAKCGROUND_DESC BackGroundDesc{};
    CUIObject* pUIObject = nullptr;

    BackGroundDesc.fX = 0;
    BackGroundDesc.fY = 0;
    BackGroundDesc.fSizeX = g_iWinSizeX;
    BackGroundDesc.fSizeY = g_iWinSizeY;

    BackGroundDesc.iTextureCount = 2;
    BackGroundDesc.iTexture = 0;
    BackGroundDesc.iPassIdx = 0;
    BackGroundDesc.fAlpha = 0.5f;
    BackGroundDesc.strObjTag = TEXT("BackGround Black");

    pUIObject = nullptr;
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_Title_BackGround"), &BackGroundDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_TitleBackGruonds[ENUM_CLASS(BackGround::BACKGROUND_BLACK)] = static_cast<CTitle_BackGround*>(pUIObject);

    return S_OK;
}

HRESULT CTitle::Ready_Title_BackGround_White()
{
    CTitle_BackGround::TITLE_BAKCGROUND_DESC BackGroundDesc{};
    CUIObject* pUIObject = nullptr;
    BackGroundDesc.fX = 0;
    BackGroundDesc.fY = 0;
    BackGroundDesc.fSizeX = g_iWinSizeX;
    BackGroundDesc.fSizeY = g_iWinSizeY;

    BackGroundDesc.iTextureCount = 2;
    BackGroundDesc.iTexture = 1;
    BackGroundDesc.iPassIdx = 4;
    BackGroundDesc.fAlpha = 0.21f;
    BackGroundDesc.strObjTag = TEXT("BackGround White");
    

    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_Title_BackGround"), &BackGroundDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_TitleBackGruonds[ENUM_CLASS(BackGround::BACKGROUND_WHITE)] = static_cast<CTitle_BackGround*>(pUIObject);


    return S_OK;
}

HRESULT CTitle::Ready_Title_BackGround_Modify_Light()
{
    CTitle_BackGround::TITLE_BAKCGROUND_DESC BackGroundDesc{};
    CUIObject* pUIObject = nullptr;
    BackGroundDesc.fX = 0;
    BackGroundDesc.fY = 0;
    BackGroundDesc.fSizeX = g_iWinSizeX;
    BackGroundDesc.fSizeY = g_iWinSizeY;

    BackGroundDesc.iTextureCount = 2;
    BackGroundDesc.iTexture = 2;
    BackGroundDesc.iPassIdx = 4;
    BackGroundDesc.fAlpha = 0.23f;
    BackGroundDesc.strObjTag = TEXT("BackGround Modify_Light");

    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_Title_BackGround"), &BackGroundDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_TitleBackGruonds[ENUM_CLASS(BackGround::BACKGROUND_WHITE_MODIFY_LIGHT)] = static_cast<CTitle_BackGround*>(pUIObject);
    return S_OK;
}

HRESULT CTitle::Ready_Title_BackGround_Light()
{
    CTitle_BackGround::TITLE_BAKCGROUND_DESC BackGroundDesc{};
    CUIObject* pUIObject = nullptr;

    BackGroundDesc.fX = 0;
    BackGroundDesc.fY = 0;
    BackGroundDesc.fSizeX = g_iWinSizeX;
    BackGroundDesc.fSizeY = g_iWinSizeY;

    BackGroundDesc.iTextureCount = 2;
    BackGroundDesc.iTexture = 3;
    BackGroundDesc.iPassIdx = 4;
    BackGroundDesc.fAlpha = 0.66f;
    BackGroundDesc.strObjTag = TEXT("BackGround Light");

    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_Title_BackGround"), &BackGroundDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_TitleBackGruonds[ENUM_CLASS(BackGround::BACKGROUND_WHITE_BASE_LIGHT)] = static_cast<CTitle_BackGround*>(pUIObject);

    return S_OK;
}

HRESULT CTitle::Ready_Title_BackGround_White_Shade()
{
    CTitle_BackGround::TITLE_BAKCGROUND_DESC BackGroundDesc{};
    CUIObject* pUIObject = nullptr;
    BackGroundDesc.fX = 0;
    BackGroundDesc.fY = 0;
    BackGroundDesc.fSizeX = g_iWinSizeX;
    BackGroundDesc.fSizeY = g_iWinSizeY;

    BackGroundDesc.iTextureCount = 2;
    BackGroundDesc.iTexture = 4;
    BackGroundDesc.iPassIdx = 4;
    BackGroundDesc.fAlpha = 0.89f;
    BackGroundDesc.strObjTag = TEXT("BackGround White Shade");


    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_Title_BackGround"), &BackGroundDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_TitleBackGruonds[ENUM_CLASS(BackGround::BACKGROUND_WHITE_SHADE)] = static_cast<CTitle_BackGround*>(pUIObject);

    return S_OK;
}

HRESULT CTitle::Ready_Title_Text()
{
    m_TitleTexts.resize(2);

    CTitleText::TITLETEXT_DESC TitleTextDesc{};
    TitleTextDesc.fX = 0;
    TitleTextDesc.fY = 0;
    TitleTextDesc.fSizeX = 1280;
    TitleTextDesc.fSizeY = 280;


    CUIObject* pUIObject = nullptr;
    TitleTextDesc.iTextureIndex = 0;
    TitleTextDesc.fY = 100;
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_Title_Text"), &TitleTextDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_TitleTexts[ENUM_CLASS(Text::TEXT)] = static_cast<CTitleText*>(pUIObject);


    pUIObject = nullptr;
    TitleTextDesc.iTextureIndex = 3;
    TitleTextDesc.fY = 150;
    pUIObject = dynamic_cast<CUIObject*>(
        m_pGameInstance->Clone_Prototype(
            PROTOTYPE::GAMEOBJECT
            , ENUM_CLASS(LEVEL::LOGO)
            , TEXT("Prototype_GameObject_Title_Text"), &TitleTextDesc));

    if (nullptr == pUIObject)
        return E_FAIL;

    AddChild(pUIObject);
    m_TitleTexts[ENUM_CLASS(Text::ENUM_LINE)] = static_cast<CTitleText*>(pUIObject);

    return S_OK;
}


CTitle* CTitle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTitle* pInstance = new CTitle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTitle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTitle::Clone(void* pArg)
{
    CTitle* pInstance = new CTitle(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTitle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTitle::Destroy()
{
    __super::Destroy();

    for (auto& Event : m_Events)
        m_pGameInstance->UnSubscribe(Event.first, Event.second);
}

void CTitle::Free()
{
    __super::Free();
    m_TitleTexts.clear();
    

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);


}
