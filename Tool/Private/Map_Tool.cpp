﻿#include "Map_Tool.h"


CMap_Tool::CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pDeviceContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pDevice);
}


/* 맵툴이 카메라 소유시키기 .*/
HRESULT CMap_Tool::Initialize(LEVEL eLevel)
{
    m_eCurLevel = eLevel;

    /* 카메라 초기화 */
    list<CGameObject*> cameraList = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Camera"))
        ->Get_GameObjects();

    auto iter = cameraList.begin();

    if (iter == cameraList.end())
        return E_FAIL;

    m_pCamera = static_cast<CCamera_Free*>(*iter);
    Safe_AddRef(m_pCamera);

    /* SaveFile Loader 초기화 */
    m_pSaveFile_Loader = CSaveFile_Loader::Create();
    m_pCameraTransformCom = static_cast<CTransform*>(m_pCamera->Get_Component(L"Com_Transform"));
    
    if (FAILED(Ready_Imgui()))
        return E_FAIL;


    return S_OK;
}


void CMap_Tool::Change_SelectObject(CGameObject* pSelectedObject)
{
    m_pSelectedObject = pSelectedObject;
}

void CMap_Tool::Update(_float fTimeDelta)
{
   /* if (m_pGameInstance->Get_KeyUp(DIK_C))
        m_eToolMode = TOOLMODE::CREATE;

    if (m_pGameInstance->Get_KeyUp(DIK_E))
        m_eToolMode = TOOLMODE::EDIT;*/


    if (m_IsPossible_Picking && m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::RB))
        Update_Picking(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Map_Parts"));

    Handle_SelectedObject();
}

struct LevelButton {
    const char* label;
    const _wstring vibuffer_type;
};

void CMap_Tool::Render()
{
    
    //Default_Render();

    if (m_eToolMode == TOOLMODE::CREATE)
        Render_Model_Create();
    else
        Render_Model_Edit();

    // Render_SaveLoad 렌더링.
    Render_SaveLoad();

    // 디버그 정보는 항상 렌더링
    Render_Debug_Window();

}

