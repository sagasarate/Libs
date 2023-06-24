/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DScene.h                                               */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

#pragma comment(linker,"/include:?m_CD3DSceneClassInfoRegister@CD3DScene@D3DLib@@1VCClassInfoRegister@@A")

class CD3DScene :
	public CD3DObject
{
protected:
	CD3DBoundingBox				m_BoundingBox;

	DECLARE_CLASS_INFO(CD3DScene)
public:
	CD3DScene(void);
	virtual ~CD3DScene(void);

	virtual bool CanRender();

	virtual CD3DBoundingBox * GetBoundingBox();

	bool GetHeightByXZ(const CD3DVector3 Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight);
	

	virtual bool AddChild(CTreeObject* pChild,bool CheckExist=false);

};

inline bool CD3DScene::CanRender()
{
	return false;
}

inline CD3DBoundingBox * CD3DScene::GetBoundingBox()
{
	return &m_BoundingBox;
}

}