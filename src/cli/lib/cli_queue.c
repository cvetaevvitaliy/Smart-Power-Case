#include "cli_queue.h"
#include "cli_config.h"

#include "stdlib.h"
#include "string.h"


void Q_Init(QueueObj* qdObj, uint16_t sizeQueue, uint8_t sizeObj, uint32_t mode)
{
	CLI_Queue_t* qd = (CLI_Queue_t*) qdObj;

    qd->ptrObj = (void*) cli_malloc(sizeObj * sizeQueue);

    if (qd->ptrObj == NULL)
    {
	}

	qd->size = sizeQueue;
	qd->_cntr = 0;
	qd->sizeObj = sizeObj;
	qd->mode = mode;
}

bool Q_Push(QueueObj* qdObj, const void* value)
{
	CLI_Queue_t* qd = (CLI_Queue_t*) qdObj;

	if (qd->_cntr >= qd->size)
	{
		if ((qd->mode & QUEUE_FORCED_PUSH_POP_Msk) != 0)
		{
			memcpy(qd->ptrObj, qd->ptrObj + qd->sizeObj, qd->sizeObj * (qd->size - 1));
			memcpy(qd->ptrObj + qd->sizeObj * (qd->_cntr - 1), value, qd->sizeObj);
			qd->_cntr = qd->size;
		}
		else
			return false;
	}
	else
	{
		memcpy(qd->ptrObj + qd->sizeObj * qd->_cntr, value, qd->sizeObj);
		qd->_cntr++;
	}

	return true;
}

bool Q_Pop(QueueObj* qdObj, void* value)
{
	CLI_Queue_t* qd = (CLI_Queue_t*) qdObj;

	if (qd->_cntr == 0)
		return false;

	memcpy((uint8_t*)value, qd->ptrObj, qd->sizeObj);
	for(uint8_t i = 0; i < qd->_cntr; i++)
	{
		memcpy((uint8_t*)(qd->ptrObj + qd->sizeObj * i), (uint8_t*)(qd->ptrObj + qd->sizeObj * (i+1)), qd->sizeObj);
	}

	qd->_cntr--;
	return true;
}

bool Q_IsFull(QueueObj* qdObj)
{
	CLI_Queue_t* qd = (CLI_Queue_t*) qdObj;

	return qd->_cntr >= qd->size;
}

bool Q_IsEmpty(QueueObj* qdObj)
{
	CLI_Queue_t* qd = (CLI_Queue_t*) qdObj;

	return qd->_cntr == 0;
}

bool Q_IsEqual(QueueObj* qd, const void* items, uint32_t size)
{
	if (size > qd->_cntr)
		return false;

	return (memcmp(qd->ptrObj, items, qd->sizeObj * size) == 0);
}
