using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DOSSystem;

namespace CSTest
{
    public class CSTestMain
    {
        static uint m_PluginID;
        bool InitPlugin(uint PluginID, uint LogChannel, string ConfigDir, string LogDir)
        {
            //int a = 0;
            //int b = 10 / a;

            m_PluginID = PluginID;
            Logger.LogChannel = LogChannel;

            for (int i = 0; i < 2; i++)
            {
                CSTester Tester = new CSTester();
                if (i == 0)
                    Tester.IsActive = true;
                DOS_OBJECT_REGISTER_INFO_EX RegisterInfo = new DOS_OBJECT_REGISTER_INFO_EX();

                RegisterInfo.ObjectID.ObjectTypeID = 0x11;
                RegisterInfo.Object = Tester;

                DistributedObjectOperator.RegisterObject(m_PluginID, RegisterInfo);
            }

            Logger.Log("CSTestMain Initialized");

            return true;
        }
        void QueryReleasePlugin()
        {

        }
        void ReleasePlugin()
        {

        }
        
    };
};