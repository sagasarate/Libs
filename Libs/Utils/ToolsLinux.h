/****************************************************************************/
/*                                                                          */
/*      文件名:    ToolsLinux.h                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif
//
//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif






inline void DoSleep(UINT nMilliseconds)
{
	usleep(nMilliseconds*1000);
}

inline int GetSystemCPUCount()
{
	FILE * pFile=NULL;
	int CPUCount=0;

	pFile=fopen(_T("/proc/cpuinfo"),_T("r"));
	if(pFile)
	{
		char LineBuffer[1024];
		while(fgets(LineBuffer,1023,pFile))
		{
			if(_strnicmp(LineBuffer,_T("processor"),9)==0)
			{
				CPUCount++;
			}
		}
		fclose(pFile);
		pFile = NULL;
	}
	if(CPUCount<=0)
		CPUCount++;
	return CPUCount;
}

inline int CloseAllFD()
{
	DIR *dir;
	struct dirent *entry, _entry;
	int retval, rewind, fd;

	dir = opendir("/dev/fd");
	if (dir == NULL)
		return -1;

	rewind = 0;
	while (true) 
	{
		retval = readdir_r(dir, &_entry, &entry);
		if (retval != 0) 
		{
			errno = -retval;
			retval = -1;
			break;
		}
		if (entry == NULL) 
		{
			if (!rewind)
				break;
			rewinddir(dir);
			rewind = 0;
			continue;
		}
		if (entry->d_name[0] == '.')
			continue;
		fd = atoi(entry->d_name);

		if (dirfd(dir) == fd)
			continue;

		if (fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO)
			continue;

		retval = close(fd);
		if (retval != 0)
			break;
		rewind = 1;
	}

	closedir(dir);

	return retval;

}

bool Addr2Line(const char * ModulaName, void * Address, int * CallPipes, char * pOutBuffer, size_t BufferSize);