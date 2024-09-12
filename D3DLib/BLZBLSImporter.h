/****************************************************************************/
/*                                                                          */
/*      文件名:    BLZBLSImporter.h                                         */
/*      创建日期:  2010年07月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#pragma once

namespace D3DLib{

class CBLZBLSImporter :
	public CNameObject
{
protected:
	BYTE *								m_pData;
	UINT								m_DataSize;
	bool								m_IsSelfRelease;
	int									m_ShaderType;
	CEasyArray<BLZ_BLS_DATA_BLOCK *>	m_Blocks;

	DECLARE_FILE_CHANNEL_MANAGER

public:
	enum SHADER_TYPE
	{
		SHADER_TYPE_NONE,
		SHADER_TYPE_VS,
		SHADER_TYPE_PS,
	};
	CBLZBLSImporter(void);
	~CBLZBLSImporter(void);

	bool Load(LPCTSTR FileName);
	bool Load(IFileAccessor * pFile);
	bool Load(BYTE * pData,UINT DataSize,bool IsSelfRelease);
	void Close();

	UINT GetDataBlockCount();
	BLZ_BLS_DATA_BLOCK * GetDataBlock(UINT Index);
	CEasyString GetD3DXDisassembledShader(UINT Index);
};

}