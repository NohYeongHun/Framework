﻿#include "../Public/Base.h"

CBase::CBase()
	: m_iID{ CIdGen::Next() }
{

}

unsigned int CBase::AddRef()
{
	return ++m_iRefCnt;
}

unsigned int CBase::Release()
{
	if (0 == m_iRefCnt)
	{
		Free();

		/* 삭제한다. */
		delete this;

		return 0;
	}
	else
	{
		return m_iRefCnt--;
	}
}

void CBase::Free()
{

}
