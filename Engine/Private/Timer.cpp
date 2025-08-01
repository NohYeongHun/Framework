#include "Timer.h"

CTimer::CTimer() 
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_FixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_LastTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_FrameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}


HRESULT CTimer::Initialize_Clone()
{
	/* 현재 cpu카운팅한 숫자. */
	QueryPerformanceCounter(&m_FrameTime);			// 1077
	QueryPerformanceCounter(&m_LastTime);			// 1085
	QueryPerformanceCounter(&m_FixTime);			// 1090

	/* cpu가 1초에 카운팅할 수 있는 최대 숫자. */
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void CTimer::Update_Timer()
{
	QueryPerformanceCounter(&m_FrameTime);			// 1500

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = (m_FrameTime.QuadPart - m_LastTime.QuadPart) / static_cast<_float>(m_CpuTick.QuadPart);

	m_LastTime = m_FrameTime;
}

CTimer* CTimer::Create()
{
	CTimer* pInstance = new CTimer();

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX(TEXT("Failed to Created : CTimer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
	__super::Free();
}
