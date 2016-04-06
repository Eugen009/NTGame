#ifndef __E_RESMGR_H__
#define __E_RESMGR_H__

#include <hash_map>
#include <Resource/EResBase.h>

class EResMgr {
public:
	
	struct ResData{
	public:
		int m_iRefCount;
		std::vector<byte> m_Data;
		std::shared_ptr<EResBase> m_Res;
		bool m_bLoaded;
		Concurrency::task<std::vector<byte>> m_LoadTask;
		ResData(){
			m_iRefCount = 0;
			m_bLoaded = false;
			m_Res = nullptr;
		}
	};

public:
	EResMgr();
	void LoadRes( const std::wstring& filepath );
	void UnLoadRes( const std::wstring& filepath );
	bool IsResLoaded(const std::wstring& filepath);
	const std::vector<byte>& GetResData(const std::wstring& filepath);
	const std::shared_ptr<EResBase> GetRes(const std::wstring& filepath) const;
	void OnInit();
	void RegisterType();

	static EResMgr& GetInstance();
	//void CheckResLoading();

protected:
	void OnLoadFinished(ResData& data, const std::wstring& filepath, const EDataBytes& bytes);

public:
	std::hash_map<std::wstring, ResData> m_DataBytes;
	typedef std::hash_map<std::wstring, ResData>::iterator DataIterator;
	typedef std::hash_map<std::wstring, ResData>::const_iterator ConstDataIterator;
	
	typedef std::shared_ptr<EResBase>(*RES_BUILDER_FUN) (const EDataBytes& bytes);
	typedef std::hash_map<std::wstring, RES_BUILDER_FUN> RegisterResMap;
	RegisterResMap m_RegisterResTypes;

protected:
	bool m_bInit;
};

#endif