void CMap_Tool::Render_SaveLoad()
{
    if (!m_IsPossible_SaveLoad)
        return;

    ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("open"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../../SaveFile/Model/";
                config.flags = ImGuiFileDialogFlags_ReadOnlyFileNameField;

                // 파일 다이얼로그 열기
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".dat", config);
            }
            if (ImGui::MenuItem("save"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../../SaveFile/Model/";
                config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;

                ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Choose File", ".dat", config);

            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // 읽기용 로직
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string load_path = ImGuiFileDialog::Instance()->GetFilePathName();

            /* Save 타입에 따라 저장 방식이 달라집니다. */
            if (m_eSaveMode == SAVEMODE::MAP_OBJECT)
                m_pSaveFile_Loader->Load_MapFile(load_path, m_eCurLevel);
            else if (m_eSaveMode == SAVEMODE::MODEL_COMPONENT)
                m_pSaveFile_Loader->Load_ModelFile(load_path, m_eCurLevel);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // 저장용 로직
    if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string save_path = ImGuiFileDialog::Instance()->GetFilePathName();

            if (m_eSaveMode == SAVEMODE::MODEL_COMPONENT)
                m_pSaveFile_Loader->Save_ModelFile(save_path, m_wSelected_PrototypeModelTag);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::End();
}





/* Debug 모드에서 현재 상태값에 대한 지정을 수행합니다. */
void CMap_Tool::Render_Debug_Window()
{
    ImGuiIO& io = ImGui::GetIO();

    // 윈도우 하단에서 150 → 200~250 정도 위로
    ImVec2 windowPos = ImVec2(10.f, io.DisplaySize.y - 350.f);
    ImVec2 windowSize = ImVec2(300.f, 500.f);

    // Cond_Once: 최초 한 번만 위치 적용 → 이후 드래그 가능
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    // NoCollapse만 유지, 이동 가능하게
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    
    _float3 camPos = {};
    XMStoreFloat3(&camPos,m_pCameraTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);

    const char* modeStr = (m_eToolMode == TOOLMODE::CREATE) ? "CREATE" : "EDIT";
    ImGui::Text("Tool Mode: %s", modeStr);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

   

    // Selected 되어있을 때만.
    if (m_pSelectedObject)
    {
        _float3 pickingWorld = {};
        XMStoreFloat3(&pickingWorld, XMVector3TransformCoord(
            XMLoadFloat3(&m_RayHitDesc.vHitLocal), m_pSelectedObject->Get_Transform()->Get_WorldMatrix()));

        ImGui::Text("Picking Local: (%.2f, %.2f, %.2f)"
            , m_RayHitDesc.vHitLocal.x, m_RayHitDesc.vHitLocal.y, m_RayHitDesc.vHitLocal.z);
        ImGui::Text("Picking World: (%.2f, %.2f, %.2f)"
            , pickingWorld.x, pickingWorld.y, pickingWorld.z);
    }
    
    
    
    // 체크박스 추가 - 피킹 가능 여부
    ImGui::Checkbox("Enable Picking", &m_IsPossible_Picking);
    // 체크박스 추가 - 파일 Load Save 여부
    ImGui::Checkbox("Enable SaveLoad", &m_IsPossible_SaveLoad);



    if (m_eToolMode == TOOLMODE::CREATE)
        ImGui::Text("Select Model : %s", m_Selected_PrototypeModelTag.c_str());
    else if(m_eToolMode == TOOLMODE::EDIT)
        ImGui::Text("Select Object : %s", m_Selected_EditObjTag.c_str());

    const char* items[] = { "Map", "Model" };

    // enum -> int
    int iCurMode = static_cast<int>(m_eSaveMode);
    ImGui::Combo("Save Type", static_cast<int*>(&iCurMode), items, IM_ARRAYSIZE(items));
        m_eSaveMode = static_cast<SAVEMODE>(iCurMode); // int → enum
    
    if (ImGui::Button("Edit Mode"))
    {
        m_eToolMode = TOOLMODE::EDIT;
        // 현재 생성된 Layer 정보를 Load합니다.
        Load_Layer();
    }
    
    if (ImGui::Button("Create Mode"))
    {
        m_eToolMode = TOOLMODE::CREATE;
    }


    ImGui::End();
}

void CMap_Tool::Handle_SelectedObject()
{
    if (nullptr == m_pSelectedObject)
        return;

    if (m_eToolMode == TOOLMODE::CREATE)
        Handle_CreateMode_SelectedObject();
    else if (m_eToolMode == TOOLMODE::EDIT)
        Handle_EditMode_SelectedObject();
}   




#pragma region CREATE_MODE
void CMap_Tool::Render_Model_Create()
{
    if (m_IsPossible_SaveLoad)
        return;

    Render_Prototype_Hierarchy();
}


void CMap_Tool::Render_Prototype_Hierarchy()
{
    ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX - 310.f, 10.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
    ImGui::Begin("Prototype_Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse);

    static int iSelectedIndex = -1;
    _uint id = 0;

    _wstring objTag = {};
    _wstring modelTag = {};
    ImVec2 hierarchyPos = ImGui::GetWindowPos();
    ImVec2 hierarchySize = ImGui::GetWindowSize();
    

    for (auto& pair : m_PrototypeNames)
    {
        if (ImGui::Selectable(pair.second.c_str(), id == iSelectedIndex))
        {
            m_PrototypeinspectorPos = ImVec2(hierarchyPos.x - 310.f, hierarchyPos.y); // 왼쪽 붙이기

            iSelectedIndex = id;
            m_wSelected_PrototypeObjTag = _wstring(pair.first.begin(), pair.first.end());
            m_wSelected_PrototypeModelTag = _wstring(pair.second.begin(), pair.second.end());

            m_Selected_PrototypeModelTag = pair.second;
            m_Selected_PrototypeObjTag = pair.first;
        }
        
    }

    if (iSelectedIndex >= 0 && iSelectedIndex < m_PrototypeNames.size())
        Render_Prototype_Inspector(m_PrototypeinspectorPos);

    ImGui::End();
}

void CMap_Tool::Render_Prototype_Inspector(ImVec2 vPos)
{

    ImGui::SetNextWindowPos(vPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);
    ImGui::Begin("Prototype_Transform");

    ImGui::Text(m_Selected_PrototypeModelTag.c_str());

    if (!m_IsPicking_Create)
    {
        static float fPosition[3] = { 0.f, 0.f, 0.f };
        ImGui::InputFloat3("Position", fPosition);

        static float fRotation[3] = { 0.f, 0.f, 0.f };
        ImGui::InputFloat3("Rotation", fRotation);

        static float fScale[3] = { 1.f, 1.f, 1.f };
        ImGui::InputFloat3("Scale", fScale);

        if (ImGui::Button("Create Instance"))
        {
            CToolMap_Part::MAP_PART_DESC Desc{};
            Desc.eArgType = CToolMap_Part::ARG_TYPE::CREATE;

            MODEL_CREATE_DESC CreateDesc{};
            CreateDesc.pModelTag = m_wSelected_PrototypeModelTag.c_str();
            CreateDesc.vPosition = _float4(fPosition[0], fPosition[1], fPosition[2], 1.f);
            CreateDesc.vRotate = _float3(fRotation[0], fRotation[1], fRotation[2]);
            CreateDesc.vScale = _float3(fScale[0], fScale[1], fScale[2]);

            /* 구조체 데이터 넣기. */
            Desc.pData = reinterpret_cast<void*>(&CreateDesc);

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
                , TEXT("Layer_Map_Parts")
                , ENUM_CLASS(m_eCurLevel)
                , m_wSelected_PrototypeObjTag, &Desc)))
            {
                MSG_BOX(TEXT("Add GameObject_To_Layer Failed"));
                return;
            }
        }


    }
    else if (m_IsPicking_Create)
    {
        static float fInterval[3] = { 0.f, 0.f, 0.f };
        ImGui::InputFloat3("Interval", fInterval);
        m_vInterval = _float3(fInterval[0], fInterval[1], fInterval[2]);
    }

    /* 인스턴스 생성. */
    ImGui::Checkbox("Picking Create", &m_IsPicking_Create);

    ImGui::End();
}


