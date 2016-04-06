#ifndef __E_PATH_HELPER_H__
#define __E_PATH_HELPER_H__

#define SAFE_DELETE(obj) if( (obj) != nullptr ) {delete (obj); (obj) = nullptr;}
#define SAFE_DELETE_ARRAY(obj) if( (obj) != nullptr ){delete[] (obj); (obj) = nullptr;}


#endif