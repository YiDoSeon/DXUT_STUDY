#include "DXUT.h"
#include "EfSurface.h"


CEfSurface::CEfSurface()
{
	m_pTx = NULL;
	m_pSf = NULL;
	m_pMesh = NULL;

	m_iTxW = 512;
	ScreenW = 1024;
	ScreenH = 768;
}


CEfSurface::~CEfSurface()
{
	Destroy();
}

INT CEfSurface::Create(LPDIRECT3DDEVICE9 pDev)
{
	//화면 크기 얻기
	LPDIRECT3DSURFACE9 pBackSuface = NULL;
	D3DSURFACE_DESC dsc;

	if (FAILED(pDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSuface)))
	{
		return -1;
	}

	pBackSuface->GetDesc(&dsc);
	pBackSuface->Release();

	FLOAT sx = FLOAT(dsc.Width);
	FLOAT sy = FLOAT(dsc.Height);

	sx /= 2.0f;
	sy /= 2.0f;

	m_pVtx[0] = VtxwDUV(0, 0, 0.f, 0.0005f, 0.05f, 0xFFFFFF);
	m_pVtx[1] = VtxwDUV(sx, 0, 0.f, 0.9995f, 0.05f, 0xFFFFFF);
	m_pVtx[2] = VtxwDUV(0, sy , 0.f, 0.0005f, 0.95f, 0xFFFFFF);
	m_pVtx[3] = VtxwDUV(sx , sy , 0.f, 0.9995f, 0.95f, 0xFFFFFF);

	D3DXCreateBox(pDev, 1.0f, 1.0f, 1.0f, &m_pMesh, 0);

	m_Vmgr = new TheoraVideoManager();

	m_pClip = m_Vmgr->createVideoClip("Videos/GameIntro.ogv", TH_BGRA);
	m_pClip->play();

	// TO DO : SURFACE 와 TEXTURE 
	return 0;
}

void CEfSurface::Destroy()
{
	if (m_Vmgr) delete m_Vmgr;
	SAFE_RELEASE(m_pSf);
	SAFE_RELEASE(m_pTx);
	SAFE_RELEASE(m_pMesh);
}

INT CEfSurface::Restore(LPDIRECT3DDEVICE9 pDev)
{
	// 텍스처 해상도?
	//pDev->CreateTexture(1024, 768, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTx, NULL);
	pDev->CreateTexture(1024, 768, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTx, NULL);
	m_pTx->GetSurfaceLevel(0, &m_pSf);

	return 0;
}

void CEfSurface::Invalidate()
{
	SAFE_RELEASE(m_pSf);
	SAFE_RELEASE(m_pTx);
	//SAFE_RELEASE(m_pMesh);
}

INT CEfSurface::FrameMove(LPDIRECT3DDEVICE9 pDev, float _elapsedTime)
{

	ProcessFrames();

	m_Vmgr->update(_elapsedTime);
	HRESULT hr = 0;

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	/*pDev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	pDev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);*/

	LPDIRECT3DSURFACE9 pSfOrgD = NULL;	// Back Buffer Depth and Stencil
	LPDIRECT3DSURFACE9 pSfOrgT = NULL;	// Back Buffer target;

	/*hr = pDev->GetRenderTarget(0, &pSfOrgT);
	hr = pDev->GetDepthStencilSurface(&pSfOrgD);
	hr = pDev->SetRenderTarget(0, m_pSf);*/

	hr = pDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF006699, 1.0f, NULL);

	m_pMesh->DrawSubset(0);

	//hr = pDev->SetRenderTarget(0, pSfOrgT);										//렌더 타겟을 원래 대로.
	//hr = pDev->SetDepthStencilSurface(pSfOrgD);

	SAFE_RELEASE(pSfOrgT);
	SAFE_RELEASE(pSfOrgD);


	return 0;
}

void CEfSurface::Render(LPDIRECT3DDEVICE9 pDev)
{
	pDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDev->SetTexture(0, m_pTx);
	pDev->SetFVF(VtxwDUV::FVF);
	pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_pVtx, sizeof(VtxwDUV));

	pDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	

}

void CEfSurface::ProcessFrames()
{
	TheoraVideoFrame* frame = m_pClip->getNextFrame();

	if (frame)
	{
		//unsigned char* frameData = frame->getBuffer();
		BYTE* pTexBuffer;
		LONG TexPitch;

		BYTE* pTextureBytes = NULL;
		UINT row, col;

		D3DLOCKED_RECT d3dlr;

		m_pSf->LockRect(&d3dlr, 0, D3DLOCK_DISCARD);

		pTexBuffer = static_cast<byte*>(d3dlr.pBits);
		TexPitch = d3dlr.Pitch;
		pTextureBytes = pTexBuffer;

		unsigned char* mb = (unsigned char*)frame->getBuffer();

		for (row = 0; row < (UINT)m_pClip->getHeight(); row++)
		{
			pTextureBytes = pTexBuffer;
			//memcpy(&bits[row*TexPitch], &mb[m_pClip->getHeight() * 4], m_pClip->getHeight() * 4);
			for (col = 0; col < (UINT)m_pClip->getWidth(); col++)
			{
				pTextureBytes[2] = mb[0]; // r
				pTextureBytes[1] = mb[1]; // g
				pTextureBytes[0] = mb[2]; // b

				//pTextureBytes[3] = 0xff;
				pTextureBytes[3] = mb[3];
			}
			pTextureBytes += 4;
			mb += 3;
		}
		pTexBuffer += TexPitch;
		m_pSf->UnlockRect();

		// TO DO : SURFACE & TEXTURE UPDATE

		m_pClip->popFrame();
	}
}