/****************************************************************************/
/*                                                                          */
/*      文件名:    DBStaticRecordSet.h                                      */
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



	class CDBStaticRecordSet :
		public IDBRecordSet
	{
	protected:
		typedef CEasyArray<CDBValue> CDBValueRow;
		CEasyArray<DB_COLUMN_INFO>		m_pColumnInfos;
		CEasyArray<CDBValueRow>			m_Records;
		CEasyArray<CDBValue>			m_Params;
		int								m_CurRow;
		CDBValue						m_EmptyValue;

		DECLARE_CLASS_INFO_STATIC(CDBStaticRecordSet)
	public:
		CDBStaticRecordSet(void);
		virtual ~CDBStaticRecordSet(void);

		int Init(LPCVOID pData, size_t DataSize);
		virtual void Destory() override;

		virtual int GetRecordCount() override;
		virtual int GetColumnCount() override;
		virtual LPCSTR GetColumnName(int Index) override;
		virtual int GetIndexByColumnName(LPCSTR Name) override;
		virtual DB_COLUMN_INFO* GetColumnInfo(int Index) override;


		virtual CDBValue& GetField(int Index) override;
		virtual CDBValue& GetField(LPCSTR Name) override;
		virtual int MoveFirst() override;
		virtual int MoveLast() override;
		virtual int MoveNext() override;
		virtual int MovePrevious() override;
		virtual int MoveTo(int Index) override;
		virtual bool IsEOF() override;
		virtual bool IsBOF() override;

		virtual bool Close() override;

		virtual bool SetBlobMaxProcessSize(size_t MaxSize) override;
	};

}
