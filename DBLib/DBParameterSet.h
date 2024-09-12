/****************************************************************************/
/*                                                                          */
/*      文件名:    DBParameterSet.h                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace DBLib
{


class CDBParameterSet :
	public IDBParameterSet
{
protected:	
	struct PARAM_DATA
	{
		DB_PARAM_INFO	ParamInfo;
		CDBValue		ParamValue;
	};

	CEasyArray<PARAM_DATA>		m_Params;
	CDBValue					m_EmptyValue;

	DECLARE_CLASS_INFO_STATIC(CDBParameterSet)
public:
	CDBParameterSet(void);
	virtual ~CDBParameterSet(void);

	
	virtual void Destory();

	virtual int GetCount();
	
	virtual DB_PARAM_INFO * GetParamInfo(int Index);

	virtual CDBValue& GetParam(int Index);
	
	virtual bool AddParam(const DB_PARAM_INFO& ParamInfo, LPCVOID pData, size_t DataLen);
	virtual bool AddParam(const DB_PARAM_INFO& ParamInfo, const CDBValue& Value);
	virtual bool AddParam(const DB_PARAM_INFO& ParamInfo);
	virtual bool SetParam(int Index, DB_PARAM_INFO& ParamInfo, const CDBValue& Value);
	virtual bool SetParam(int Index, DB_PARAM_INFO& ParamInfo);
	
};

}