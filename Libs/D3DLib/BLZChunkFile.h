/****************************************************************************/
/*                                                                          */
/*      文件名:    BLZChunkFile.h                                           */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CBLZChunkFile :
	public CNameObject
{
public:
	typedef CEasyArray<BLZ_CHUNK_HEADER *>	CChunkList;
protected:
	BYTE *										m_pData;
	UINT										m_DataSize;	
	bool										m_IsSelfRelease;
	CEasyMap<UINT,CChunkList>					m_Chunks;
public:
	CBLZChunkFile(void);
	~CBLZChunkFile(void);

	BOOL Load(IFileAccessor * pFile);
	BOOL Load(BYTE * pData,UINT DataSize);
	void Close();

	BYTE * GetData();
	UINT GetDataSize();

	CChunkList * GetChunks(UINT ChunkID);
	BLZ_CHUNK_HEADER * GetFirstChunk(UINT ChunkID);
	BLZ_CHUNK_HEADER * GetChunkByOffset(UINT ChunkID,UINT Offset);


};

inline BYTE * CBLZChunkFile::GetData()
{
	return m_pData;
}

inline UINT CBLZChunkFile::GetDataSize()
{
	return m_DataSize;
}


}