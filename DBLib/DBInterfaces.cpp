/****************************************************************************/
/*                                                                          */
/*      文件名:    DBInterfaces.cpp                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

namespace DBLib
{

IMPLEMENT_CLASS_INFO_STATIC(IDatabase,CNameObject);
IDatabase::IDatabase():CNameObject()
{
}

IDatabase::~IDatabase()
{
}

IMPLEMENT_CLASS_INFO_STATIC(IDBConnection,CNameObject);
IDBConnection::IDBConnection():CNameObject()
{
}

IDBConnection::~IDBConnection()
{
}

IMPLEMENT_CLASS_INFO_STATIC(IDBRecordSet,CNameObject);
IDBRecordSet::IDBRecordSet():CNameObject()
{
}

IDBRecordSet::~IDBRecordSet()
{
}

IMPLEMENT_CLASS_INFO_STATIC(IDBParameterSet,CNameObject);
IDBParameterSet::IDBParameterSet():CNameObject()
{
}

IDBParameterSet::~IDBParameterSet()
{
}

//IMPLEMENT_CLASS_INFO_STATIC(CDBValue,CNameObject);
//CDBValue::CDBValue():CNameObject()
//{
//}
//
//CDBValue::~CDBValue()
//{
//}

}