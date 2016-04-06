#include "pch.h"
#include "Content\ShaderStructures.h"
#include "common\StepTimer.h"
#include <sstream>
#include "../Common/DirectXHelper.h"
#include <hash_map>
#include "EResMgr.h"
#include "../Tools/EPahtHelper.h"
#include "EObjResource.h"
#include "EResLoaders.h"

EResMgr::EResMgr():
	m_bInit(false)
{
}

void EResMgr::OnInit(){
	m_bInit = true;
	m_RegisterResTypes[L"obj"] = &LoadObjData;
}

void EResMgr::LoadRes(const std::wstring& filepath){
	DataIterator it = this->m_DataBytes.find(filepath);
	if (it == m_DataBytes.end()){
		this->m_DataBytes[filepath] = ResData();
		ResData& rData = this->m_DataBytes[filepath];
		rData.m_iRefCount++;
		rData.m_LoadTask= DX::ReadDataAsync(filepath);
		rData.m_LoadTask.then([this, filepath](const std::vector<byte>& data){
			DataIterator it = this->m_DataBytes.find(filepath);
			if (it != m_DataBytes.end()){
				//it->second.m_Data = data;
				//it->second.m_bLoaded = true;
				this->OnLoadFinished( it ->second, filepath, data);
			}
		});
	}
}

void EResMgr::OnLoadFinished(ResData& resData, const std::wstring& filepath, const EDataBytes& bytes){
	//DataIterator it = this->m_DataBytes.find(filepath);
	//if (it != m_DataBytes.end()){
		
		resData.m_Data = bytes;
		resData.m_bLoaded = true;

		std::wstring extName = EPathHelper::GetExtName(filepath);
		RegisterResMap::iterator it = this->m_RegisterResTypes.find(extName);
		if (it != m_RegisterResTypes.end()) {
			RES_BUILDER_FUN fun = it->second;
			if ( fun != nullptr )
				resData.m_Res = (*fun)(bytes);
		}

	//}
}

void EResMgr::UnLoadRes(const std::wstring& filepath){
	DataIterator it = this->m_DataBytes.find(filepath);
	if (it != m_DataBytes.end()){
		it->second.m_iRefCount--;
		if (it->second.m_iRefCount <= 0 && it ->second.m_bLoaded){
			it->second.m_Data.clear();
			m_DataBytes.erase(it);
		}
	}
}

const std::vector<byte>& EResMgr::GetResData(const std::wstring& filepath){
	DataIterator it = this->m_DataBytes.find(filepath);
	assert(it != m_DataBytes.end() && it->second.m_bLoaded);
	return it->second.m_Data;
}

const std::shared_ptr<EResBase> EResMgr::GetRes(const std::wstring& filepath) const{
	ConstDataIterator it = this->m_DataBytes.find(filepath);
	assert(it != m_DataBytes.end() && it->second.m_bLoaded);
	return it->second.m_Res;
}

bool EResMgr::IsResLoaded(const std::wstring& filepath){
	DataIterator it = this->m_DataBytes.find(filepath);
	if (it != m_DataBytes.end()){
		return it->second.m_bLoaded;
	}
	return false;
}

EResMgr& EResMgr::GetInstance(){
	static EResMgr mgr;
	if (!mgr.m_bInit){
		mgr.OnInit();
	}
	return mgr;
}
