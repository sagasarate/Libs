using System;
using System.Collections.Generic;
using DOSSystem;

namespace DOSSystem
{
    public class SingletonObject<T>
    {
        private static T m_Singleton;
        private static object lockHelper = new object();

        protected SingletonObject()
        {
        }

        public static T GetInstance()
        {
            if (m_Singleton == null)
            {
                lock (lockHelper)
                {
                    if (m_Singleton == null)
                    {
                        m_Singleton = (T)Activator.CreateInstance(typeof(T));
                    }
                }
            }
            return m_Singleton;
        }
        //public static T GetInst()
        //{
        //    return GetInstance();
        //}
    }

    //public class SingletonMsgCaller<T>
    //{
    //    private static T m_Singleton;
    //    private static object lockHelper = new object();

    //    protected SingletonMsgCaller()
    //    {
    //    }

    //    public static T GetInstance()
    //    {
    //        if (m_Singleton == null)
    //        {
    //            lock (lockHelper)
    //            {
    //                if (m_Singleton == null)
    //                {
    //                    object[] args = new object[2];
    //                    args[0] = NetworkManager.GetInstance().GetMainConnection();
    //                    args[1] = new OBJECT_ID();
    //                    m_Singleton = (T)Activator.CreateInstance(typeof(T), args);
    //                }
    //            }
    //        }
    //        return m_Singleton;
    //    }
    //    public static T GetInst()
    //    {
    //        return GetInstance();
    //    }
    //}

    //public class SingletonSynMsgCaller<T>
    //{
    //    private static T m_Singleton;
    //    private static object lockHelper = new object();

    //    protected SingletonSynMsgCaller()
    //    {
    //    }

    //    public static T GetInstance()
    //    {
    //        if (m_Singleton == null)
    //        {
    //            lock (lockHelper)
    //            {
    //                if (m_Singleton == null)
    //                {
    //                    object[] args = new object[2];
    //                    args[0] = NetworkManager.GetInstance().GetSynDataConnection();
    //                    args[1] = new OBJECT_ID();
    //                    m_Singleton = (T)Activator.CreateInstance(typeof(T), args);
    //                }
    //            }
    //        }
    //        return m_Singleton;
    //    }
    //    public static T GetInst()
    //    {
    //        return GetInstance();
    //    }
    //}

}