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

#pragma once

#define SQL_HANDLER_MYSQL				1
//#define SQL_HANDLER_PGSQL				2

#include "../sdk/amx/amx.h"

#include "sql_query.h"
#include "../mutex.h"

#include <boost/lockfree/queue.hpp>

#ifdef _WIN32
	#include <Windows.h>
	#define SLEEP(x) Sleep(x);
#else
	#include "pthread.h"
	#include <unistd.h>
	#define SLEEP(x) usleep(x * 1000);
	typedef unsigned long DWORD;
	typedef unsigned int UINT;
#endif

class SQL_Handler {
	private:
		typedef boost::lockfree::queue< class SQL_Query *, boost::lockfree::allocator< std::allocator<class SQL_Query *> > > queue_t;
		// Pending queries for this handler.
		queue_t pending, done;
		// Should the handler remain running.
		bool is_active;
		// Just one mutex in case a single connection is used to run both
		// threaded and unthreded queries.
		Mutex mut;
	protected:
		// Destructor.
		virtual ~SQL_Handler();
	public:
		// Helper function to safely destroy a handler.
		static void destroy(SQL_Handler *This);
		// The AMX machine hosting this handler.
		AMX *amx;
		// Handler's ID & type.
		int id, handler_type;
		// Constructor.
		SQL_Handler();
		// Connects to a MySQL server and returns handler's ID.
		virtual bool connect(const char *host, const char *user, const char *pass, const char *db, int port) = 0;
		// Disconnects from the MySQL server.
		virtual void disconnect() = 0;
		// Gets latest error ID.
		virtual int get_errno() = 0;
		// Gets latest error message.
		virtual const char *get_error() = 0;
		// Pings the servers.
		virtual int ping() = 0;
		// Gets server's information.
		virtual const char *get_stat() = 0;
		// Gets default character set.
		virtual const char *get_charset() = 0;
		// Sets default character set.
		virtual bool set_charset(char *charset) = 0;
		// Escapes a string and returns the new length.
		virtual int escape_string(const char *src, char *&dest) = 0;
		// Executes a query.
		virtual void execute_query(SQL_Query *query) = 0;
		// Seeks a result.
		virtual bool seek_result(SQL_Query *query, int result) = 0;
		// Fetches the name of a field.
		virtual bool fetch_field(SQL_Query *query, int fieldix, char *&dest, int &len) = 0;
		// Seeks a row in the result.
		virtual bool seek_row(SQL_Query *query, int row) = 0;
		// Fetches a cell by it's index.
		virtual bool fetch_num(SQL_Query *query, int fieldidx, char *&dest, int &len) = 0;
		// Fetches a cell by it's name.
		virtual bool fetch_assoc(SQL_Query *query, char *fieldname, char *&dest, int &len) = 0;
		#ifdef _WIN32
		static DWORD __stdcall Process(LPVOID lpParam);
		HANDLE thread;
		#else
		static void *Process(void *lpParam);
		pthread_t thread;
		#endif
		bool pop(SQL_Query* & ret) { return done.pop(ret); };
		bool push(SQL_Query* const & ret) { return pending.push(ret); };
		void lock() { mut.lock(); };
		void unlock() { mut.unlock(); };
};
