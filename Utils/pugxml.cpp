/****************************************************************************/
/*                                                                          */
/*      文件名:    pugxml.cpp                                      */
/*      创建日期:  2012年02月02日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


//#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
//#undef new
//#endif

#include <sstream>

//#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
//#define new new( __FILE__, __LINE__ )
//#endif


bool xml_parser::parse_file(const TCHAR* path, unsigned int optmsk, int FileChannel)
{
	bool Ret = false;
	CStringFile XMLFile(FileChannel);

	clear();
	XMLFile.SetLocalCodePage(m_LocalCodePage);

	if (XMLFile.LoadFile(path, false))
	{
		_buffer.SetString(XMLFile.GetData(), (int)XMLFile.GetDataLen());

		_xmldoc = new_node(node_document); //Allocate a new root.
		_xmldoc->parent = _xmldoc; //Point to self.
		if (optmsk != parse_noset) _optmsk = optmsk;
		_strpos = parse(_buffer, _xmldoc, _growby, _optmsk);
		return true;
	}
	return false;
}

void xml_parser::SaveToString(const xml_node& RootNode, CEasyString& String)
{
#ifdef UNICODE
	std::wstringstream os;
#else
	std::stringstream os;
#endif

	os << RootNode;
	os<<'\0';

	String=os.str().c_str();

	String.Replace(_T("\n"),_T("\r\n"));


}
bool xml_parser::SaveToFile(const xml_node& RootNode, LPCTSTR FileName, int FileCodePage, int FileChannel)
{
	CStringFile XMLFile(FileChannel);

	XMLFile.SetLocalCodePage(m_LocalCodePage);
	XMLFile.SetSaveCodePage(FileCodePage);

	CEasyString XMLString;

	SaveToString(RootNode, XMLString);

	if (XMLFile.SaveToFile(XMLString, FileName))
		return true;

	return false;
}
