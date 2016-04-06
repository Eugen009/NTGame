#include <pch.h>
#include <memory>
#include <Resource/EResBase.h>
#include <Resource/EResType.h>
#include "EResLoaders.h"
#include "EObjResource.h"

std::shared_ptr<EResBase> LoadObjData(const EDataBytes& bytes){
	EResource::EObjResource* res = new EResource::EObjResource();
	
	size_t count = bytes.size();
	//str->resize( count + 1 );
	wchar_t* pstr = new wchar_t[count + 1];
	wchar_t* p = pstr;
	for (int i = 0; i < count; i++){
		*p = (wchar_t)bytes.at(i);
		p++;
	}
	*p = L'\0';
	std::shared_ptr<std::wstring> str(new std::wstring(pstr));
	delete[] pstr;
	pstr = nullptr;
	p = nullptr;
	res->parse( str );
	std::shared_ptr<EResBase> resptr( res );
	return resptr;
}