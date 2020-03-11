/*#pragma once  
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
#include <unordered_map>
#include <vector>
#include <variant>

struct sqlite3_stmt;
struct sqlite3;

namespace db
{
	using namespace std::string_literals;
	struct DB Blob
	{
		Blob() : size{ 0 }, value{ nullptr }{}
		Blob(const Blob& other) noexcept;
		Blob& operator=(const Blob& other) noexcept;
		Blob(Blob&& other) noexcept;
		Blob& operator=(Blob&& other) noexcept;
		~Blob() noexcept;

		size_t size;
		void* value;
	};

	class Prepared_statement;
	class DB DB_connection
	{
	public:
		enum class Mode { Read_only, Read_write, Create };

		DB_connection() : m_path{ ""s }, m_connection{ nullptr }, m_mode{ Mode::Read_only } {}
		explicit DB_connection(std::string path, Mode mode = Mode::Read_only);
		DB_connection(const DB_connection& other);
		DB_connection& operator= (DB_connection other);
		DB_connection(DB_connection&& other) noexcept;
		DB_connection& operator= (DB_connection&& other) noexcept;
		~DB_connection() noexcept;
		friend void swap(DB_connection& first, DB_connection& second) noexcept
		{
			using std::swap;
			swap(first.m_path, second.m_path);
			swap(first.m_mode, second.m_mode);
			swap(first.m_connection, second.m_connection);
		}
		Prepared_statement prepare(std::string sql);
	private:
		void close() noexcept;

		std::string m_path;
		Mode m_mode;
		sqlite3* m_connection;
	};

	using value_t = std::variant<std::monostate, int, double, std::string, Blob>;
	using row_t = std::unordered_map<std::string, value_t>;
	using table_t = std::vector<row_t>;

	class Prepared_statement
	{
	public:
		enum class Row_result { Success, Row, Error };

		Prepared_statement() : m_statement{ nullptr } {}
		explicit Prepared_statement(sqlite3_stmt* stmt) : m_statement{ stmt } {}
		Prepared_statement(const Prepared_statement& other) = delete;
		Prepared_statement& operator=(Prepared_statement& other) = delete;
		Prepared_statement(Prepared_statement&& other) noexcept;
		Prepared_statement& operator=(Prepared_statement&& other) noexcept;
		~Prepared_statement() noexcept;

		void bind(int index, value_t value);
		Row_result execute_row();
		row_t fetch_row();
		table_t fetch_table();
		void reset() noexcept;
	private:
		sqlite3_stmt* m_statement;
	};

	class db_connection;
	class prepared_statement;
	struct resultset;

	using db_connection_ptr = std::unique_ptr<db_connection>;
	using prepared_statement_ptr = std::shared_ptr<prepared_statement>;
	//using value_t = std::variant<std::monostate, int, double, std::string>;

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
	};*/

/*	struct DB value
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
	};*/

	//using row_t = std::map<std::string, value>;
//	using row_t = std::map<std::string, value_t>;
/*	using table_data_t = std::vector<row_t>;

	enum class Result_code {Success, Row, Error};
	class DB prepared_statement
	{
	public:
		static prepared_statement_ptr create();
		void bind(int index, value_t value);
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
	};*/

	/*	struct resultset
	{
	std::vector<int> column_types;
	std::vector<row_t> tabledata;
	};
}*/