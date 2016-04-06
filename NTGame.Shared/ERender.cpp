#include "pch.h"
#include "Content\ShaderStructures.h"
#include "common\StepTimer.h"
#include "ERender.h"
#include "Common\DirectXHelper.h"
#include "EObjResource.h"
#include "ERender\EFbxObj.h"
#include <sstream>
#include "Resource/DDSLoader.h"
#include <iostream>
#include "Content/ShaderStructures.h"
#include "ERender/EMesh.h"
#include "ERender/EMaterial.h"

using namespace EResource;

#define _RENDER_TEX

#ifdef _RENDER_TEX
int ERender::ESimpleRender::VEX_CUSTOM_SIZE = 3 + 3 + 2;
#else
int ERender::ESimpleRender::VEX_CUSTOM_SIZE = 3 + 3;
#endif

namespace ERender{

	struct BasicVertex{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 norm;
		DirectX::XMFLOAT2 tex;
	};

	struct ConstantBuffer{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	ESimpleRender::ESimpleRender(const std::shared_ptr<DX::DeviceResources>& deviceResources)
		:Sample3DSceneRenderer(deviceResources){
		//m_TexView = nullptr;
		//m_Sampler = nullptr;
		//m_bRenderTex = false;
		EMesh::CreateMeshBuffer();
	}

	ESimpleRender::~ESimpleRender(){
		EMesh::ClearMeshBuffer();
		//if (this->m_ObjData){
		//	delete[] m_ObjData;
		//	m_ObjData = NULL;
		//}
		//if (this->m_Indexes){
		//	delete[] m_Indexes;
		//	m_Indexes = NULL;
		//}
		//if (this->m_Res){
		//	delete m_Res;
		//	m_Res = NULL;
		//}
	}

#if 0
	void ESimpleRender::CreateDeviceDependentResources(){

		// prepare the memory
		if (m_ObjData == NULL){
			m_ObjData = new float[MAX_VEX_NUM * VEX_CUSTOM_SIZE];
		}

		if (this->m_Indexes == NULL){
			m_Indexes = new unsigned short[MAX_FACE_NUM * 3];
		}

		// begin to loading something

		//BasicLoader* loader = new BasicLoader(m_d3dDevice.Get());

		// Load shaders asynchronously.
		auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
		auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

		// After the vertex shader file is loaded, create the shader and input layout.
		auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader)
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
				m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
				);
		});

		// After the pixel shader file is loaded, create the shader and constant buffer.
		auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
				);

			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(NTGame::ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
				);
		});

		m_Res = new EObjResource(ref new String
				(L"box.obj"));

#ifdef _RENDER_TEX
		auto textureLoadTask = DX::ReadDataAsync(L"hand.dds");
		auto textureCreateTask = textureLoadTask.then([this](const std::vector<byte>& fileData){

			ID3D11Resource* texRes = nullptr;
			//ID3D11ShaderResourceView* texView = nullptr;

			CreateDDSTextureFromMemory(
				this->m_deviceResources->GetD3DDevice(),
				fileData.data(),
				fileData.size(),
				nullptr, &(this ->m_TexView)
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
				m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&samplerDesc,
				&(this->m_Sampler)
				)
				);
			});
#endif
			
				 //(L"mrQ.obj"));
		auto resTask = m_Res->ReLoad();

		(resTask && createVSTask && createPSTask && textureCreateTask).then([this]() {
			this->renderMesh( m_Res );
			m_loadingComplete = true;
		});

		//EFbxObj testObj( "testbox.fbx" );
		//testObj.DoTest();


	}
#else
void ESimpleRender::CreateDeviceDependentResources(){

	//prepare material
	this->m_Material = std::shared_ptr<EMaterial>( new EMaterial() );
	this->m_Mesh = std::shared_ptr<EMesh>( new EMesh(L"box.obj") );
	m_Material->SetTexture(L"hand.dds");

	m_Material->Prepare();
	m_Mesh->Prepare();

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(NTGame::ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
		&constantBufferDesc,
		nullptr,
		&m_constantBuffer
		)
		);

}

#endif

