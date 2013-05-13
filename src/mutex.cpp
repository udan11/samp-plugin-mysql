/**
 * Copyright (c) 2013, Dan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mutex.h"

Mutex *Mutex::singleton = 0;
bool Mutex::isEnabled = false;

Mutex::Mutex() {
#ifdef WIN32
	handle = CreateMutex(0, FALSE, NULL); //, "samp_plugin_mysql");
#else
	//handle = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&handle, &attr);
#endif
	isEnabled = true;
}

Mutex::~Mutex() {
	isEnabled = false;
#ifdef WIN32
	CloseHandle(handle);
#else
	pthread_mutex_destroy(&handle);
#endif
	singleton = 0;
}

Mutex *Mutex::getInstance() {
	if (singleton == 0) {
		singleton = new Mutex();
	}
	return singleton;
}

void Mutex::lock() {
	if (isEnabled) {
#ifdef WIN32
		WaitForSingleObject(handle, INFINITE);
#else
		pthread_mutex_lock(&handle);
#endif
	}
}

void Mutex::unlock() {
	if (isEnabled) {
#ifdef WIN32
		ReleaseMutex(handle);
#else
		pthread_mutex_unlock(&handle);
#endif
	}
}
