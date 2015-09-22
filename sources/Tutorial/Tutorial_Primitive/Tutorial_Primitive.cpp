#include "DXUT.h"
#include "Tutorial_Primitive.h"


Tutorial_Primitive::Tutorial_Primitive()
{
}


Tutorial_Primitive::~Tutorial_Primitive()
{
	SAFE_RELEASE(m_pTex);
}

void Tutorial_Primitive::Init()
{
	iSpriteindex = 1;

	fAanimTime = 0.1f;
	fPassTime = 0.0f;

	m_pVtx[0] = VtxRHWUV1(0.f, 0.f, 0.f, 0.f, 0.f);		//Left Bottom
	m_pVtx[1] = VtxRHWUV1(0.f, 53.f, 0.f, 0.f, 1.f);		//Left UP
	m_pVtx[2] = VtxRHWUV1(31.25f, 0.f, 0.f, 1.f/8.f, 0.f);		//Right Bottom
	m_pVtx[3] = VtxRHWUV1(31.25f, 53.f, 0.f, 1.f/8.f, 1.f);		//Right UP

	// 인덱스 버퍼 값을 갱신한다.
	WORD dwIndices[] = { 0, 1, 2, 3, 2, 1 };
	memcpy(m_pIdx, dwIndices, sizeof(WORD)* 6);

/*
	if (FAILED(DXUTGetD3D9Device()->CreateVertexBuffer(sizeof(pVtx), 0, VtxRHWUV1::FVF, D3DPOOL_MANAGED, &m_pVB, NULL)))
	{
		return;
	}*/
	D3DXCreateTextureFromFile(DXUTGetD3D9Device(), L"Sprites/Laharl.png", &m_pTex);
}

void Tutorial_Primitive::FrameMove(float _fdeltaTime)
{
	if (fPassTime > fAanimTime)
	{
		fPassTime = 0.0f;
		iSpriteindex++;

	}
	fPassTime += _fdeltaTime;


	if (iSpriteindex > 8)
	{
		iSpriteindex = 1;
	}

	m_pVtx[0] = VtxRHWUV1(0.f, 0.f, 0.f, (iSpriteindex-1) / 8.f, 0.f);		//Left Bottom
	m_pVtx[1] = VtxRHWUV1(0.f, 53.f*4, 0.f, (iSpriteindex - 1) / 8.f, 1.f);		//Left UP
	m_pVtx[2] = VtxRHWUV1(250.0f/8.0f*4, 0.f, 0.f, iSpriteindex / 8.f, 0.f);		//Right Bottom
	m_pVtx[3] = VtxRHWUV1(250.0f/8.0f*4, 53.f*4, 0.f, iSpriteindex / 8.f, 1.f);		//Right UP
	//printf("frameIndex : %d\n", iSpriteindex);
	//printf("frameIndex : %f\n", _fdeltaTime);
}

void Tutorial_Primitive::Render(float _fdeltaTime)
{
	const LPDIRECT3DDEVICE9 dev = DXUTGetD3D9Device();
	dev->SetTexture(0, m_pTex);

	// SetRenderState : Alpha blend enable - true
	// SetRenderState : Alpha test enable - false
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	dev->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	dev->SetFVF(VtxRHWUV1::FVF);

	dev->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST
		, 0					// Minimum Vertex Index
		, 6					// Number vertex indices
		, 2					// Primitive Count
		, m_pIdx			// IndexData pointer
		, D3DFMT_INDEX16	// Index Data format
		, m_pVtx			// Vetex stream zero data
		, sizeof(VtxRHWUV1)	// Vertex Stream Zero Stride
		);
	//DXUTGetD3D9Device()->SetTexture(0, NULL);
}
