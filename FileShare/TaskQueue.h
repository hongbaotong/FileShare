#pragma once
#include <afxmt.h>
#include "LibCommon/common.h"

class CTaskQueue
{
public:
	CTaskQueue(void);
public:
	~CTaskQueue(void);
public:
	void PutTask(TASK_INFO_TYPE* pTaskInfo);
	TASK_INFO_TYPE* GetTask(void);
	CSemaphore* m_Semaphore;
private:
	CList<TASK_INFO_TYPE*> m_TaskQueue;
	CCriticalSection m_Lock;
};
