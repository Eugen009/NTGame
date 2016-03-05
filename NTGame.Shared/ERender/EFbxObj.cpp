#include "pch.h"
#include <fbxsdk.h>
#include "EFbxObj.h"

#include <sstream>



#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

using namespace FBXSDK_NAMESPACE;

namespace EResource{
	int numTabs = 0;
	//FbxManager* gFbxManager = nullptr;
	//char msgBuf[1024];

	FbxScene* EFbxObj::LoadScene(const char* filename){
		if (m_pFbxMgr == nullptr)
			m_pFbxMgr = FbxManager::Create();
		FbxIOSettings* ios = FbxIOSettings::Create(m_pFbxMgr, IOSROOT);
		m_pFbxMgr->SetIOSettings(ios);
		FbxImporter* import = FbxImporter::Create(m_pFbxMgr, "");

		if (!import->Initialize(filename, -1, m_pFbxMgr->GetIOSettings())){
			return nullptr;
		}
		FbxScene* scene = FbxScene::Create(m_pFbxMgr, "RootScene");
		import->Import(scene);
		import->Destroy();
		return scene;
	}

	void PrintTabs() {
		for (int i = 0; i < numTabs; i++)
			printf("\t");
	}

	void PrintNode(FbxNode* pNode) {
		PrintTabs();
		const char* nodeName = pNode->GetName();
		FbxDouble3 translation = pNode->LclTranslation.Get();
		FbxDouble3 rotation = pNode->LclRotation.Get();
		FbxDouble3 scaling = pNode->LclScaling.Get();

		// Print the contents of the node.
		
		//wsprintf(msgBuf, "<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
		//	nodeName,
		//	translation[0], translation[1], translation[2],
		//	rotation[0], rotation[1], rotation[2],
		//	scaling[0], scaling[1], scaling[2]
		//	);
		std::wstringstream in;
		//in << L"<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n" <<
		in << nodeName << L":"<<
			translation[0] << translation[1] << translation[2] <<
			rotation[0] << rotation[1] << rotation[2] <<
			scaling[0] << scaling[1] << scaling[2];
		//std::wstring temp = msgBuf;
		in.flush();
		OutputDebugString( in.str().c_str() );
		numTabs++;

		// Print the node's attributes.
		//for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
		//	PrintAttribute(pNode->GetNodeAttributeByIndex(i));

		// Recursively print the children.
		for (int j = 0; j < pNode->GetChildCount(); j++)
			PrintNode(pNode->GetChild(j));

		numTabs--;
		PrintTabs();
		printf("</node>\n");
	}

	void PrintAllMesh(FbxNode* node){
		std::wstringstream strbuf;
		for (int i = 0; i < node->GetNodeAttributeCount(); i++){
			FbxNodeAttribute* attr = node->GetNodeAttributeByIndex( i );
			if (attr->GetAttributeType() == FbxNodeAttribute::eMesh){
				FbxMesh* mesh = (FbxMesh*)attr;
				int i, ctrlPointsCount = mesh->GetControlPointsCount();
				FbxVector4* ctrlPoints = mesh->GetControlPoints();
				for (i = 0; i < ctrlPointsCount; i++){
					//test only
					strbuf << ctrlPoints[i][0] <<", " << ctrlPoints[i][1]<<", " << ctrlPoints[i][2] << std::endl;
				}
			}
		}
		OutputDebugString(strbuf.str().c_str());

		for (int j = 0; j < node->GetChildCount(); j++){
			PrintAllMesh(node);
		}
	}

	void TestPrintAllNodes(FbxScene* scene){
		FbxNode* lRootNode = scene->GetRootNode();
		if (lRootNode) {
			for (int i = 0; i < lRootNode->GetChildCount(); i++)
				PrintNode(lRootNode->GetChild(i));
		}
	}

	EFbxObj::EFbxObj(const char* filename){
		this->m_Filename = filename;
		m_pFbxMgr = nullptr;
	}

	EFbxObj::~EFbxObj(){
		if (m_pFbxMgr){
			m_pFbxMgr->Destroy();
		}
		this->m_pScene = nullptr;
	}

	void EFbxObj::DoTest(){
		FbxScene* scene = LoadScene(this->m_Filename.c_str());
		if (scene){
			//TestPrintAllNodes(scene);
			FbxNode* lRootNode = scene->GetRootNode();
			if (lRootNode) {
				for (int i = 0; i < lRootNode->GetChildCount(); i++)
					PrintAllMesh(lRootNode->GetChild(i));
					//PrintNode(lRootNode->GetChild(i));
			}
		}
	}

