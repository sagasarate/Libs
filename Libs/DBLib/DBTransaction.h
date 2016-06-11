/****************************************************************************/
/*                                                                          */
/*      文件名:    DBTransaction.h                                          */
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

class CDBTransaction :
	public CNameObject
{
protected:
	bool		m_IsSucceed;

	DECLARE_CLASS_INFO_STATIC(CDBTransaction)
public:
	CDBTransaction(void);
	~CDBTransaction(void);

	virtual bool OnExecute(IDBConnection * pConnection);
	virtual void OnFinish();

	void SetResult(bool IsSucceed)
	{
		m_IsSucceed=IsSucceed;
	}
};

}