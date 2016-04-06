#include "pch.h"
#include "Content\ShaderStructures.h"
#include "common\StepTimer.h"
#include <sstream>
#include "../Common/DirectXHelper.h"
#include "EMaterial.h"
#include "../Resource/EResMgr.h"
#include "../Resource/DDSLoader.h"

EMaterial::EMaterial()
	: m_bLoadCompleted( false )
{
	m_strVertShader = L"SampleVertexShader.cso";
	m_strPixelShader = L"SamplePixelShader.cso";
}

void EMaterial::Prepare(){
	EResMgr::GetInstance().LoadRes(m_strVertShader);
	EResMgr::GetInstance().LoadRes(m_strPixelShader);
	EResMgr::GetInstance().LoadRes(m_strTexture);
}

void EMaterial::SetVertexShader(const wchar_t* filename){
	m_strVertShader = filename;
}

void EMaterial::SetPixelShader(const wchar_t* filename){
	m_strPixelShader = filename;
}

void EMaterial::SetTexture(const wchar_t* filename){
	m_strTexture = filename;
}

bool EMaterial::IsOK(){
	return (m_vertexShader != nullptr && this->m_pixelShader != nullptr && m_TexView != nullptr);
}

bool EMaterial::CheckLoaded(ID3D11Device* dev){
	if (m_vertexShader != nullptr && this->m_pixelShader != nullptr && m_TexView != nullptr) return true;
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
	if (this->m_TexView == nullptr){
		if (EResMgr::GetInstance().IsResLoaded(this->m_strTexture)){

			const EDataBytes& fileData = EResMgr::GetInstance().GetResData(this->m_strTexture);

			ID3D11Resource* texRes = nullptr;

			CreateDDSTextureFromMemory(
				dev,
				fileData.data(),
				fileData.size(),
				nullptr, &(this->m_TexView)
				);

			// Once the texture view is created, create a sampler.  This defines how the color
			// for a particular texture coordinate is determined using the relevant texture data.
			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));

			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			// The sampler does not use anisotropic filtering, so this parameter is ignored.
			samplerDesc.MaxAnisotropy = 0;

			// Specify how texture coordinates outside of the range 0..1 are resolved.
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

			// Use no special MIP clamping or bias.
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			// Don't use a comparison function.
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

			// Border address mode is not used, so this parameter is ignored.
			samplerDesc.BorderColor[0] = 0.0f;
			samplerDesc.BorderColor[1] = 0.0f;
			samplerDesc.BorderColor[2] = 0.0f;
			samplerDesc.BorderColor[3] = 0.0f;

			Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
			DX::ThrowIfFailed(
				dev->CreateSamplerState(
				&samplerDesc,
				&(this->m_Sampler)
				)
				);
		}
	}
}

bool EMaterial::SetMaterial(ID3D11DeviceContext* context, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer){
	if (m_vertexShader != nullptr && this->m_pixelShader != nullptr && m_TexView != nullptr){
		
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetInputLayout(m_inputLayout.Get());
		
		context->VSSetShader(
			m_vertexShader.Get(),
			nullptr,
			0
			);

		// Send the constant buffer to the graphics device.
		context->VSSetConstantBuffers(
			0,
			1,
			constantBuffer.GetAddressOf()
			);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.Get(),
			nullptr,
			0
			);

		context->PSSetShaderResources(
			0,
			1,
			this->m_TexView.GetAddressOf() //textureView.GetAddressOf()
			);

		context->PSSetSamplers(
			0,
			1,
			this->m_Sampler.GetAddressOf() //sampler.GetAddressOf()
			);



		return true;
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
//#ifdef _RENDER_TEX
			//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//#endif
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