	template<class T>
	int GetIdByMode(FbxLayerElement* pelem, int cpi, int vid){
		FbxLayerElementTemplate<T>* elem = (FbxLayerElementTemplate<T>*)pelem;
		int id = -1;
		switch ( elem ->GetMappingMode()){
		default:
			break;
		case FbxGeometryElement::eByControlPoint:
			switch (elem->GetReferenceMode()){
			case FbxGeometryElement::eDirect:
				id = cpi;
				//DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
				break;
			case FbxGeometryElement::eIndexToDirect:{
				id = elem ->GetIndexArray().GetAt(cpi);
				//DisplayColor(header,color ->GetDirectArray().GetAt(id));
			}
				break;
			default:
				break; // other reference modes not shown here!
			}
			break;
		case FbxGeometryElement::eByPolygonVertex:{
			switch (elem->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				id = vid;
				//DisplayColor(header, color->GetDirectArray().GetAt(vertexId));
				break;
			case FbxGeometryElement::eIndexToDirect:{
				int id = elem ->GetIndexArray().GetAt(vid);
				//DisplayColor(header, color->GetDirectArray().GetAt(id));
			}
				break;
			default:
				break; // other reference modes not shown here!
			}
		}
			break;

		case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
		case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
		case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
			break;
		}
		return id;
	}

	FbxColor GetPolygonVertexColorAt(FbxMesh* mesh, int cpi, int vid){
		FbxColor out_color;
		for (int i = 0; i < mesh->GetElementVertexColorCount(); i++){
			//FbxGeometryElementVertexColor 
			FbxGeometryElementVertexColor* color = mesh->GetElementVertexColor(i);
			if (color){
				int id = GetIdByMode<FbxGeometryElementVertexColor::ArrayElementType>(color, cpi, vid);
				if (id > 0){
					out_color = color->GetDirectArray().GetAt(id);
					//return true;
				}
			}
		}
		return out_color;
	}

	FbxVector2 GetPolygonUVAt(FbxMesh* mesh, int cpi, int vid){
		FbxVector2 out_uv;
		for (int i = 0; i < mesh->GetElementUVCount(); i++){
			FbxGeometryElementUV* uv = mesh->GetElementUV(i);
			if (uv){
				int id = GetIdByMode<FbxGeometryElementUV::ArrayElementType>(uv, cpi, vid);
				if (id > 0){
					out_uv = uv->GetDirectArray().GetAt(id);
				}
			}
		}
		return out_uv;
	}

	FbxVector4 GetPolygonNormalAt(FbxMesh* mesh, int cpi, int vid){
		FbxVector4 out_normal;
		for (int i = 0; i < mesh->GetElementNormalCount(); i++){
			FbxGeometryElementNormal* normal = mesh->GetElementNormal(i);
			if (normal){
				int id = GetIdByMode<FbxGeometryElementNormal::ArrayElementType>(normal, cpi, vid);
				if (id > 0){
					out_normal = normal->GetDirectArray().GetAt(id);
				}
			}
		}
		return out_normal;
	}

	FbxVector4 GetPolygonTangentAt(FbxMesh* mesh, int cpi, int vid){
		FbxVector4 out_tangent;
		for (int i = 0; i < mesh->GetElementTangentCount(); i++){
			FbxGeometryElementTangent* tangent = mesh->GetElementTangent(i);// GetElementTangent(i);
			if (tangent){
				int id = GetIdByMode<FbxGeometryElementTangent::ArrayElementType>(tangent, cpi, vid);
				if (id > 0){
					out_tangent = tangent->GetDirectArray().GetAt(id);
				}
			}
		}
		return out_tangent;
	}

	bool GetPolygonBinormalAt(FbxMesh* mesh, int cpi, int vid, FbxGeometryElementBinormal::ArrayElementType& out_binormal){
		//FbxVector4 out_binormal;
		for (int i = 0; i < mesh->GetElementBinormalCount(); i++){
			FbxGeometryElementBinormal* binormal = mesh->GetElementBinormal(i);// GetElementTangent(i);
			if (binormal){
				int id = GetIdByMode<FbxGeometryElementBinormal::ArrayElementType>(binormal, cpi, vid);
				if (id > 0){
					out_binormal = binormal->GetDirectArray().GetAt(id);
					//return true;
				}
			}
		}
		return out_binormal;
	}

	void exportToVexBuf( FbxMesh* mesh, float* vexBuf, int& vexSize, unsigned short* indexes, int& indexSize ){
		int i, ctrlPointsCount = mesh->GetControlPointsCount();
		assert(vexSize > ctrlPointsCount);
		FbxVector4* ctrlPoints = mesh->GetControlPoints();
		for (i = 0; i < ctrlPointsCount; i++){
			*vexBuf = ctrlPoints[i][0]; vexBuf++;
			*vexBuf = ctrlPoints[i][1]; vexBuf++;
			*vexBuf = ctrlPoints[i][2]; vexBuf++;
		}
		int polygonCount = mesh->GetPolygonCount();
		int vid = 0;
		for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++){
			int polygonSize = mesh->GetPolygonSize(polygonIndex);
			for (int j = 0; j < polygonSize; j++){
				int cpi = mesh->GetPolygonVertex(polygonIndex, j);
				//FbxVector4 
				FbxColor color = GetPolygonVertexColorAt(mesh, cpi, vid);
				FbxVector4 normal = GetPolygonNormalAt(mesh, cpi, vid);
				
			}
		}
	}

	void EFbxObj::exportToVexBuf(float* buf, int size){

	}





}