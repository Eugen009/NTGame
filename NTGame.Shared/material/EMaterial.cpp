#include "pch.h"
#include "Content\ShaderStructures.h"
#include "common\StepTimer.h"
#include <sstream>
#include "../Common/DirectXHelper.h"
#include "EMaterial.h"
#include "../Resource/EResMgr.h"

EMaterial::EMaterial()
	: m_bLoadCompleted( false )
{
	m_strVertShader = L"SampleVertexShader.cso";
	m_strPixelShader = L"SamplePixelShader.cso";
}

void EMaterial::PrepareMaterial(){
	EResMgr::GetInstance().LoadRes(m_strVertShader);
	EResMgr::GetInstance().LoadRes(m_strPixelShader);
}

bool EMaterial::SetMaterial(ID3D11Device* dev, ID3D11DeviceContext* context ){
	if (this->m_vertexShader == nullptr) {
		if (EResMgr::GetInstance().IsResLoaded(m_strVertShader)){
			this->CreateVertexShader(dev, EResMgr::GetInstance().GetResData(m_strVertShader));
		}
	}
	if (this->m_pixelShader == nullptr) {
		if (EResMgr::GetInstance().IsResLoaded(this->m_strPixelShader)){
			this->CreatePixelShader(dev, EResMgr::GetInstance().GetResData(this->m_strPixelShader));
		}
	}
	if (m_vertexShader != nullptr && this->m_pixelShader != nullptr){
		context->VSSetShader(
			m_vertexShader.Get(),
			nullptr,
			0
			);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.Get(),
			nullptr,
			0
			);
	}
}

void EMaterial::Destroy(){
	EResMgr::GetInstance().UnLoadRes(m_strVertShader);
	EResMgr::GetInstance().UnLoadRes(m_strPixelShader);
}

void EMaterial::CreateVertexShader(ID3D11Device* dev, const std::vector<byte>& fileData){
	DX::ThrowIfFailed(
		dev->CreateVertexShader(
		&fileData[0],
		fileData.size(),
		nullptr,
		&m_vertexShader
		)
	);

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
#ifdef _RENDER_TEX
			//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
#endif
	};

	DX::ThrowIfFailed(
		dev->CreateInputLayout(
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		&fileData[0],
		fileData.size(),
		&m_inputLayout
		)
	);
}

void EMaterial::CreatePixelShader(ID3D11Device* dev, const std::vector<byte>& fileData){
	DX::ThrowIfFailed(
		dev ->CreatePixelShader(
		&fileData[0],
		fileData.size(),
		nullptr,
		&m_pixelShader
		)
	);
}


