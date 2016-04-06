#ifndef __E_RESBASE_H__
#define __E_RESBASE_H__

#include <Resource/EResType.h>

class EResBase {
public:
	virtual ~EResBase();
	virtual EResType GetResType() const{
		return EResType::NONE;
	}
};

#endif