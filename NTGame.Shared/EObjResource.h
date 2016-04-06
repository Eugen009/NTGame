#pragma once

using namespace Windows::Storage;
using namespace Platform;
#include "ESubStr.h"
#include <string>
#include <Resource\EMeshRes.h>

namespace EResource{

	class EObjResource:public EMeshRes
	{
	public:
		virtual EResType GetResType() const{
			return EResType::MESH;
		}

	public:
		EObjResource(String^ filename);
		EObjResource();
		~EObjResource();

		Concurrency::task<void> ReLoad();
		bool IsFinish() const{ return m_bFinish; }
		void parse(std::shared_ptr<std::wstring> str);
		const std::vector<ObjVec>& getAllVexes() const;
		const std::vector<ObjMesh>& getAllMeshes() const;
		int getFaceCount() const;
		std::wstring toString();
		virtual const std::vector<ObjMesh>* GetAllMesh() const{ return &m_meshes; };
		int getMeshCount() const{ return (int)m_meshes.size(); }
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
		
		std::vector<ObjVec> vertexes;
		std::vector<ObjMesh> m_meshes;

		bool m_bFinish;
	};

}


