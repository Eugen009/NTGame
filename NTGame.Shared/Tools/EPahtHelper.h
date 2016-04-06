#ifndef __E_PATH_HELPER_H__
#define __E_PATH_HELPER_H__

#include <sstream>
#include <iostream>

namespace EPathHelper{
	inline std::wstring GetFilename( const std::wstring& path ) {
		std::wstring res;
		size_t pos = path.find_last_of('/');
		if (pos == std::wstring::npos){
			pos = 0;
		} else pos++;
		if (pos < path.size()){
			res = path.substr( pos, path.size() - pos );
		}
		return res;
	}

	inline std::wstring GetExtName(const std::wstring& path) {
		std::wstring res;
		size_t pos = path.find_last_of('.');
		if (pos == std::wstring::npos){
			return res;
		} else pos++;
		if (pos < path.size()){
			res = path.substr(pos, path.size() - pos);
		}
		return res;
	}
};


#endif