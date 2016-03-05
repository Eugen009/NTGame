#pragma once

using namespace Windows::Storage;
using namespace Platform;
#include "ESubStr.h"
#include <string>
namespace EResource{

	struct ObjVec{

			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 color;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 uv;

			ObjVec(){}
			void init(){
				memset( this, 0, sizeof(ObjVec) );
			}
	};

	struct FaceData{
		int posId;
		int uvId;
		int normalId;
	};

	struct ObjMesh{
		typedef unsigned short DATA_TYPE;
		std::vector<DATA_TYPE> indexes;
		//public std::vector<int> uvIndex;
		//public std::vector<int> normalIndex;
		
		std::vector<ObjVec> vertexes;
		void addFace(DATA_TYPE a, DATA_TYPE b, DATA_TYPE c){
			indexes.push_back(a);
			indexes.push_back(b);
			indexes.push_back(c);
		}

		int getFaceCount() const{
			return (int)indexes.size() / 3;
		}
	};

	class EObjResource
	{
	public:


	public:
		EObjResource(String^ filename);
		~EObjResource();

		Concurrency::task<void> ReLoad();
		bool IsFinish() const{ return m_bFinish; }
		void parse(String^ str);
		const std::vector<ObjVec>& getAllVexes() const;
		const std::vector<ObjMesh>& getAllMeshes() const;
		int getFaceCount() const;
		std::wstring toString();
		int getMeshCount() const{ return m_meshes.size(); }
		const ObjMesh& getMeshAt(int index){ return this->m_meshes[index]; }

	protected:
		void createVex(const ESubStr& strbuf);
		void createMesh(const ESubStr& strbuf);
		void createUV( const ESubStr& strbuf );
		void createNormal(const ESubStr& strbuf );
		void parseLine(const ESubStr& str);
		void createFace(const ESubStr& strbuf);

		void parseFloats(const ESubStr& strbuf, float* fs, int count);

		ESubStr getNextNotBlankStr(const ESubStr& str, unsigned from);

	protected:
		String^ m_filename;
		StorageFile^ m_file;

		std::vector<ObjVec> m_vexes;
		std::vector<DirectX::XMFLOAT2> m_uvs;
		std::vector<DirectX::XMFLOAT3> m_normals;
		std::vector<ObjMesh> m_meshes;

		bool m_bFinish;
	};

}