/* 프로토타입 인덱스, 객체 이름, 모델 컴포넌트 이름.*/
void CMap_Tool::Register_Prototype_Hierarchy(_uint iPrototypeLevelIndex, const _wstring& strObjectTag, const _wstring& strModelPrefix)
{
    /* 실제 Clone 작업은.*/
    list<_wstring> outList = {};
    //m_pGameInstance->Get_PrototypeName_List(outList, ENUM_CLASS(m_eCurLevel), TEXT("MapPart_"));
    

    _char szFullPath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, strObjectTag.c_str(), -1, szFullPath, MAX_PATH, nullptr, nullptr);
    string strObject = szFullPath;


    // 전역 변수로 저장해둠.
    for (_uint i = 0; i < Model_PrototypeSize; ++i)
        outList.push_back(Model_Prototypes[i].prototypeName);
    

     for (auto& modelName : outList)
    {
        _char szModelName[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0, modelName.c_str(), -1, szModelName, MAX_PATH, nullptr, nullptr);
        m_PrototypeNames.emplace_back(strObject, szModelName);
    }

}
void CMap_Tool::Handle_CreateMode_SelectedObject()
{
    /* Create Mode에서 Selecetd Object의 역할*/
    if (nullptr == m_pSelectedObject)
        return;

    if (m_IsPicking_Create)
        Picking_Create();


  
}
void CMap_Tool::Picking_Create()
{
    /* 생성할 위치. 월드 좌표 반영이 안됨. */
    _float3 vPos = {};
    XMStoreFloat3(&vPos, XMLoadFloat3(&m_RayHitDesc.vHitPoint));

    CToolMap_Part::MAP_PART_DESC Desc{};
    Desc.eArgType = CToolMap_Part::ARG_TYPE::CREATE;

    MODEL_CREATE_DESC CreateDesc{};
    CreateDesc.pModelTag = m_wSelected_PrototypeModelTag.c_str();
    CreateDesc.vPosition = _float4(vPos.x + m_vInterval.x, vPos.y + m_vInterval.y, vPos.z + m_vInterval.z, 1.f);
    CreateDesc.vRotate = _float3(0.f, 0.f, 0.f);
    CreateDesc.vScale = _float3(1.f, 1.f, 1.f);

    /* 구조체 데이터 넣기. */
    Desc.pData = reinterpret_cast<void*>(&CreateDesc);

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("Layer_Map_Parts")
        , ENUM_CLASS(m_eCurLevel)
        , m_wSelected_PrototypeObjTag, &Desc)))
    {
        MSG_BOX(TEXT("Create GameObject_To_Layer Failed"));
        return;
    }

    // 이미 생성했으면? => 계속 생성 방지. 
    m_pSelectedObject = nullptr; // 계속 생성 방지.
}
#pragma endregion


