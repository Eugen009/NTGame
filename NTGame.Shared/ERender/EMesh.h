#ifndef __E_MESH_H__
#define __E_MESH_H__

#include <Resource\EResBase.h>

class EMesh {
public:
	EMesh( const std::wstring& meshRes );
	void Destroy();
	void Prepare();

	void SetMesh(ID3D11DeviceContext* context);
	bool IsOK();
	bool CheckLoaded(ID3D11Device* dev);

	static void ClearMeshBuffer();
	static void CreateMeshBuffer();

protected:
	void CreateMeshBuffer(ID3D11Device* dev, const std::shared_ptr<EResBase> res );

protected:
	std::wstring m_MeshFilename;

	static float* g_ObjData; //建一个足够大的内存块
	static float* GetObjData();
	static E_INDEX_DATA_TYPE* g_iIndex;
	static E_INDEX_DATA_TYPE* GetIndexData();

public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	UINT m_indexCount;

};

#endif