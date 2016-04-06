#ifndef __E_MESHRES_H__
#define __E_MESHRES_H__

#include <Resource/EResType.h>
#include <Resource/EResBase.h>
#include <vector>
#include <ERender\ERenderStruct.h>

class EMeshRes: public EResBase{
public:
	struct ObjVec{

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;

		ObjVec(){}
		void init(){
			memset(this, 0, sizeof(ObjVec));
		}
	};

	struct ObjMesh{
		//typedef unsigned short DATA_TYPE;
		std::vector<E_INDEX_DATA_TYPE> indexes;

		std::vector<ObjVec> vertexes;
		void addFace(E_INDEX_DATA_TYPE a, E_INDEX_DATA_TYPE b, E_INDEX_DATA_TYPE c){
			indexes.push_back(a);
			indexes.push_back(b);
			indexes.push_back(c);
		}

		int getFaceCount() const{
			return (int)indexes.size() / 3;
		}
	};

	typedef unsigned short DATA_TYPE;
	typedef std::vector<ObjMesh> MeshLst;
	virtual ~EMeshRes();
	virtual EResType GetResType() const{
		return EResType::MESH;
	}

	virtual const MeshLst* GetAllMesh() const{ return nullptr; };

};

#endif