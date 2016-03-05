#ifndef __E_QUICK_TEST_H__
#define __E_QUICK_TEST_H__

#ifdef E_QUICK_TEST

#include <vector>

class IETestBase{
public:
	virtual ~IETestBase(){};
	virtual void init(){};
	virtual void deInit(){};

protected:
	static std::vector<IETestBase*> g_testObjcts;
};

#define ET_BEGIN_TEST() \
#define ET_REGIST_FUN( testfun )\


#endif

#endif