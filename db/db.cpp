// db.cpp : Defines the exported functions for the DLL application.
//
/*
#include "stdafx.h"
#include "db.h"
#include "sqlite3.h"
#include <iostream>
#include <cstdlib>

using namespace std;
namespace db
{
	Blob::Blob(const Blob& other) noexcept
	{
		std::cout << "Blob copy constructor\n";
		value = std::malloc(other.size);
		if (value)
		{
			std::memcpy(value, other.value, other.size);
			size = other.size;
		}
		else
		{
			size = 0;
		}
	}

	Blob& Blob::operator=(const Blob& other) noexcept
	{
		std::cout << "Blob copy assignment\n";
		if (this == &other) return *this;
		if (size != other.size)
		{
			std::free(value);
			value = std::malloc(other.size);
			size = value ? other.size : 0;
		}
		std::memcpy(value, other.value, size);
		return *this;
	}

	Blob::Blob(Blob&& other) noexcept
	{
		std::cout << "Blob move constructor\n";
		value = other.value;
		size = other.size;
		other.value = nullptr;
		other.size = 0;
	}

	Blob& Blob::operator=(Blob&& other) noexcept
	{
		std::cout << "Blob move assignment\n";
		if (this == &other) return *this;
		std::free(value);
		value = other.value;
		size = other.size;
		other.value = nullptr;
		other.size = 0;
		return *this;
	}

	Blob::~Blob()
	{
		std::cout << "Blob destructor\n";
		std::free(value);
	}

	int convert_mode(DB_connection::Mode mode) noexcept
	{
		switch (mode)
		{
		case DB_connection::Mode::Create:
			return SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
		case DB_connection::Mode::Read_only:
			return SQLITE_OPEN_READONLY;
		case DB_connection::Mode::Read_write:
			return SQLITE_OPEN_READWRITE;
		default:
			return SQLITE_OPEN_READONLY;
		}
	}

	DB_connection::DB_connection(std::string path, Mode mode) : m_path{ std::move(path) }, m_mode{ mode }
	{
		int rc;
		rc = sqlite3_open_v2(m_path.c_str(), &m_connection, convert_mode(m_mode), nullptr);
		if (rc != SQLITE_OK)
		{
			std::string msg = sqlite3_errmsg(m_connection);
			sqlite3_close_v2(m_connection);
			m_connection = nullptr;
			throw (db_exception("Could not open database " + m_path + ": " + msg));
		}
	}

	DB_connection::DB_connection(const DB_connection& other) : DB_connection{ other.m_path, other.m_mode } {}

	DB_connection& DB_connection::operator=(DB_connection other)
	{
		swap(*this, other);
		return *this;
	}

	DB_connection::DB_connection(DB_connection&& other) noexcept
	{
		swap(*this, other);
	}

	DB_connection& DB_connection::operator=(DB_connection&& other) noexcept
	{
		if (this != &other)
		{
			close();
			swap(*this, other);
		}
		return *this;
	}

	DB_connection::~DB_connection() noexcept
	{
		close();
	}

	void DB_connection::close() noexcept
	{
		sqlite3_stmt* statement = sqlite3_next_stmt(m_connection, nullptr);
		while (statement != nullptr)
		{
			sqlite3_finalize(statement);
			statement = sqlite3_next_stmt(m_connection, statement);
		}
		sqlite3_close_v2(m_connection);
	}

	Prepared_statement DB_connection::prepare(std::string sql)
	{
		int rc;
		sqlite3_stmt* statement;
		rc = sqlite3_prepare_v2(m_connection, sql.c_str(), -1, &statement, nullptr);
		if (rc == SQLITE_OK)
		{
			return Prepared_statement(statement);
		}
		else
		{
			std::string msg = sqlite3_errstr(rc);
			throw db_exception(msg);
		}
	}

	Prepared_statement::Prepared_statement(Prepared_statement&& other) noexcept
	{
		m_statement = other.m_statement;
		other.m_statement = nullptr;
	}

	Prepared_statement& Prepared_statement::operator=(Prepared_statement&& other) noexcept
	{
		sqlite3_finalize(m_statement);
		m_statement = other.m_statement;
		other.m_statement = nullptr;
		return *this;
	}

	Prepared_statement::~Prepared_statement() noexcept
	{
		sqlite3_finalize(m_statement);
	}

	void handle_error(int errorcode)
	{
		if (errorcode != SQLITE_OK)
		{
			throw db_exception(sqlite3_errstr(errorcode));
		}
	}

	template<class... Ts>
	struct overload : Ts...
	{
		using Ts::operator()...;
	};

	template<class... Ts>
	overload(Ts...)->overload<Ts...>;

	void Prepared_statement::bind(int index, value_t value)
	{
		std::visit
		(
			overload
			{
				[this, index](const std::monostate m) {handle_error(sqlite3_bind_null(m_statement, index)); },
				[this, index](const int i) {handle_error(sqlite3_bind_int(m_statement, index, i)); },
				[this, index](const double d) {handle_error(sqlite3_bind_double(m_statement, index, d)); },
				[this, index](const std::string& s) {handle_error(sqlite3_bind_text(m_statement, index, s.c_str(), -1, SQLITE_TRANSIENT)); },
				[this, index](const Blob b) {handle_error(sqlite3_bind_blob(m_statement, index, b.value, b.size, SQLITE_TRANSIENT)); }
			}
			, value
		);
	}

	Prepared_statement::Row_result Prepared_statement::execute_row()
	{
		int rc = sqlite3_step(m_statement);
		if (rc == SQLITE_DONE)
		{
			return Row_result::Success;
		}
		if (rc == SQLITE_ROW)
		{
			return Row_result::Row;
		}
		throw db_exception(sqlite3_errstr(rc));
		return Row_result::Error;
	}

	row_t Prepared_statement::fetch_row()
	{
		auto num_columns = sqlite3_column_count(m_statement);
		row_t result;
		for (int i = 0; i < num_columns; ++i)
		{
			std::string colname = sqlite3_column_name(m_statement, i);
			auto coltype = sqlite3_column_type(m_statement, i);
			switch (coltype)
			{
			case SQLITE_NULL:
				result[colname] = std::monostate{};
				break;
			case SQLITE_INTEGER:
				result[colname] = sqlite3_column_int(m_statement, i);
				break;
			case SQLITE_FLOAT:
				result[colname] = sqlite3_column_double(m_statement, i);
				break;
			case SQLITE_TEXT:
				result[colname] = std::string{ reinterpret_cast<const char*> (sqlite3_column_text(m_statement, i)) };
				break;
			case SQLITE_BLOB:
			{
				Blob b;
				b.value = const_cast<void*> (sqlite3_column_blob(m_statement, i));
				b.size = sqlite3_column_bytes(m_statement, i);
				result[colname] = b;
			}
				break;
			default:
				result[colname] = std::monostate{};
			}
		}
		return result;
	}

	table_t Prepared_statement::fetch_table()
	{
		table_t result;
		while (execute_row() == Row_result::Row)
		{
			result.push_back(fetch_row());
		}
		reset();
		return result;
	}

	void Prepared_statement::reset() noexcept
	{
		sqlite3_reset(m_statement);
		sqlite3_clear_bindings(m_statement);
	}

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
	}*/

/*	void handle_error (int errorcode)
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
*/
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
/*
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
	*/

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
/*
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
}*/