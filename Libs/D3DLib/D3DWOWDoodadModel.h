/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWDoodadModel.h		                                */
/*      创建日期:  2010年08月05日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

#pragma comment(linker,"/include:?m_CD3DWOWDoodadModelClassInfoRegister@CD3DWOWDoodadModel@D3DLib@@1VCClassInfoRegister@@A")

class CD3DWOWDoodadModel :
	public CD3DWOWM2Model
{
protected:
	WORD		m_GroupIndex;
	DECLARE_CLASS_INFO(CD3DWOWDoodadModel)
public:
	CD3DWOWDoodadModel(void);
	~CD3DWOWDoodadModel(void);

	virtual void Destory();

	void SetGroupIndex(WORD Index);
	WORD GetGroupIndex();
};

inline void CD3DWOWDoodadModel::SetGroupIndex(WORD Index)
{
	m_GroupIndex=Index;
}

inline WORD CD3DWOWDoodadModel::GetGroupIndex()
{
	return m_GroupIndex;
}
}