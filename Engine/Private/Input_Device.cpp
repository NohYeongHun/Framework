﻿
#include "Input_Device.h"

CInput_Device::CInput_Device(void)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
}

_bool CInput_Device::Get_KeyPress(_ubyte byKeyID)
{
	_bool IsCurrent = m_byKeyState[byKeyID] & 0x80;
	_bool IsPrev = m_byPrevKeyState[byKeyID] & 0x80;

	return IsCurrent && IsPrev;
}

_bool CInput_Device::Get_KeyDown(_ubyte byKeyID)
{
	_bool IsCurrent = m_byKeyState[byKeyID] & 0x80;
	_bool IsPrev = m_byPrevKeyState[byKeyID] & 0x80;

	return IsCurrent && !IsPrev;
}

_bool CInput_Device::Get_KeyUp(_ubyte byKeyID)
{
	_bool IsCurrent = m_byKeyState[byKeyID] & 0x80;
	_bool IsPrev = m_byPrevKeyState[byKeyID] & 0x80;

	return !IsCurrent && IsPrev;
}

_bool CInput_Device::Get_MouseKeyPress(MOUSEKEYSTATE eMouse)
{
	_bool IsCurrent = m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;
	_bool IsPrev = m_tPrevMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;

	return IsCurrent && IsPrev;
}

_bool CInput_Device::Get_MouseKeyUp(MOUSEKEYSTATE eMouse)
{
	_bool IsCurrent = m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;
	_bool IsPrev = m_tPrevMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;

	return !IsCurrent && IsPrev;
}

_bool CInput_Device::Get_MouseKeyDown(MOUSEKEYSTATE eMouse)
{
	_bool IsCurrent = m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;
	_bool IsPrev = m_tPrevMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;
	
	return IsCurrent && !IsPrev;
}



HRESULT CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;
	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

/*
* 
*/
void CInput_Device::Update(void)
{
	memcpy(m_byPrevKeyState, m_byKeyState, sizeof(m_byPrevKeyState));
	memcpy(&m_tPrevMouseState, &m_tMouseState, sizeof(m_tPrevMouseState));

	// Device State를 m_byKeyState에 받아온다.
	if (FAILED(m_pKeyBoard->GetDeviceState(256, m_byKeyState)))
	{
		m_pKeyBoard->Acquire();
		ZeroMemory(&m_byKeyState, sizeof(m_byKeyState));
	}

	if (FAILED(m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState)))
	{
		m_pMouse->Acquire();
		ZeroMemory(&m_tMouseState, sizeof(m_tMouseState));
	}
}

CInput_Device* CInput_Device::Create(HINSTANCE hInstance, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

