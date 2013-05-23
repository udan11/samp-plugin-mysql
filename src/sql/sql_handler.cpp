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

#include "sql_handler.h"
#include "../log.h"
#include <map>

extern std::map<int, class SQL_Handler*> handlers;

SQL_Handler::SQL_Handler() :
	amx(0),
	id(0),
	handler_type(0),
	// Allocate space for 32 queries by default.  More pending at any once time
	// will require an OS memory allocation.
	pending(32),
	done(32),
	mut(),
	is_active(true) {
	// Start the processing thread.
#ifdef _WIN32
	DWORD threadId = 0;
	thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(SQL_Handler::Process), (LPVOID)this, 0, &threadId);
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&thread, &attr, &ProcessQueryThread, (void*)this);
	pthread_attr_destroy(&attr);
#endif
}

SQL_Handler::~SQL_Handler() {
	// Remove myself from the main map.
	handlers.erase(id);
}

void SQL_Handler::destroy(SQL_Handler *This) {
#ifdef _WIN32
	// Signal the processing thread to end soonish.
	This->is_active = false;
	WaitForSingleObject(This->thread, INFINITE);
	CloseHandle(This->thread);
#else
	void *status;
	pthread_join(thread, &status);
#endif
	delete This;
}

#ifdef _WIN32
DWORD __stdcall SQL_Handler::Process(LPVOID lpParam) {
#else
void *SQL_Handler::Process(void *lpParam) {
#endif
	SQL_Handler *This = (SQL_Handler *)lpParam;
	SQL_Query *query = 0;
	while (true) {
		// Only this code can do "pop" on "pending" or "push" on "done".
		while (This->pending.pop(query)) {
			// No need for flags any more - the status is determined by the
			// containing queue.
			log(LOG_DEBUG, "ProcessQueryThread(): Executing query (query->id = %d, query->query = %s)...", query->id, query->query);
			This->lock();
			This->execute_query(query);
			This->unlock();
			This->done.push(query);
			query = 0;
		}
		// One last iteration to clear pending queries.
		if (!This->is_active) break;
		SLEEP(50);
	}
	// Removed by the other thread (set inactive).
	//delete This;
	return 0;
}
