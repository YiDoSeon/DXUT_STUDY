#pragma once

struct VtxRHWUV1
{
	D3DXVECTOR4 p;
	float u, v;
	VtxRHWUV1() {};
	VtxRHWUV1(float _x, float _y, float _z, float _u, float _v)
	{
		p.x = _x;
		p.y = _y;
		p.z = _z;
		p.w = 1.0f;

		u = _u;
		v = _v;
	}
	enum {FVF=D3DFVF_XYZRHW|D3DFVF_TEX1};
};

class Tutorial_Primitive
{
public:
	Tutorial_Primitive();
	virtual ~Tutorial_Primitive();

	void Init();

	void FrameMove(float _fdeltaTime);
	void Render(float _fdeltaTime);

	void Destroy();

public:
	int iSpriteindex;
	float fAanimTime;
	float fPassTime;
	VtxRHWUV1 m_pVtx[4];
	WORD	m_pIdx[6];
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DTEXTURE9 m_pTex;
};

