#include "pch.h"
#include "EObjResource.h"

using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace concurrency;

#include <sstream>

namespace EResource{

	const wchar_t* E_OBJ_RES_KEY_WORD[] = {
		L"#",
		L"mtllib",
		L"o",
		L"v",
		L"usemtl",
		L"s",
		L"f",
		L"vt",
		L"vn",
	};

	enum E_OBJ_KEY_WORD_TYPE{
		EOK_COMMENT,
		EOK_MTLLIB,
		EOK_O,
		EOK_V,
		EOK_USERMTL,
		EOK_S,
		EOK_F,
		EOK_UV,
		EOK_NORMAL,
		EOK_COUNT
	};

	EObjResource::EObjResource(String^ filename)
	{
		this->m_filename = filename;// ref new String("");
		m_file = nullptr;
		m_bFinish = false;
	}

	EObjResource::~EObjResource()
	{
	}

	Concurrency::task<void> EObjResource::ReLoad(){
		m_bFinish = false;
		
		assert(m_filename != nullptr && !m_filename->IsEmpty());
		//{
		//	m_bFinish = true;
		//	return create_task(void);
		//}

		Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
		Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;

		OutputDebugString( installedLocation ->Path ->Data() );

		auto first = create_task(installedLocation->CreateFileAsync(
			this->m_filename,
			CreationCollisionOption::OpenIfExists));
		auto second = first.then(
				[this](StorageFile^ file){
				m_file = file;
				return create_task(FileIO::ReadBufferAsync(file)).then([this, file](task<IBuffer^> task){
					try
					{
						IBuffer^ buffer = task.get();
						if (buffer != nullptr){
							DataReader^ dataReader = DataReader::FromBuffer(buffer);
							String^ fileContent = dataReader->ReadString(buffer->Length);
							if (fileContent != nullptr){
								this->parse(fileContent);
								//OutputDebugString(fileContent->Data());
								//OutputDebugString(this->toString().c_str());
							}
						}
					} catch (COMException^ ex){
						//rootPage->HandleFileNotFoundException(ex);
					}
			});
		});
		return second;
	}

	void EObjResource::parseFloats(const ESubStr& strbuf, float* fs, int count){
		unsigned startIndex = 0;// strbuf.getStartPos();
		unsigned nextIndex = strbuf.findFirstOf(BLANK_LETTERS);

		//ObjVec vec;
		int i = 0;
		//float tempPos[] = { 0.0f, 0.0f, 0.0f };
		while (nextIndex > startIndex && i < 3){
			ESubStr temp = strbuf.subStr(startIndex, nextIndex - startIndex);
			temp.toFloat(fs[i++]);
			//vec.pos = DirectX::XMFLOAT3(tempPos);
			startIndex = strbuf.findFirstNotOf(BLANK_LETTERS, nextIndex);
			nextIndex = strbuf.findFirstOf(BLANK_LETTERS, startIndex);
		}
		//m_vexes.push_back(vec);
	}


	void EObjResource::createVex( const ESubStr& strbuf){
		float fs[] = { .0f, .0f, .0f };
		this->parseFloats(strbuf, fs, 3);
		ObjVec vec;
		vec.pos = DirectX::XMFLOAT3(fs);
		m_vexes.push_back( vec );
	}

	void EObjResource::createMesh(const ESubStr& strbuf){

	}

	void EObjResource::createUV(const ESubStr& strbuf){
		float uv[] = { .0f, .0f };
		this->parseFloats( strbuf, uv, 2 );
		this ->m_uvs.push_back(DirectX::XMFLOAT2( uv ));
	}

	void EObjResource::createNormal(const ESubStr& strbuf){
		float normal[] = { .0f, .0f, .0f };
		this->parseFloats(strbuf, normal, 2);
		this ->m_normals.push_back( DirectX::XMFLOAT3( normal ) );
	}

