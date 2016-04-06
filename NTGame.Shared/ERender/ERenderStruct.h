#ifndef __E_RENDER_STRUCT_H__
#define __E_RENDER_STRUCT_H__

#include <DirectXMath.h>
#include <memory>

struct EDefVex{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
	//DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;

	EDefVex(){}
	void init(){
		memset(this, 0, sizeof(EDefVex));
	}
};

#define E_MAX_VEX_NUM 1024
#define E_MAX_FACE_NUM 1024
#define E_INDEX_DATA_TYPE unsigned short

#endif