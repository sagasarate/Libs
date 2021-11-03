/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSLib.h                                                 */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#include "DOSMessage.h"
#include "DOSDefines.h"

#include "IDistributedObject.h"
#include "IDOSObjectProxy.h"

#include "DOSBaseObject.h"

#include "DOSObjectGroup.h"
#include "DOSObjectManager.h"

//#include "DOSObjectProxyConnectionBase.h"
//#include "DOSObjectProxyServiceBase.h"
#include "DOSObjectProxyConnectionDefault.h"
#include "DOSObjectProxyConnectionGroup.h"
#include "DOSObjectProxyServiceDefault.h"
#ifndef WIN32
#include "DOSObjectProxyConnectionNoBuff.h"
#include "DOSObjectProxyConnectionGroupNoBuff.h"
#include "DOSObjectProxyServiceNoBuff.h"
#endif

//#include "DOSProxyConnection.h"
//#include "DOSObjectProxyService.h"
#include "DOSProxyManager.h"

#include "DOSRouterLinkManager.h"
#ifndef WIN32
#include "ENLConnectionNoBuff.h"
#endif
#include "DOSRouterLink.h"
#include "DOSRouter.h"

#include "DOSServer.h"



#include "DOSClient.h"

