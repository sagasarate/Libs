/****************************************************************************/
/*                                                                          */
/*      文件名:    StandardFileObjectCreator.h                              */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CStandardFileObjectCreator :
	public IFileObjectCreator
{
protected:
	DECLARE_CLASS_INFO_STATIC(CStandardFileObjectCreator);
public:
	CStandardFileObjectCreator(void);
	virtual ~CStandardFileObjectCreator(void);

	virtual IFileAccessor * CreateFileAccessor();
	
};