#if 0
#define SCALE_VEC( v, s ) (v).x*=s;(v).y*=s;(v).z*=s;

	void ESimpleRender::renderMesh(EResource::EObjResource* obj){
		if (m_ObjData == NULL || m_Indexes == NULL || obj ->getMeshCount() == 0 ) return;

		//const std::vector<ObjMesh>& meshes = mesh ->getAllMeshes();
		const ObjMesh& mesh = obj->getMeshAt(0);
		const std::vector<ObjVec>&  vec = 
			//obj->getAllVexes();
			 mesh.vertexes;
		if (vec.size()== 0) return;
		std::vector<ObjVec>::const_iterator it;
		float* p = m_ObjData;
		assert(MAX_VEX_NUM > vec.size());
		for (it = vec.begin(); it != vec.end(); it++){
			DirectX::XMFLOAT3 tp = it->pos;
			SCALE_VEC(tp, 0.1f);
			memcpy(p, &(tp), 3 * sizeof(float)); p += 3;
			memcpy(p, &(it->color), 3 * sizeof(float)); p += 3;
			//memcpy(p, &(it->normal), 3 * sizeof(float)); p += 3;
			memcpy(p, &(it->uv), 2 * sizeof(float)); p += 2;
		}

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = m_ObjData;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc((UINT)sizeof(float)*vec.size()*VEX_CUSTOM_SIZE, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
			)
		);

		m_indexCount = mesh.indexes.size();
		const std::vector<ObjMesh::DATA_TYPE>& one_indexes = mesh.indexes;
		size_t index_count = one_indexes.size();
		std::vector<ObjMesh::DATA_TYPE>::const_iterator index_it;
		unsigned short* ip = this->m_Indexes;
		int ttCount = 0;
		for (index_it = mesh.indexes.begin(); index_it != mesh.indexes.end(); index_it++){
			std::cout << *index_it;
			ttCount++;
			if (ttCount % 3 == 0) {
				std::cout << std::endl;
			}
			//*ip = *index_it;
			//ip++;
		}
		memcpy(m_Indexes, one_indexes.data(), index_count * sizeof(ObjMesh::DATA_TYPE) );

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = this ->m_Indexes;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(ObjMesh::DATA_TYPE)*index_count, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
			)
		);
	}

	void ESimpleRender::PrepareTexture(){
		// Load the raw texture data from disk and construct a subresource description that references it.
	
	}

#endif
	void ESimpleRender::ReleaseDeviceDependentResources(){
		Sample3DSceneRenderer::ReleaseDeviceDependentResources();
		//if (this->m_ObjData){
		//	delete[] m_ObjData;
		//	m_ObjData = NULL;
		//}
		//if (this->m_Indexes){
		//	delete[] m_Indexes;
		//	m_Indexes = NULL;
		//}
		//if (this->m_Res){
		//	delete m_Res;
		//	m_Res = NULL;
		//}
	}

#if 0 
	void ESimpleRender::Render(){
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!m_loadingComplete)
		{
			return;
		}

		auto context = m_deviceResources->GetD3DDeviceContext();

		// Prepare the constant buffer to send it to the graphics device.
		context->UpdateSubresource(
			m_constantBuffer.Get(),
			0,
			NULL,
			&m_constantBufferData,
			0,
			0
			);

		//material
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetInputLayout(m_inputLayout.Get());

		// Attach our vertex shader.
		context->VSSetShader(
			m_vertexShader.Get(),
			nullptr,
			0
			);

		// Send the constant buffer to the graphics device.
		context->VSSetConstantBuffers(
			0,
			1,
			m_constantBuffer.GetAddressOf()
			);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.Get(),
			nullptr,
			0
			);

#ifdef _RENDER_TEX

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
#endif
		//material end

		// Each vertex is one instance of the VertexPositionColor struct.
		UINT stride = VEX_CUSTOM_SIZE * sizeof(float);
		UINT offset = 0;
		context->IASetVertexBuffers(
			0,
			1,
			m_vertexBuffer.GetAddressOf(),
			&stride,
			&offset
			);

		context->IASetIndexBuffer(
			m_indexBuffer.Get(),
			DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
			0
			);


		// Draw the objects.
		context->DrawIndexed(
			m_indexCount,
			0,
			0
			);
	}
#else
	void ESimpleRender::Render(){
		auto context = m_deviceResources->GetD3DDeviceContext();
		auto dev = m_deviceResources->GetD3DDevice();
		// Prepare the constant buffer to send it to the graphics device.
		if (this->m_Mesh != nullptr && this->m_Material != nullptr){

			this->m_Mesh->CheckLoaded(dev);
			this->m_Material->CheckLoaded(dev);
			if (this->m_Mesh->IsOK() && this->m_Material->IsOK()){
#if 1
				context->UpdateSubresource(
					m_constantBuffer.Get(),
					0,
					NULL,
					&m_constantBufferData,
					0,
					0
					);
				m_Material->SetMaterial(context, m_constantBuffer);
				m_Mesh->SetMesh(context);
#else
				auto context = m_deviceResources->GetD3DDeviceContext();

				// Prepare the constant buffer to send it to the graphics device.
				context->UpdateSubresource(
					m_constantBuffer.Get(),
					0,
					NULL,
					&m_constantBufferData,
					0,
					0
					);

				//material
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				context->IASetInputLayout(this ->m_Material ->m_inputLayout.Get());

				// Attach our vertex shader.
				context->VSSetShader(
					m_Material->m_vertexShader.Get(),
					nullptr,
					0
					);

				// Send the constant buffer to the graphics device.
				context->VSSetConstantBuffers(
					0,
					1,
					m_constantBuffer.GetAddressOf()
					);

				// Attach our pixel shader.
				context->PSSetShader(
					m_pixelShader.Get(),
					nullptr,
					0
					);

#ifdef _RENDER_TEX

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
#endif
				//material end

				// Each vertex is one instance of the VertexPositionColor struct.
				UINT stride = VEX_CUSTOM_SIZE * sizeof(float);
				UINT offset = 0;
				context->IASetVertexBuffers(
					0,
					1,
					m_vertexBuffer.GetAddressOf(),
					&stride,
					&offset
					);

				context->IASetIndexBuffer(
					m_indexBuffer.Get(),
					DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
					0
					);


				// Draw the objects.
				context->DrawIndexed(
					m_indexCount,
					0,
					0
					);
#endif
			}
		}
	}

#endif

}