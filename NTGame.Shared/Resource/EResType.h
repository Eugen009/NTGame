#ifndef __E_RESTYPE_H__
#define __E_RESTYPE_H__

#include <vector>

enum EResType{
	NONE,
	MESH,
	SHADER,
	TEXTURE
};

typedef std::vector<byte> EDataBytes;

#endif