	void EObjResource::createFace(const ESubStr& strbuf){
		if (m_meshes.empty()){
			m_meshes.push_back(ObjMesh());
		}
		ObjMesh& cur = m_meshes[m_meshes.size() - 1];
		unsigned startIndex = 0;
		unsigned nextIndex = strbuf.findFirstOf(BLANK_LETTERS);
		unsigned i = 0;
		int faces[4];
		memset(faces, 0, sizeof(int) * 4);
		while (nextIndex > startIndex  && i < 4){
			ESubStr temp = strbuf.subStr(startIndex, nextIndex);
			//创建顶点
			//ObjVec vec;
			//vec.init();
			int vIndex = 0;
			temp.toInt( vIndex );

			//vec.pos = this->m_vexes[vIndex-1];
			ObjVec& vec = this->m_vexes[vIndex - 1];
			//startIndex = strbuf.findFirstNotOf(BLANK_LETTERS, nextIndex);

			// uv index
			if (strbuf[startIndex] == L'/') {
				int tt = strbuf.getIntFrom( startIndex + 1, startIndex );
				tt--;
				vec.uv = this->m_uvs[tt];
			}
			// normal index 
			if (strbuf[startIndex] == L'/') {
				int tt = strbuf.getIntFrom( startIndex + 1, startIndex);
				tt--;
				vec.normal = this->m_normals[tt];
			}
			//int fIndex = cur.vertexes.size();
			faces[i++] = vIndex; // fIndex;
			//cur.vertexes.push_back(vec);
			
			
			nextIndex++;
			if (nextIndex > strbuf.getLen()) break;
			startIndex = nextIndex;
			nextIndex = strbuf.findFirstOf(BLANK_LETTERS, startIndex);
			//startIndex = nextIndex + 1;
			//if (startIndex > strbuf.getLen()) break;
			//nextIndex = strbuf.findFirstOf(BLANK_LETTERS, startIndex);
			if (nextIndex > strbuf.getLen()) break;
		}
		cur.addFace(faces[2], faces[1], faces[0]);
		//有可能只有三个顶点
		if ( i > 2 ) 
			cur.addFace(faces[0], faces[3], faces[2]);
	}

	ESubStr EObjResource::getNextNotBlankStr(const ESubStr& str, unsigned from ){
		if (from>str.getLen()) from = str.getLen();
		int start = str.findFirstNotOf(BLANK_LETTERS, from);
		return str.subStr(start);
	}

	void EObjResource::parseLine(const ESubStr& str){
		unsigned firstLetterIndex = str.findFirstNotOf( BLANK_LETTERS );
		wchar_t c = str[firstLetterIndex];
		ESubStr word;
		if (ESubStr::isAlpha(c)){
			word = str.getWord(firstLetterIndex);
		} else {
			word = str.subStr(firstLetterIndex, 1);
		}
		int wordType = -1;
		for (int i=0; i < EOK_COUNT; i++){
			if (word == E_OBJ_RES_KEY_WORD[i]){
				wordType = i;
				break;
			}
		}

		//create vex
		ESubStr followStr = getNextNotBlankStr(str, word.getLen());
		switch (wordType){
			case EOK_V:{
				createVex( followStr);
			}break;
			case EOK_F:{
				createFace(followStr);
			}break;
			case EOK_UV:{
				createUV( followStr );
			}break;
			case EOK_NORMAL:{
				createNormal(followStr );
			}break;
		}
		
	}

	void EObjResource::parse(String^ str){
		if (str == nullptr || str->IsEmpty()){
			return;
		}
		ESubStr strbuf(str);
		ESubStr line = strbuf.getLine();
		while (!line.empty()){
			parseLine(line);
			line = strbuf.getLine(line.getEndPos() + 1);  
		}
		//清空一些临时的buff
		//this->m_vexes.clear();
		//this->m_uvs.clear();
		//this->m_normals.clear();
	}

	std::wstring EObjResource::toString(){
		std::wstringstream strbuf;
		//size_t vexCount = m_vexes.size();
		//for (size_t i = 0; i < vexCount; i++){
		//	DirectX::XMFLOAT3& vec = m_vexes[i];
		//	strbuf << L"v " << vec.x << L' '
		//		<< vec.y << L' ' << vec.z << L'\n';
		//}

		size_t faceCount = m_meshes.size();
		for (size_t i = 0; i < faceCount; i++){
			ObjMesh& mesh = m_meshes[i];
			size_t indexCount = mesh.indexes.size();
			if (indexCount >= 3){
				//vex
				for (size_t j = 0; j < mesh.vertexes.size(); j++){
					strbuf << mesh.vertexes[j].pos.x
						<< L' ' << mesh.vertexes[j].pos.y
						<< L' ' << mesh.vertexes[j].pos.z << L'\n';
				}
				//face
				for (size_t j = 0; j < indexCount; j += 3){
					strbuf << 
						L"f " << mesh.indexes[j]
						<< L' '<< mesh.indexes[j+1] 
						<< L' '<< mesh.indexes[j+2]<<L'\n';
				}
			}
		}
		return strbuf.str();
	}

	const std::vector<ObjVec>& EObjResource::getAllVexes() const{
		return m_vexes; 
	}
	
	const std::vector<ObjMesh>& EObjResource::getAllMeshes() const{
		return m_meshes; 
	}

	int EObjResource::getFaceCount() const{
		size_t count = m_meshes.size();
		int t = 0;
		for (size_t i = 0; i < count; i++){
			t += m_meshes[i].getFaceCount();
		}
		return t;
	}

}
