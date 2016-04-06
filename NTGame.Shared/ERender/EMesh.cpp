#include "pch.h"
#include "Content\ShaderStructures.h"
#include "common\StepTimer.h"
#include <sstream>
#include "../Common/DirectXHelper.h"
#include "../Resource/EResMgr.h"
#include "../EObjResource.h"
#include "../Resource/EMeshRes.h"
#include "ERenderStruct.h"
#include "EMesh.h"
#include "../Tools/EMemHelper.h"

EMesh::EMesh(const std::wstring& meshRes) :
	m_indexCount( 0 ),
	m_MeshFilename( meshRes )
{
}

void EMesh::Prepare(){
	EResMgr::GetInstance().LoadRes( this->m_MeshFilename );
}

void EMesh::SetMesh(ID3D11DeviceContext* context){
	if (this->m_vertexBuffer != nullptr && this->m_indexBuffer != nullptr){
		UINT stride = sizeof(EDefVex);
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
}

float* EMesh::g_ObjData = nullptr;

void EMesh::ClearMeshBuffer(){
	SAFE_DELETE_ARRAY( g_ObjData );
	SAFE_DELETE_ARRAY(g_iIndex);
}

void EMesh::CreateMeshBuffer(){
	if (g_ObjData == nullptr){
		size_t s = sizeof(EDefVex) / sizeof(float);
		g_ObjData = new float[s * E_MAX_VEX_NUM];//1MµÄ¶¥µãÄÚ´æ
	}
	if (g_iIndex == nullptr){
		g_iIndex = new E_INDEX_DATA_TYPE[3 * E_MAX_FACE_NUM];
	}
}


float* EMesh::GetObjData(){
	return g_ObjData;
}

E_INDEX_DATA_TYPE* EMesh::g_iIndex = nullptr;
E_INDEX_DATA_TYPE* EMesh::GetIndexData(){
	return g_iIndex;
}

bool EMesh::IsOK(){
	return (this->m_vertexBuffer != nullptr && this->m_indexBuffer != nullptr);
}

bool EMesh::CheckLoaded(ID3D11Device* dev){
	if (this->m_vertexBuffer != nullptr && this->m_indexBuffer != nullptr) return true;
	if (EResMgr::GetInstance().IsResLoaded(this->m_MeshFilename)){
		this->CreateMeshBuffer(dev, EResMgr::GetInstance().GetRes(this->m_MeshFilename));
	}
}

#define SCALE_VEC( v, s ) (v).x*=s;(v).y*=s;(v).z*=s;

void EMesh::CreateMeshBuffer(ID3D11Device* dev, const std::shared_ptr<EResBase> res){
	if (res == nullptr || res->GetResType() != EResType::MESH)
		return;

	EMeshRes* meshRes = (EMeshRes*)res.get();

	const EMeshRes::MeshLst* meshes = meshRes->GetAllMesh();

	if (meshes == nullptr) return;

	const EMeshRes::ObjMesh& mesh = meshes ->at(0);

	const std::vector<EMeshRes::ObjVec>&  vec = mesh.vertexes;
	if (vec.size() == 0) return;
	std::vector<EMeshRes::ObjVec>::const_iterator it;
	float* p = EMesh::GetObjData();
	assert(E_MAX_VEX_NUM > vec.size());
	for (it = vec.begin(); it != vec.end(); it++){
		DirectX::XMFLOAT3 tp = it->pos;
		SCALE_VEC(tp, 0.1f);
		memcpy(p, &(tp), 3 * sizeof(float)); p += 3;
		memcpy(p, &(it->color), 3 * sizeof(float)); p += 3;
		//memcpy(p, &(it->normal), 3 * sizeof(float)); p += 3;
		memcpy(p, &(it->uv), 2 * sizeof(float)); p += 2;
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = GetObjData();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc((UINT)vec.size()*sizeof(EDefVex), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		dev ->CreateBuffer(
		&vertexBufferDesc,
		&vertexBufferData,
		&m_vertexBuffer
		)
		);

	m_indexCount = mesh.indexes.size();
	const std::vector<E_INDEX_DATA_TYPE>& one_indexes = mesh.indexes;
	UINT index_count = (UINT)one_indexes.size();
	//std::vector<E_INDEX_DATA_TYPE>::const_iterator index_it;
	unsigned short* ip = GetIndexData();
	//int ttCount = 0;
	memcpy(ip, one_indexes.data(), index_count * sizeof(E_INDEX_DATA_TYPE));

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = GetIndexData();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(E_INDEX_DATA_TYPE)*index_count, D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		dev->CreateBuffer(
		&indexBufferDesc,
		&indexBufferData,
		&m_indexBuffer
		)
	);
}