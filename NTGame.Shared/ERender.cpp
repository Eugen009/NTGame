#include "pch.h"
#include "Content\ShaderStructures.h"
#include "common\StepTimer.h"
#include "ERender.h"
#include "Common\DirectXHelper.h"
#include "EObjResource.h"
#include "ERender\EFbxObj.h"

using namespace EResource;

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
		:Sample3DSceneRenderer(deviceResources){}

	ESimpleRender::~ESimpleRender(){
		if (this->m_ObjData){
			delete[] m_ObjData;
			m_ObjData = NULL;
		}
		if (this->m_Indexes){
			delete[] m_Indexes;
			m_Indexes = NULL;
		}
		if (this->m_Res){
			delete m_Res;
			m_Res = NULL;
		}
	}

	void ESimpleRender::CreateDeviceDependentResources(){

		// prepare the memory
		if (m_ObjData == NULL){
			m_ObjData = new float[MAX_VEX_NUM * VEX_CUSTOM_SIZE];
		}

		if (this->m_Indexes == NULL){
			m_Indexes = new unsigned short[MAX_FACE_NUM * 3];
		}

		// begin to loading something

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
				&m_vertexShader
				)
				);

			static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

		m_Res = new EObjResource(ref new String(L"box.obj"));
		auto resTask = m_Res->ReLoad();

		(resTask && createVSTask && createPSTask).then([this]() {
			this->renderMesh( m_Res );
			m_loadingComplete = true;
		});

		//EFbxObj testObj( "testbox.fbx" );
		//testObj.DoTest();


	}

#if 0

	void ESimpleRender::renderMesh(EResource::EObjResource* obj){
		// Load mesh vertices. Each vertex has a position and a color.
		using namespace DirectX;

		static const NTGame::VertexPositionColor cubeVertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
		};

		float* p = m_ObjData;
		int totalVexSize = sizeof(float) * VEX_CUSTOM_SIZE * 8;
		memcpy(p, cubeVertices, totalVexSize);
		//assert(MAX_VEX_NUM > vec.size());
		//for (it = vec.begin(); it != vec.end(); it++){
			//memcpy(p, &(it->pos), 3 * sizeof(float)); p += 3;
			//memcpy(p, &(it->color), 3 * sizeof(float)); p += 3;
			//memcpy(p, &(it->normal), 3 * sizeof(float)); p += 3;
			//memcpy(p, &(it->uv), 2 * sizeof(float)); p += 2;
		//}


		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = m_ObjData;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc( totalVexSize, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
			)
			);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{
			0, 2, 1, // -x
			1, 2, 3,

			4, 5, 6, // +x
			5, 7, 6,

			0, 1, 5, // -y
			0, 5, 4,

			2, 6, 7, // +y
			2, 7, 3,

			0, 4, 6, // -z
			0, 6, 2,

			1, 3, 7, // +z
			1, 7, 5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		int totalIndexSize = m_indexCount * sizeof(ObjMesh::DATA_TYPE);
		memcpy(m_Indexes, cubeIndices, m_indexCount * sizeof(ObjMesh::DATA_TYPE));

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };

		indexBufferData.pSysMem = m_Indexes;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc( totalIndexSize, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
			)
			);
	
	}
#else
	void ESimpleRender::renderMesh(EResource::EObjResource* obj){
		if (m_ObjData == NULL || m_Indexes == NULL || obj ->getMeshCount() == 0 ) return;

		//const std::vector<ObjMesh>& meshes = mesh ->getAllMeshes();
		const ObjMesh& mesh = obj->getMeshAt(0);
		const std::vector<ObjVec>&  vec = obj->getAllVexes();// mesh.vertexes;
		if (vec.size()== 0) return;
		std::vector<ObjVec>::const_iterator it;
		float* p = m_ObjData;
		assert(MAX_VEX_NUM > vec.size());
		for (it = vec.begin(); it != vec.end(); it++){
			memcpy(p, &(it ->pos), 3 * sizeof(float));p += 3;
			memcpy(p, &(it->color), 3 * sizeof(float)); p += 3;
			//memcpy(p, &(it->normal), 3 * sizeof(float)); p += 3;
			//memcpy(p, &(it->uv), 2 * sizeof(float)); p += 2;
		}

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = m_ObjData;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(float)*vec.size()*VEX_CUSTOM_SIZE, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
			)
		);

		m_indexCount = mesh.indexes.size();
		const std::vector<ObjMesh::DATA_TYPE>& one_indexes = mesh.indexes;
		int index_count = one_indexes.size();
		//std::vector<ObjMesh::DATA_TYPE>::const_iterator index_it;
		//unsigned short* ip = this->m_Indexes;
		//for (index_it = mesh.indexes.begin(); index_it != mesh.indexes.end(); index_it++){
		//	*ip = *index_it;
		//	ip++;
		//}
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

#endif

	void ESimpleRender::ReleaseDeviceDependentResources(){
		Sample3DSceneRenderer::ReleaseDeviceDependentResources();
		if (this->m_ObjData){
			delete[] m_ObjData;
			m_ObjData = NULL;
		}
		if (this->m_Indexes){
			delete[] m_Indexes;
			m_Indexes = NULL;
		}
		if (this->m_Res){
			delete m_Res;
			m_Res = NULL;
		}
	}

}