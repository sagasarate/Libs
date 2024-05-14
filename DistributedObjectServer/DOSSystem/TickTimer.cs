using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DOSSystem
{
    public class TickTimer
    {
        const uint MAXTIME = (uint)0xffffffff;

        uint m_dwSavedTime = 0;
        uint m_dwTimeoutTime = 0;

        public static uint GetTimeToTime(uint t1, uint t2)
        {
            return (t1 <= t2 ? (t2 - t1) : (MAXTIME - t1 + t2));
        }

        public static uint GetTime()
        {
            return (uint)System.Environment.TickCount;
        }
        public void SaveTime(uint dwCurTime)
        {
            m_dwSavedTime = dwCurTime;
        }
        public void SaveTime()
        {
            SaveTime(GetTime());
        }
        public static bool IsTimeOut(uint starttime, uint timeout)
        {
            uint dwTime = GetTime();
            if (GetTimeToTime(starttime, dwTime) >= timeout)
            {
                return true;
            }
            return false;
        }
        public bool IsTimeOut(uint dwTimeOut)
        {
            uint dwTime = GetTime();
            if (GetTimeToTime(m_dwSavedTime, dwTime) >= dwTimeOut)
            {
                return true;
            }
            return false;
        }
        public void SetTimeOut(uint dwTimeOut)
        {
            m_dwSavedTime = GetTime();
            m_dwTimeoutTime = dwTimeOut;
        }
        public bool IsTimeOut()
        {
            uint dwTime = GetTime();
            if (GetTimeToTime(m_dwSavedTime, dwTime) >= m_dwTimeoutTime)
                return true;
            return false;
        }
        public void Reset(uint dwCurTime)
        {
            m_dwSavedTime = dwCurTime;
        }
        public void Reset()
        {
            Reset(GetTime());
        }
        public uint GetLeftTime(uint dwCurTime)
        {
            uint dwTime = GetTimeToTime(m_dwSavedTime, dwCurTime);
            if (dwTime >= m_dwTimeoutTime) return 0;
            return (m_dwTimeoutTime - dwTime);
        }
        public uint GetLeftTime()
        {
            return GetLeftTime(GetTime());
        }
        public uint GetPastTime(uint dwCurTime)
        {
            return GetTimeToTime(m_dwSavedTime, dwCurTime);
        }
        public uint GetPastTime()
        {
            return GetPastTime(GetTime());
        }
        public uint GetTimeOut() 
        { 
            return m_dwTimeoutTime; 
        }
        public uint GetSavedTime() 
        { 
            return m_dwSavedTime; 
        }
    }
}
