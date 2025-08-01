﻿#pragma once
#include "EnginePch.h"
#include "Tool_Defines.h"

#pragma region TOOL 관련
#include "SaveFile_Define.h"
#include "SaveFile_Loader.h"
#include "PrototypeName.h"
#pragma endregion



#pragma region IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Imgui_Define.h"
#include "Imgui_Manager.h"
#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"
#pragma endregion

#include "Tool_MainApp.h"
#include "Map_Tool.h"

#pragma region LOADER
#include "Loader.h"
#pragma endregion

#pragma region LEVEL
#include "Level_Loading.h"
#include "Level_Logo.h"
#pragma endregion

#pragma region Model
#include "Tool_Bone.h"
#include "Tool_Channel.h"
#include "Tool_Animation.h"
#include "Tool_Mesh.h"
#include "Tool_MeshMaterial.h"
#include "Tool_Model.h"
#pragma endregion


#pragma region GAMEOBJECT
#include "ToolMap_Part.h" // 저장 용
#include "Map_Part.h"	  // Load 용
#include "Map.h"
#include "Camera_Free.h"
#include "Player.h"
#pragma endregion



//static void Transform_Print_Imgui(const char* szName, _float* vPos)
//{
//	ImGui::Begin(szName);
//	ImGui::InputFloat3("pos", vPos);
//	ImGui::End();
//}