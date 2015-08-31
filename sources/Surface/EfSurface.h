#pragma once
#include <theoraplayer/TheoraPlayer.h>

class CEfSurface
{
public:
	struct VtxwDUV
	{
		D3DXVECTOR4 p;
		DWORD d;
		FLOAT u, v;

		VtxwDUV() {}
		VtxwDUV(FLOAT X, FLOAT Y, FLOAT Z, FLOAT U, FLOAT V, DWORD D = 0xFFFFFFFF)
			:p(X, Y, Z, 1.F), u(U), v(V), d(D){}

		enum { FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1) };
	};

	INT ScreenW;
	INT ScreenH;

	TheoraVideoManager* m_Vmgr;
	TheoraVideoClip* m_pClip;
public:
	CEfSurface();
	virtual ~CEfSurface();

	INT Create(LPDIRECT3DDEVICE9 pDev);
	void Destroy();

	INT Restore(LPDIRECT3DDEVICE9 pDev);
	void Invalidate();

	INT FrameMove(LPDIRECT3DDEVICE9 pDev, float _elapsedTime);
	void Render(LPDIRECT3DDEVICE9 pDev);

	void ProcessFrames();
protected:
	LPDIRECT3DTEXTURE9 m_pTx;
	LPDIRECT3DSURFACE9 m_pSf;
	LPD3DXMESH			m_pMesh;
	INT m_iTxW;	// Render Target Texture Width

	VtxwDUV m_pVtx[4];
};

