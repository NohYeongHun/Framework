﻿#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoad_Animation final : public CBase
{
private:
	explicit CLoad_Animation();
	explicit CLoad_Animation(const CLoad_Animation& Prototype);

	virtual ~CLoad_Animation() = default;


public:
	HRESULT Initialize(std::ifstream& ifs);
	void Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _bool isLoop, _bool* pFinished, BLEND_DESC& blendDesc, _float fTimeDelta);
	void Blend_Update_TransformationMatrices(const vector<class CLoad_Bone*>& Bones, _bool isLoop, _bool* pFinished, BLEND_DESC& blendDesc, _float fTimeDelta);
	const _float Get_CurrentTrackPosition() { return m_fCurrentTrackPosition; }
	void Set_CurrentTrackPosition(_float fTrackPosition) { m_fCurrentTrackPosition = fTrackPosition; }
	_matrix Get_BoneMatrixAtTime(_uint iBoneIndex, _float fCurrentTrackPosition);
	const _float Get_Duration() { return m_fDuration; } // ⭐ 새로 추가

	/* 보간 */
public:
	void Build_BoneChannelCache(_uint iNumBones); // 채널과 Bone과의 관계를 캐시해두기
	void Reset();
	void Update_TrackPosition(_float fTimeDelta);
	CLoad_Channel* Find_Channel(_uint iBoneIndex);

	const vector<class CLoad_Channel*>& Get_Channels() { return m_Channels; }

	_uint Get_CurrentKeyFrame();
	void Set_TickPerSecond(_float fTickPerSecond) { m_fTickPerSecond = fTickPerSecond;  }
	_float Get_TickPerSecond() { return m_fTickPerSecond; }


private:
	/* 채널 이름 */
	_char m_szName[MAX_PATH] = {};
	/* 애니메이션 전체 재생 길이 */
	_float m_fDuration = {};

	/* 초당 이동해야할 재생 거리 */
	_float m_fTickPerSecond = {};
	_float m_fCurrentTrackPosition = {};
	_float m_fBlendTrackPosition = {};

	/* 이 동작을 위한 뼈들의 상태 */
	_uint m_iNumChannels = {};
	vector<class CLoad_Channel*> m_Channels = {};

	// 본 인덱스에 맞는 채널들 캐싱
	vector<class CLoad_Channel*> m_BoneChannelCache; 
	// 최근에 재생한 키프레임의 인덱스들.
	vector<_uint> m_CurrentKeyFrameIndices;

private:
	HRESULT Load_Channels(std::ifstream& ifs);
	

public:
	static CLoad_Animation* Create(std::ifstream& ifs);
	CLoad_Animation* Clone();
	virtual void Free();


};
NS_END

