#include "stdafx.h"
#include <execinfo.h>
#include <malloc.h>
#include <sys/wait.h>
#include <termios.h>

bool Addr2Line(const char * ModulaName, void * Address, int * CallPipes, char * pOutBuffer, size_t BufferSize)
{
	if (BufferSize < 128)
		return false;

	char * argv[5];

	argv[0] = (char *)"addr2line";
	argv[1] = (char *)"-e";
	argv[2] = (char *)ModulaName;
	sprintf_s(pOutBuffer, 128, "%p", Address);
	argv[3] = pOutBuffer;
	argv[4] = NULL;

	pid_t pid = fork();
	if (pid > 0)
	{
		int Status = -1;
		if (waitpid(-1, &Status, 0) != -1)
		{
			int ExitCode = WEXITSTATUS(Status);
			PrintImportantLog("调用addressline结果%d", ExitCode);
			size_t ReadSize = read(CallPipes[0], pOutBuffer, BufferSize - 1);
			if (ReadSize != -1)
			{
				pOutBuffer[ReadSize] = 0;
				return true;
			}
			else
			{
				PrintImportantLog("读取管道%d失败%d", CallPipes[0], errno);
			}
			return ExitCode == 0;
		}
		else
		{
			PrintImportantLog("等待子进程%d结束失败%d", pid, errno);
		}
	}
	else if (pid == 0)
	{
		PrintImportantLog("调用addr2line");
		dup2(CallPipes[1], STDOUT_FILENO);
		dup2(CallPipes[1], STDERR_FILENO);
		execvp(argv[0], argv);
		PrintImportantLog("调用addr2line结果%d", errno);
		exit(2);
	}
	else
	{
		PrintImportantLog("新建进程失败%d", errno);
	}
	return false;
}