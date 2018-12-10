// db.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#include "stdafx.h"
#include "db.h"
#include "sqlite3.h"
#include <iostream>
#include <vector>

using namespace std;
namespace db
{

	db_connection_ptr db_connection::create(string dbname)
	{
		if (dbname.empty())
		{
			throw "Name must not be null";
		}
		auto conn = unique_ptr<db::db_connection>(new db::db_connection());
		conn->init(dbname);
		return conn;
	}

	void db_connection::init(string dbname)
	{
		sqlite3* sqlitehandle;
		int rc;
		rc = sqlite3_open_v2(dbname.c_str(), &sqlitehandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		if (rc)			// open failed
		{
			connection = nullptr;
			string msg = sqlite3_errmsg(sqlitehandle);
			sqlite3_close(sqlitehandle);
			throw msg;
		}
		connection = sqlitehandle;
	}

	void db_connection::connect(string dbname)
	{
		if (dbname.empty())
		{
			sqlite3_close(connection);
			connection = nullptr;
		}
		if (dbname.compare(name) == 0 && connection != nullptr)
		{
			return;
		}

		this->init(dbname);
	}
	static int callback(void* ignoreme, int argc, char** argv, char ** colname)
	{
		int i;
		for (i = 0; i < argc; i++)
		{
			string value = argv[i] ? argv[i] : "NULL";
			cout << colname[i] << "\t" << value << "\t";
		}
		cout << endl;
		return 0;
	}

	void db_connection::exec_sql(string sql)
	{
		int rc;
		char* msg = nullptr;
		rc = sqlite3_exec(connection, sql.c_str(), callback, NULL, &msg);
		if (rc != SQLITE_OK)
		{
			string errmsg(msg);
			sqlite3_free(msg);
			throw errmsg;
		}
	}

	prepared_statement_ptr db_connection::prepare(string sql)
	{
		int rc;
		sqlite3_stmt* statement;
		prepared_statement_ptr p = prepared_statement::create();
		rc = sqlite3_prepare_v2(connection, sql.c_str(), -1, &statement, NULL);//sql.length () + 1, &statement, NULL);
		if (rc == SQLITE_OK)
		{
			p->set_statement(statement);
			return p;
		}
		else
		{
			string msg = sqlite3_errstr(rc);
			throw msg;
		}
	}

	db_connection::~db_connection()
	{
		sqlite3_close_v2(connection);
	}

	prepared_statement::~prepared_statement()
	{
		int rc;
		rc = sqlite3_finalize(statement);
	}

	prepared_statement_ptr prepared_statement::create()
	{
		return shared_ptr<prepared_statement>(new prepared_statement());
	}

	void prepared_statement::set_statement(sqlite3_stmt* prep_stmt)
	{
		statement = prep_stmt;
	}

	void handle_error (int errorcode)
	{
		if (errorcode != SQLITE_OK)
		{
			throw db_exception (sqlite3_errstr (errorcode));
		}
	}

	template<class... Ts>
	struct overload : Ts...
	{
		using Ts::operator()...;
	};

	template<class... Ts>
	overload (Ts...)->overload<Ts...>;

	void prepared_statement::bind (int index, value_t value)
	{
		std::visit 
		(
			overload 
			{
				[this, index] (const std::monostate& m) {handle_error (sqlite3_bind_null (statement, index)); },
				[this, index](const int& i) {handle_error (sqlite3_bind_int (statement, index, i)); },
				[this, index](const double& d) {handle_error (sqlite3_bind_double (statement, index, d)); },
				[this, index](const std::string& s) {handle_error (sqlite3_bind_text (statement, index, s.c_str (), -1, SQLITE_TRANSIENT)); }
			}
			, value
		);
	}

/*	void prepared_statement::bind(int index, string value)
	{
		int rc;
		rc = sqlite3_bind_text(statement, index, value.c_str(), -1, SQLITE_TRANSIENT);
		if (rc != SQLITE_OK)
		{
			throw sqlite3_errstr(rc);
		}
	}

	void prepared_statement::bind(int index, int value)
	{
		int rc;
		rc = sqlite3_bind_int(statement, index, value);
		if (rc != SQLITE_OK)
		{
			throw sqlite3_errstr(rc);
		}
	}

	void prepared_statement::bind(int index, double value)
	{
		int rc;
		rc = sqlite3_bind_double(statement, index, value);
		if (rc != SQLITE_OK)
		{
			throw sqlite3_errstr(rc);
		}
	}
	void prepared_statement::bind_null(int index)
	{
		int rc;
		rc = sqlite3_bind_null(statement, index);
		if (rc != SQLITE_OK)
		{
			throw sqlite3_errstr(rc);
		}
	}*/

	void prepared_statement::reset()
	{
		sqlite3_reset(statement);
		sqlite3_clear_bindings(statement);
	}

	Result_code prepared_statement::execute_row()
	{
		int rc = 0;
		rc = sqlite3_step(statement);
		if (rc == SQLITE_DONE)
		{
			return Result_code::Success;
		}
		if (rc == SQLITE_ROW)
		{
			return Result_code::Row;
		}
		throw db_exception (sqlite3_errstr (rc));
		return Result_code::Error;
	}

	string prepared_statement::column_name(int index)
	{
		const char* value_cstr = sqlite3_column_name(statement, index);
		return string(value_cstr);
	}

	string prepared_statement::fetch_string(int index)
	{
		if (sqlite3_column_type(statement, index) == SQLITE_NULL)
		{
			return "";
		}
		char* value_cstr = (char*)sqlite3_column_text(statement, index);
		string result = "";
		result.assign(value_cstr);
		return result;
	}

	int prepared_statement::fetch_int(int index)
	{
		int result = sqlite3_column_int(statement, index);
		return result;
	}

	row_t prepared_statement::fetch_row ()
	{
		int num_columns = sqlite3_column_count (statement);
		row_t result;
		for (int i = 0; i < num_columns; ++i)
		{
			string colname = sqlite3_column_name (statement, i);
			int coltype = sqlite3_column_type (statement, i);
			switch (coltype)
			{
			case SQLITE_NULL:
				result[colname] = std::monostate{};
				break;
			case SQLITE_INTEGER:
				result[colname] = sqlite3_column_int (statement, i);
				break;
			case SQLITE_FLOAT:
				result[colname] = sqlite3_column_double (statement, i);
				break;
			case SQLITE_TEXT:
				result[colname] = std::string{ (char*)(sqlite3_column_text (statement, i)) };
				break;
			case SQLITE_BLOB:
				break;
			}
		}
		return result;
	}


/*			value cell;
			string colname = sqlite3_column_name(statement, i);
			int coltype = sqlite3_column_type(statement, i);
			switch (coltype)
			{
			case SQLITE_NULL:
				cell.type = value::Type::NULL_T;
				break;
			case SQLITE_INTEGER:
				cell.type = value::Type::INTEGER;
				cell.integer_value = sqlite3_column_int(statement, i);
				break;
			case SQLITE_FLOAT:
				cell.type = value::Type::DOUBLE;
				cell.double_value = sqlite3_column_double(statement, i);
				break;
			case SQLITE_TEXT:
				cell.type = value::Type::STRING;
				{
					char* temp = (char*)sqlite3_column_text(statement, i);
					cell.string_value.assign(temp);
				}
				break;
			case SQLITE_BLOB:
				cell.type = value::Type::BLOB;
				{
					int blobsize = sqlite3_column_bytes16(statement, i);
					auto blob = sqlite3_column_blob(statement, i);
					cell.blob_value = new char[blobsize];
					memcpy(cell.blob_value, blob, blobsize);
				}
				break;
			}
			result[colname] = cell;
		}
		return result;
	}*/

	table_data_t prepared_statement::fetch_table()
	{
		table_data_t result;
		while (this->execute_row() == Result_code::Row)
		{
			row_t row = this->fetch_row();
			result.push_back(row);
		}
		sqlite3_reset(statement);
		return result;
	}
}