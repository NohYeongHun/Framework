﻿#pragma once
#include "Base.h"

NS_BEGIN(Tool)

using LayerTable = map<const _wstring, class CLayer*>;

class CMap_Tool final : public CBase
{
public:
	enum TOOLMODE 
	{
		// 1. 프로토타입 하이어라키를 선택했을 때 메시를 원하는 위치에 배치하도록.
		// 메시에 클릭 했을 때 해당 메시 옆에 배치되도록.
		CREATE = 0, 
		EDIT = 1,
		RENDER_END
	};

	enum SAVEMODE
	{
		MAP_OBJECT = 0,
		MODEL_COMPONENT = 1,
		SAVE_END
	};


private:
	explicit CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize(LEVEL eLevel);

public:
	void Change_SelectObject(class CGameObject* pSelectedObject);
	void Update(_float fTimeDelta);
	
	void Render();
	void Render_SaveLoad();
	void Render_Debug_Window();
	void Handle_SelectedObject();

#pragma region Create Mode
public:
	void Render_Model_Create();
	void Render_Prototype_Hierarchy();
	void Render_Prototype_Inspector(ImVec2 vPos);
	void Register_Prototype_Hierarchy(_uint iPrototypeLevelIndex, const _wstring& strObjectTag, const _wstring& strModelPrefix);
	void Handle_CreateMode_SelectedObject();
	void Picking_Create();
#pragma endregion

#pragma region Edit Mode
public:
	void Render_Model_Edit();
	void Load_Layer();
	void Render_Edit_Hierarchy();
	void Render_Edit_Inspector(ImVec2 vPos);
	void Handle_EditMode_SelectedObject();
#pragma endregion


#pragma region Map Tool의 기능들
public:
	void Update_Picking(_uint iLayerLevelIndex, const _wstring& strLevelLayerTag);
	void Load_EditObject(); // 현재 레벨에 생성된 객체들을 모두 불러옵니다.

	string WString_ToString(const wstring& ws);
	void SelectObject(class CGameObject* pObj);
	

	void Default_Render();
	//void Set_Navigation();
#pragma endregion



#pragma region CREATE 시 사용 변수
private:
	/* 현재 선택된 SelectedObjTag */
	string m_Selected_PrototypeObjTag = {};
	string m_Selected_PrototypeModelTag = {};
	_wstring m_wSelected_PrototypeObjTag = {};
	_wstring m_wSelected_PrototypeModelTag = {};

	list<pair<string, string>> m_PrototypeNames = {};

	/* Prototype Hierarchy Pos 저장 */
	ImVec2 m_PrototypeinspectorPos = {};
	_bool m_IsPicking_Create = {};
#pragma endregion

#pragma region EDIT 시 사용 변수.

private:
	string m_Selected_EditObjTag = {};
	uint32_t m_Selected_EditObjID = {};
	_wstring m_Selected_EditLayerTag = {};

	/* Edit Hierarchy Pos 저장 */
	ImVec2 m_EditinspectorPos = {};
	int m_iSelectedIndex = { -1 };
#pragma endregion

#pragma region 공통
private:
	RAYHIT_DESC m_RayHitDesc = {};
	_bool m_IsPossible_Picking = { false };
	_bool m_IsPossible_SaveLoad = { false };
	
	class CGameObject* m_pSelectedObject = { nullptr }; // 선택된 객체
	class CLayer* m_pSelectedLayer = { nullptr };
	TOOLMODE m_eToolMode = { TOOLMODE::CREATE }; // 상태 저장.

	LEVEL m_eCurLevel = {};
	_float m_Interval = 1.f;
	_float3 m_vInterval = {};

	SAVEMODE m_eSaveMode = {};

	_float m_fEditorAlpha = { 0.5f};
	_bool m_bSave = {};
	_bool m_bLoad = {};
	_bool m_bShowSimpleMousePos = {};
	_bool m_bShowPickedObject = {};

	_bool m_bNaviPicking = {};
	_bool m_bShowOnlyNavi = {};


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CCamera_Free* m_pCamera = { nullptr };
	class CTransform* m_pCameraTransformCom = { nullptr };
	class CSaveFile_Loader* m_pSaveFile_Loader = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CImgui_Manager* m_pImgui_Manager = { nullptr };

	LayerTable m_LayerTable = {};
	vector<EventType> m_Events = {};
#pragma endregion


private:
	HRESULT Ready_Imgui();
	HRESULT Ready_Events();
	



//private:
//	void ImGui_MenuBar_Render();


public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual void Free();
};
NS_END

