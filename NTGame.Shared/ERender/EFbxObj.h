#ifndef _EFBX_H_
#include <string>
namespace FBXSDK_NAMESPACE{
	class FbxManager;
	class FbxScene;
}

namespace EResource{
	class EFbxObj{
	public:
		EFbxObj(const char* filename);
		~EFbxObj();
		void DoTest();
		void exportToVexBuf(float* buf, int size);
	
	protected:
		FBXSDK_NAMESPACE::FbxScene* LoadScene( const char* filename );

	protected:
		//FBXSDK_NAMESPACE::FbxGeometry* m_pFbxGeometry;
		FBXSDK_NAMESPACE::FbxManager* m_pFbxMgr;
		FBXSDK_NAMESPACE::FbxManager* m_pScene;
		std::string m_Filename;
	};
}

#endif