#include "StdAfx.h"
#include "TaskQueue.h"

CTaskQueue::CTaskQueue(void)
{
	m_Semaphore=new CSemaphore(0,20);
}

CTaskQueue::~CTaskQueue(void)
{
	SAFE_DELETE(m_Semaphore);

	m_TaskQueue.RemoveAll();

}

void CTaskQueue::PutTask(TASK_INFO_TYPE* pTaskInfo)
{
	m_Lock.Lock();
	m_TaskQueue.AddTail(pTaskInfo);
	m_Semaphore->Unlock();
	m_Lock.Unlock();
}

TASK_INFO_TYPE* CTaskQueue::GetTask(void)
{
	TASK_INFO_TYPE* pTaskInfo=NULL;

	m_Semaphore->Lock();
	m_Lock.Lock();
	if(m_TaskQueue.GetCount()>0)
	{
		pTaskInfo=m_TaskQueue.GetHead();
		m_TaskQueue.RemoveHead();
	}
	m_Lock.Unlock();

	return pTaskInfo;
}