#pragma region EDIT MODE

void CMap_Tool::Render_Model_Edit()
{
    if (m_IsPossible_SaveLoad)
        return;
    
    Render_Edit_Hierarchy();
}

/* Load Layer */
void CMap_Tool::Load_Layer()
{
    m_LayerTable = m_pGameInstance->Export_EditLayer(ENUM_CLASS(m_eCurLevel));
}


/*
*  Object Layer에서 Hierarchy에 불러옵니다.
*  객체들이 생성되고 삭제될 수 있으므로 해당 상황마다 동적으로 불러와야 합니다.
*  Layer에 변동사항이 있을때마다 호출합니다.
*/
void CMap_Tool::Render_Edit_Hierarchy()
{
    ImGui::SetNextWindowPos({ g_iWinSizeX - 310.f, 10.f }, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ 300, 400 }, ImGuiCond_Once);
    ImGui::Begin("Edit_Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse);

    /* ---------- 레이어 루프 ---------- */
    for (auto itLayer = m_LayerTable.begin(); itLayer != m_LayerTable.end(); ++itLayer)
    {
        const std::wstring& tagW = itLayer->first;
        CLayer* pLayer = itLayer->second;
        std::string tag = WString_ToString(tagW);

        bool layerSelected = (pLayer == m_pSelectedLayer);
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth
            | ImGuiTreeNodeFlags_DefaultOpen
            | (layerSelected ? ImGuiTreeNodeFlags_Selected : 0);

        bool open = ImGui::TreeNodeEx(tag.c_str(), flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            SelectObject(nullptr);              // 레이어만 클릭 → 오브젝트 선택 해제

        /* ---------- 오브젝트 루프 ---------- */
        if (open && pLayer)
        {
            for (CGameObject* pObj : pLayer->Get_GameObjects())
            {
                if (!pObj) continue;
                std::string objTag = WString_ToString(pObj->Get_ObjectTag());

                ImGui::PushID(pObj);           // 포인터로 ID 충돌 방지
                bool objSel = (pObj == m_pSelectedObject);
                if (ImGui::Selectable(objTag.c_str(), objSel))
                    SelectObject(pObj);        // 트리 클릭 → 선택 동기화
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }

    /* ---------- Inspector 창 ---------- */
    if (m_pSelectedObject)
    {
        ImVec2 pos(ImGui::GetWindowPos().x - 310.f, ImGui::GetWindowPos().y);
        Render_Edit_Inspector(pos);
    }

    ImGui::End();
}


void CMap_Tool::Render_Edit_Inspector(ImVec2 vPos)
{
    ImGui::SetNextWindowPos(vPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);
    if (!ImGui::Begin("Edit_Inspector"))
    {
        ImGui::End(); return;
    }

    ImGui::Text(m_Selected_EditObjTag.c_str());


    /* ----- 선택 객체 없으면 나간다.  ----- */
    if (m_pSelectedObject == nullptr)
    {
        ImGui::End(); return;
    }

    CTransform* pTransformCom = static_cast<CTransform*>(m_pSelectedObject->Get_Component(L"Com_Transform"));

    /* ----- 편집 버퍼 (static) ----- */
    static uint32_t cachedObjID = UINT32_MAX;
    static float fPos[3] = {};
    static float fRot[3] = {};
    static float fScl[3] = { 1.f,1.f,1.f };
    
    /* 선택이 바뀌면 초기화 */
    if (cachedObjID != m_Selected_EditObjID)
    {
        _float3 pos; XMStoreFloat3(&pos, pTransformCom->Get_State(STATE::POSITION));
        _float3 scl = pTransformCom->Get_Scaled();
        _float3 rot = { 0.f, 0.f, 0.f };

        memcpy(fPos, &pos, sizeof(_float3));
        memcpy(fRot, &rot, sizeof(_float3));
        memcpy(fScl, &scl, sizeof(_float3));

        cachedObjID = m_Selected_EditObjID;
    }
    
    /* ----- ImGui 위젯 ----- */
    ImGui::InputFloat3("Position", fPos);
    ImGui::InputFloat3("Rotation", fRot);
    ImGui::InputFloat3("Scale", fScl);

    if (ImGui::Button("Apply"))
    {
        TransformData data{};
        memcpy(&data.pos, fPos, sizeof(_float3)); data.pos.w = 1.f;
        memcpy(&data.rot, fRot, sizeof(_float3));
        memcpy(&data.scale, fScl, sizeof(_float3));

        EditPayload payload;
        payload.type = EEditType::Transform;
        payload.data = data;

        m_pGameInstance->Request_EditObject(
            ENUM_CLASS(m_eCurLevel),
            m_Selected_EditLayerTag,
            m_Selected_EditObjID,
            payload);
    }

    ImGui::SameLine();
    // 오른쪽에 못붙이나?
    if (ImGui::Button("Delete"))
    {
        m_pGameInstance->Request_DeleteObject(
            ENUM_CLASS(m_eCurLevel),
            m_Selected_EditLayerTag,
            m_Selected_EditObjID
        );

        m_pSelectedObject = { nullptr };
    }

    ImGui::End();
}

void CMap_Tool::Handle_EditMode_SelectedObject()
{

}




#pragma endregion

#pragma region Helper 함수.

/* Selected Object를 설정해줍니다. */
void CMap_Tool::Update_Picking(_uint iLayerLevelIndex, const _wstring& strLevelLayerTag)
{
    _float fOutDist = {};
    m_RayHitDesc = m_pGameInstance->Get_PickingLocalObject(iLayerLevelIndex, strLevelLayerTag,
        &fOutDist);

    SelectObject(m_RayHitDesc.pHitObject);
}

void CMap_Tool::Load_EditObject()
{
    m_pGameInstance->Export_EditLayer(ENUM_CLASS(m_eCurLevel));
}

string CMap_Tool::WString_ToString(const wstring& ws)
{
    if (ws.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(len - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, s.data(), len, nullptr, nullptr);
    return s;
}

void CMap_Tool::SelectObject(CGameObject* pObj)
{
    m_pSelectedObject = pObj;

    if (pObj == nullptr)
    {
        m_pSelectedLayer = nullptr;
        return;
    }

    m_Selected_EditObjTag = WString_ToString(pObj->Get_ObjectTag());
    m_Selected_EditObjID = pObj->Get_ID();

    // 레이어 찾기 (빠른 맵을 갖고 있거나 선형 탐색)
    for (auto& kv : m_LayerTable)
    {
        CLayer* pLayer = kv.second;
        _wstring strLayerTag = kv.first;
        for (auto* o : pLayer->Get_GameObjects())
        {
            if (o == pObj)
            {
                m_pSelectedLayer = pLayer;
                m_Selected_EditLayerTag = strLayerTag;
                return;
            }
        }
    }
    m_pSelectedLayer = nullptr; // 못 찾았을 때
}
void CMap_Tool::Default_Render()
{
    ImGui::Text("Settings");
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 WindowColor = style.Colors[ImGuiCol_WindowBg];
    ImGui::SliderFloat("Editor Opacity", &m_fEditorAlpha, 0.0f, 1.0f);
    const ImVec4 NewColor = ImVec4(WindowColor.x, WindowColor.y, WindowColor.z, m_fEditorAlpha);
    style.Colors[ImGuiCol_WindowBg] = NewColor;
    ImGui::NewLine();

    //메뉴바
    if (ImGui::BeginMenuBar())
    {
        // 메뉴
        if (ImGui::BeginMenu("Files"))
        {
            if (ImGui::MenuItem("save"))
                m_bSave = true;
            ImGui::Separator();
            if (ImGui::MenuItem("open"))
                m_bLoad = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::MenuItem("Show Mouse Pos", NULL, &m_bShowSimpleMousePos);
            ImGui::Separator();
            ImGui::MenuItem("Show Picked Object", NULL, &m_bShowPickedObject);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Navigation Tool"))
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Picking for Navigation"); ImGui::SameLine();
            ImGui::Checkbox("##Picking for Navigation", &m_bNaviPicking);

            ImGui::SameLine();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Show Only Navigation"); ImGui::SameLine();
            ImGui::Checkbox("##Show Only Navigation", &m_bShowOnlyNavi);

            ImGui::Text("This is the navigation tool ");
            //Set_Navigation();
            ImGui::EndTabItem();
        }
    }

    
}

//void CMap_Tool::Set_Navigation()
//{
//    static int selected = 0;
//    {
//        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
//
//        if ((int)m_pNavigation_Manager->Get_CellsSize() != 0)
//        {
//            for (_uint i = 0; i < m_pNavigation_Manager->Get_CellsSize();)
//            {
//                //char label[MAX_PATH] = "";
//                char szLayertag[MAX_PATH] = "Cell";
//
//                char label[MAX_PATH] = "Cell ";
//                char buffer[MAX_PATH];
//                sprintf(buffer, "%d", i);
//                strcat(label, buffer);
//                if (ImGui::Selectable(label, m_iCellIndex == i))
//                {
//                    m_iCellIndex = i;
//
//                }
//                i++;
//            }
//        }
//        ImGui::EndChild();
//    }
//    ImGui::SameLine();
//    // ------------------------ Right -----------------------------------
//    {
//
//        ImGui::BeginGroup();
//        ImGui::BeginChild("Cell view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
//
//
//        ImGui::CollapsingHeader("Show Current Cell");
//
//        ImGui::Text("Selected Index : "); ImGui::SameLine();  ImGui::Text("%d", m_iCellIndex);
//        m_pNavigation_Manager->Set_CilckedCellIndex(m_iCellIndex);
//        CCell* pCurrentCell = m_pNavigation_Manager->Get_Cell();
//
//        static float fPointA[3]{ 0,0,0 };
//        static float fPointB[3]{ 0,0,0 };
//        static float fPointC[3]{ 0,0,0 };
//        static float fClickedPosition[3]{ m_fClickPoint.x, m_fClickPoint.y, m_fClickPoint.z };
//        fClickedPosition[0] = m_fClickPoint.x;
//        fClickedPosition[1] = m_fClickPoint.y;
//        fClickedPosition[2] = m_fClickPoint.z;
//
//        if (pCurrentCell != nullptr)
//        {
//            fPointA[0] = pCurrentCell->Get_PointValue(CELLPOINT::A).x;
//            fPointA[1] = pCurrentCell->Get_PointValue(CELLPOINT::A).y;
//            fPointA[2] = pCurrentCell->Get_PointValue(CELLPOINT::A).z;
//
//            fPointB[0] = pCurrentCell->Get_PointValue(CELLPOINT::B).x;
//            fPointB[1] = pCurrentCell->Get_PointValue(CELLPOINT::B).y;
//            fPointB[2] = pCurrentCell->Get_PointValue(CELLPOINT::B).z;
//
//            fPointC[0] = pCurrentCell->Get_PointValue(CELLPOINT::C).x;
//            fPointC[1] = pCurrentCell->Get_PointValue(CELLPOINT::C).y;
//            fPointC[2] = pCurrentCell->Get_PointValue(CELLPOINT::C).z;
//
//            //m_iCellType = pCurrentCell->Get_CellType();
//        }
//
//
//        ImGui::Text("PointA :"); ImGui::SameLine(); ImGui::InputFloat3("##PointA", fPointA);
//        ImGui::Text("PointB :"); ImGui::SameLine(); ImGui::InputFloat3("##PointB", fPointB);
//        ImGui::Text("PointC :"); ImGui::SameLine(); ImGui::InputFloat3("##PointC", fPointC);
//
//        ImGui::Text("ClickPointXYZ :"); ImGui::SameLine(); ImGui::DragFloat3("##ClickPointXYZ", fClickedPosition, 0.01f);
//        m_fClickPoint = _float3(fClickedPosition[0], fClickedPosition[1], fClickedPosition[2]);
//        m_pNavigation_Manager->Update_ClickedPosition(m_fClickPoint);
//
//
//        if (ImGui::Button("Cancle Click Point"))
//            m_pNavigation_Manager->Clear_ClickedPosition();
//
//        if (ImGui::Button("PopBack Cell"))
//            m_pNavigation_Manager->Cancle_Cell();
//        if (ImGui::Button("Erase Picked Cell"))
//            m_pNavigation_Manager->Erase_Cell();
//        if (ImGui::Button("All_Clear Cell"))
//            m_pNavigation_Manager->Clear_Cells();
//
//
//
//        ImGui::CollapsingHeader("Setting Cell Type");
//        //ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "All Accessible"); ImGui::SameLine();
//        //if (ImGui::RadioButton("##All Accessible", &m_iCellType, 0))
//        //    m_pNavigation_Manager->Set_CellType((CCell::CELLTYPE)m_iCellType);
//        //ImGui::SameLine();
//      /*  ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Only Jump"); ImGui::SameLine();
//        if (ImGui::RadioButton("##Only Jump", &m_iCellType, 1))
//            m_pNavigation_Manager->Set_CellType((CCell::CELLTYPE)m_iCellType);
//        ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Drop"); ImGui::SameLine();
//        if (ImGui::RadioButton("##Drop", &m_iCellType, 2))
//            m_pNavigation_Manager->Set_CellType((CCell::CELLTYPE)m_iCellType);
//        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "UpDown"); ImGui::SameLine();
//        if (ImGui::RadioButton("##UpDown", &m_iCellType, 3))
//            m_pNavigation_Manager->Set_CellType((CCell::CELLTYPE)m_iCellType);*/
//
//
//
//       /* if (ImGui::Button("Save Navigation"))
//        {
//            Save_Navigation();
//        }
//        if (ImGui::Button("Load Navigation"))
//        {
//            Load_Navigation();
//        }*/
//
//
//
//
//
//        ImGui::EndChild();
//        ImGui::EndGroup();
//    }
//
//
//
//
//
//    if (m_pGameInstance->Get_KeyUp(DIK_X) && m_pSelectedObject)
//    {
//        
//        _float3 fPosition = m_RayHitDesc.vHitLocal;
//        _vector vPosition = XMLoadFloat3(&fPosition);
//        vPosition = XMVectorSetW(vPosition, 1.f);
//        //m_pNavigation_Manager->Click_Position(vPosition);
//        //m_fClickPoint = m_pNavigation_Manager->Get_ClickedPos();
//
//
//    }
//    else if (m_pSelectedObject 
//        && m_bNaviPicking 
//        && m_pGameInstance->Get_MouseKeyPress(MOUSEKEYSTATE::RB))
//    {
//         _float3 fPosition = m_RayHitDesc.vHitLocal;
//         _vector vPosition = XMLoadFloat3(&fPosition);
//         vPosition = XMVectorSetW(vPosition, 1.f);
//        
//
//    }
//
//}
#pragma endregion





HRESULT CMap_Tool::Ready_Imgui()
{
    m_pImgui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pDeviceContext);
    if (nullptr == m_pImgui_Manager)
    {
        MSG_BOX(TEXT("Failed to Created : CImgui_Manager"));
        return E_FAIL;
    }
    
    return S_OK;
}

HRESULT CMap_Tool::Ready_Events()
{
#pragma region HP_CHANGE
    //m_pGameInstance->Subscribe(EventType::HP_CHANGE, Get_ID(), [this](void* pData)
    //    {
    //        HPCHANGE_DESC* desc = static_cast<HPCHANGE_DESC*>(pData);
    //        if (desc->bIncrease)
    //            this->Increase_Hp(desc->iHp, desc->fTime);
    //        else
    //            this->Decrease_Hp(desc->iHp, desc->fTime);
    //    });

    //// Event 목록 관리.
    //m_Events.push_back(EventType::HP_CHANGE);

#pragma endregion


    /*m_pGameInstance->Subscribe(EventType::MAPTOOL_LOAD_LAYER, Get_ID(), [this](void* pData) {
        TOOL_LOAD_LAYER* desc = static_cast<TOOL_LOAD_LAYER>
        });*/
    // m_pGameInstance->Publish()
    return S_OK;
}

CMap_Tool* CMap_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
    CMap_Tool* pInstance = new CMap_Tool(pDevice, pContext);
    if (FAILED(pInstance->Initialize(eLevel)))
    {
        MSG_BOX(TEXT("Create Failed : CMap_Tool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Tool::Free()
{
    __super::Free();

    for (auto& Event : m_Events)
        m_pGameInstance->UnSubscribe(Event, Get_ID());

    m_PrototypeNames.clear();

    /* Edit Mode Object Clear */
    

    Safe_Release(m_pSaveFile_Loader);
    Safe_Release(m_pCamera);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
}
