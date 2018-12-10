#pragma once  
#define DB_EXPORTS
#ifdef DB_EXPORTS  
#define DB __declspec(dllexport)   
#else  
#define DB __declspec(dllimport)   
#endif 

#include <memory>
#include <string>
#include <exception>
#include <stdexcept>
#include <map>
#include <vector>
#include <variant>

struct sqlite3_stmt;
struct sqlite3;

namespace db
{
	class db_connection;
	class prepared_statement;
	struct resultset;

	using db_connection_ptr = std::unique_ptr<db_connection>;
	using prepared_statement_ptr = std::shared_ptr<prepared_statement>;
	//	using row_t = std::vector<void*>;
	//	using resultset_ptr = std::shared_ptr<resultset>;
	using value_t = std::variant<std::monostate, int, double, std::string>;

	class DB db_exception : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	class DB db_connection
	{
	public:
		static db_connection_ptr create(std::string dbname);
		prepared_statement_ptr prepare(std::string sql);
		void connect(std::string dbname);
		void exec_sql(std::string sql);
		~db_connection();		// destructor
	protected:
		db_connection () : connection{ nullptr } {};
	private:
		void init(std::string dbname);

		std::string name;
		sqlite3* connection;
	};

	struct DB value
	{
		enum class Type { NULL_T, STRING, INTEGER, DOUBLE, BLOB };
		Type type;
		std::string string_value;
		int integer_value;
		double double_value;
		char* blob_value;

		value() : type{ Type::NULL_T }, string_value{ "" }, integer_value{ 0 }, double_value{ 0.0f }, blob_value{ nullptr } {}
		~value()
		{
			if (blob_value != nullptr)
			{
				delete[] blob_value;
			}
		}
	};

	//using row_t = std::map<std::string, value>;
	using row_t = std::map<std::string, value_t>;
	using table_data_t = std::vector<row_t>;

	enum class Result_code {Success, Row, Error};
	class DB prepared_statement
	{
	public:
		static prepared_statement_ptr create();
		void bind(int index, value_t value);
	/*	void bind(int index, std::string value);
		void bind(int index, int value);
		void bind(int index, double value);
		void bind_null(int index);*/
		void reset();
		Result_code execute_row();
		std::string column_name(int index);
		std::string fetch_string(int index);
		int fetch_int(int index);
		row_t fetch_row();
		table_data_t fetch_table();
		friend prepared_statement_ptr db_connection::prepare(std::string sql);
		~prepared_statement();
	protected:
		prepared_statement() = default;
	private:
		void set_statement(sqlite3_stmt* prep_statement);

		sqlite3_stmt* statement;
	};

	/*	struct resultset
	{
	std::vector<int> column_types;
	std::vector<row_t> tabledata;
	};*/
}