#pragma once



class CMainConfig :
	public CStaticObject4<CMainConfig>
{
public:
	
protected:
	
public:
	CMainConfig(void);
	~CMainConfig(void);

	bool LoadConfig(LPCTSTR FileName);

	
	
};
