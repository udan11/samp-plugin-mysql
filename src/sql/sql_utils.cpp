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

#include "sql_utils.h"
#include "sql_query.h"
#include "../main.h"

extern handler_map_t handlers;
extern query_map_t queries;

bool is_valid_handler(int id) {
	// Always call the "const_iterator" version for pure read safety.
	return static_cast<const handler_map_t>(handlers).find(id) != static_cast<const handler_map_t>(handlers).end();
}

bool is_valid_query(int id) {
	// Always call the "const_iterator" version for pure read safety.
	return static_cast<const query_map_t>(queries).find(id) != static_cast<const query_map_t>(queries).end();
}
/*
SQL_Query *get_query(queries_t & q, int query_id) {
	return const_cast<SQL_Query *>(static_cast<queries_t const &>(q).at(query_id));
}

SQL_Handler *get_handler(handlers_t & q, int query_id) {
	return const_cast<SQL_Handler *>(static_cast<handlers_t const &>(q).at(query_id));
}

SQL_Result *get_last_result(SQL_Query & q)
{
	unsigned int last_result = static_cast<SQL_Query const &>(q).last_result;
	unsigned int num_results = static_cast<SQL_Query const &>(q).results.size();
	if (last_result < num_results)
	{
		return const_cast<SQL_Result *>(static_cast<SQL_Query const &>(q).results.at(last_result));
	}
	return 0;
}
*/
