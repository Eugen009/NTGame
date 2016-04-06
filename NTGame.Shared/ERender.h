#ifndef __E_RENDER_H__
#define __E_RENDER_H__

#include "Content/Sample3DSceneRenderer.h"
#include "EObjResource.h"

class EMesh;
class EMaterial;

namespace ERender{
	class ESimpleRender : public NTGame::Sample3DSceneRenderer{
	public:
		static int VEX_CUSTOM_SIZE;// = 3 + 3;// +3 + 2;
		static const int MAX_VEX_NUM = 1024;
		static const int MAX_FACE_NUM = 20000;
	public:
		ESimpleRender(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~ESimpleRender();
		virtual void CreateDeviceDependentResources();
		virtual void ReleaseDeviceDependentResources();
		void renderMesh(EResource::EObjResource* mesh);
		void PrepareTexture();
		void Render();
	protected:
		//float* m_ObjData;
		//unsigned short* m_Indexes;
		//EResource::EObjResource* m_Res;
		std::shared_ptr<EMesh> m_Mesh;
		std::shared_ptr<EMaterial> m_Material;
		bool m_bRenderTex;



	};
}

#endif