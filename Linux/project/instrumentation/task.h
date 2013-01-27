#ifndef __TASK_H__
#define __TASK_H__

#include "LinuxNetwork.h"

class Task
{
	public:
		Task(void) {};
		~Task(void) {};

		virtual void Initialize(void);
		virtual void Execute(void);
};

#endif // __TASK_H__

