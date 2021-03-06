/*
* Copyright (c) 2017 Intel Corporation. All rights reserved.
* Modifications Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* This software is available to you under a choice of one of two
* licenses.  You may choose to be licensed under the terms of the GNU
* General Public License (GPL) Version 2, available from the file
* LICENSE-GPL-2.0 in the main directory of this source tree, or the
* BSD license below:
*
*     Redistribution and use in source and binary forms, with or
*     without modification, are permitted provided that the following
*     conditions are met:
*
*      - Redistributions of source code must retain the above
*        copyright notice, this list of conditions and the following
*        disclaimer.
*
*      - Redistributions in binary form must reproduce the above
*        copyright notice, this list of conditions and the following
*        disclaimer in the documentation and/or other materials
*        provided with the distribution.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#include <windows.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define PTHREAD_MUTEX_INITIALIZER {0}

#define pthread_cond_signal WakeConditionVariable
#define pthread_cond_broadcast WakeAllConditionVariable
#define pthread_mutex_init(mutex, attr) (InitializeCriticalSection(mutex), 0)
#define pthread_mutex_destroy(mutex) (DeleteCriticalSection(mutex), 0)
#define pthread_cond_init(cond, attr) (InitializeConditionVariable(cond), 0)
#define pthread_cond_destroy(x)	/* nothing to do */
#define pthread_spin_init(mutex, attr) (InitializeCriticalSectionAndSpinCount(mutex, 4000), 0)
#define pthread_spin_destroy(mutex) (DeleteCriticalSection(mutex), 0)

typedef CRITICAL_SECTION	pthread_mutex_t;
typedef CONDITION_VARIABLE	pthread_cond_t;
typedef HANDLE			pthread_t;
typedef CRITICAL_SECTION    pthread_spinlock_t;

static inline int pthread_spin_lock(pthread_mutex_t* mutex)
{
    EnterCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_lock(pthread_mutex_t* mutex)
{
	EnterCriticalSection(mutex);
	return 0;
}

static inline int pthread_mutex_trylock(pthread_mutex_t* mutex)
{
	return !TryEnterCriticalSection(mutex);
}

static inline int pthread_spin_unlock(pthread_mutex_t* mutex)
{
    LeaveCriticalSection(mutex);
    return 0;
}

static inline int pthread_mutex_unlock(pthread_mutex_t* mutex)
{
	LeaveCriticalSection(mutex);
	return 0;
}

static inline int pthread_join(pthread_t thread, void** exit_code)
{
	if (WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0) {
		if (exit_code) {
			DWORD ex = 0;
			GetExitCodeThread(thread, &ex);
			*exit_code = (void*)(uint64_t)ex;
		}
		CloseHandle(thread);
		return 0;
	}

	return -1;
}

typedef struct fi_thread_arg
{
	void* (*routine)(void*);
	void* arg;
} fi_thread_arg;

static DWORD WINAPI ofi_thread_starter(void* arg)
{
	fi_thread_arg data = *(fi_thread_arg*)arg;
	free(arg);
	return (DWORD)(uint64_t)data.routine(data.arg);
}

static inline int pthread_create(pthread_t* thread, void* attr, void *(*routine)(void*), void* arg)
{
	(void) attr;
	fi_thread_arg* data = (fi_thread_arg*)malloc(sizeof(*data));
	data->routine = routine;
	data->arg = arg;
	DWORD threadid;
	*thread = CreateThread(0, 0, ofi_thread_starter, data, 0, &threadid);
	return *thread == 0;
}

static inline int pthread_equal(pthread_t t1, pthread_t t2)
{
	(void)t1;
	(void)t2;
	/*
	 * TODO: temporary solution
	 * Need to implement
	 */
	return ENOSYS;
}

static inline int pthread_cancel(pthread_t thread)
{
	(void)thread;
	/*
	 * TODO: temporary solution
	 * Need to implement
	 */
	return ENOSYS;
}

static inline pthread_t pthread_self(void)
{
	/*
	 * TODO: temporary solution
	 * Need to implement
	 */
	return (pthread_t) ENOSYS;
}

/*
 * TODO: temporary solution
 * Need to re-implement
 */
typedef void(*pthread_cleanup_callback_t)(void *);
typedef struct pthread_cleanup_t
{
	pthread_cleanup_callback_t routine;
	void *arg;
} pthread_cleanup_t;
#ifndef __cplusplus 
#define pthread_cleanup_push(_rout, _arg)				\
{									\
	pthread_cleanup_t _cleanup = {					\
		.routine = (pthread_cleanup_callback_t) (_rout),	\
		.arg = (_arg),						\
	};								\

#define pthread_cleanup_pop(_execute)					\
	if (_execute)							\
		(void) (*(_cleanup.routine))(_cleanup.arg);		\
}
#else
#define pthread_cleanup_push(_rout, _arg)				\
{									\
	pthread_cleanup_t _cleanup;					\
	_cleanup.routine = (pthread_cleanup_callback_t) _rout,		\
	_cleanup.arg = (_arg);						\
	__try								\
	{								\

#define pthread_cleanup_pop(_execute)					\
	}								\
	__finally {							\
		if( _execute || AbnormalTermination())			\
			(*(_cleanup.routine))(_cleanup.arg);		\
	}								\
}
#endif
