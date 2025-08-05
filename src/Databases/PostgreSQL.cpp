#include <iostream>
#include <fmt/format.h>
#include <Databases/PostgreSQL.h>

// Example from official libpxx github
int PostgreSQL::createTable()
{
	try
	{
		// Connect to the database.  You can have multiple connections open
		// at the same time, even to the same database.
		pqxx::connection c;
		std::cout << "Connected to " << c.dbname() << '\n';

		// Start a transaction.  A connection can only have one transaction
		// open at the same time, but after you finish a transaction, you
		// can start a new one on the same connection.
		pqxx::work tx{c};

		// Query data of two columns, converting them to std::string and
		// int respectively.  Iterate the rows.
		for (auto [name, salary] : tx.query<std::string, int>(
			"SELECT name, salary FROM employee ORDER BY name"))
		{
			std::cout << name << " earns " << salary << ".\n";
		}

		// For large amounts of data, "streaming" the results is more
		// efficient.  It does not work for all types of queries though.
		//
		// You can read fields as std::string_view here, which is not
		// something you can do in most places.  A string_view becomes
		// meaningless when the underlying string ceases to exist.  In this
		// one situation, you can convert a field to string_view and it
		// will be valid for just that one iteration of the loop.  The next
		// iteration may overwrite or deallocate its buffer space.
		for (auto [name, salary] : tx.stream<std::string_view, int>(
			"SELECT name, salary FROM employee"))
		{
			std::cout << name << " earns " << salary << ".\n";
		}

		// Execute a statement, and check that it returns 0 rows of data.
		// This will throw pqxx::unexpected_rows if the query returns rows.
		std::cout << "Doubling all employees' salaries...\n";
		tx.exec0("UPDATE employee SET salary = salary*2");

		// Shorthand: conveniently query a single value from the database.
		int my_salary = tx.query_value<int>(
			"SELECT salary FROM employee WHERE name = 'Me'");
		std::cout << "I now earn " << my_salary << ".\n";

		// Or, query one whole row.  This function will throw an exception
		// unless the result contains exactly 1 row.
		auto [top_name, top_salary] = tx.query1<std::string, int>(
			R"(
				SELECT name, salary
				FROM employee
				WHERE salary = max(salary)
				LIMIT 1
			)");
		std::cout << "Top earner is " << top_name << " with a salary of "
					<< top_salary << ".\n";

		// If you need to access the result metadata, not just the actual
		// field values, use the "exec" functions.  Most of them return
		// pqxx::result objects.
		pqxx::result res = tx.exec("SELECT * FROM employee");
		std::cout << "Columns:\n";
		for (pqxx::row_size_type col = 0; col < res.columns(); ++col)
			std::cout << res.column_name(col) << '\n';

		// Commit the transaction.  If you don't do this, the database will
		// undo any changes you made in the transaction.
		std::cout << "Making changes definite: ";
		tx.commit();
		std::cout << "OK.\n";
	}
	catch (std::exception const &e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		return 1;
	}
	return 0;
}

PostgreSQL::PostgreSQL(
	std::string dbname_,
	std::string user_,
	std::string password_,
	std::string host_,
	std::string port_)
	: dbname(std::move(dbname_))
	, user(std::move(user_))
	, password(std::move(password_))
	, host(std::move(host_))
	, port(std::move(port_))
{}

pqxx::connection PostgreSQL::openConnection()
{
	// Connect to the database.  You can have multiple connections open
	// at the same time, even to the same database.
	// dbname=knowledgebase \
		user={postgres} \
		password=postgres \
		host=localhost \
		port=5432 \
		target_session_attrs=read-write
	pqxx::connection conn(
		fmt::format(
			"dbname={} user={} password={} host={} port={} target_session_attrs=read-write", 
			dbname, user, password, host, port));
	std::cout << "Connected to " << conn.dbname() << '\n';
	return conn;
}

void PostgreSQL::executeQueries(std::vector<std::string> & queries)
{
	try
	{
		pqxx::connection conn = openConnection();
		pqxx::work txn{conn};
		for (std::string & query : queries)
		txn.exec(query);
    	txn.commit();
	}
	catch (std::exception const &e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
	}
}

void PostgreSQL::createTables()
{
	std::vector<std::string> queries;
	queries.emplace_back(
		"CREATE TABLE IF NOT EXISTS users ( \
		id serial PRIMARY KEY, \
		name varchar(30), \
		email varchar(50), \
		password varchar(20), \
		UNIQUE(name))");
	queries.emplace_back(
		"INSERT INTO users VALUES \
		(DEFAULT, 'one', 'one@yandex.ru', 'password_one'), \
		(DEFAULT, 'two', 'two@yandex.ru', 'password_two'), \
		(DEFAULT, 'three', 'three@yandex.ru', 'password_three')");
	queries.emplace_back(
		"CREATE TABLE IF NOT EXISTS categories ( \
		id serial PRIMARY KEY, \
		name varchar(30), \
		UNIQUE(name))");
	queries.emplace_back(
		"CREATE TABLE IF NOT EXISTS tracks ( \
		id serial PRIMARY KEY, \
		name VARCHAR(30), \
		author VARCHAR(50), \
		\"like\" BOOLEAN NOT NULL, \
		station varchar(30), \
		UNIQUE(name))");
	executeQueries(queries);
}

void PostgreSQL::fillTables()
{
	std::vector<std::string> queries;
	queries.emplace_back(
		"INSERT INTO users VALUES \
		(DEFAULT, 'one', 'one@yandex.ru', 'password_one'), \
		(DEFAULT, 'two', 'two@yandex.ru', 'password_two'), \
		(DEFAULT, 'three', 'three@yandex.ru', 'password_three')");
	queries.emplace_back(
		"INSERT INTO categories VALUES \
		(DEFAULT, 'bard'), \
		(DEFAULT, 'classic'), \
		(DEFAULT, 'jazz'), \
		(DEFAULT, 'metal'), \
		(DEFAULT, 'pank'), \
		(DEFAULT, 'pop'), \
		(DEFAULT, 'rock')");
	queries.emplace_back(
		"INSERT INTO tracks VALUES \
		(DEFAULT, 'Sonne', 'Rammstein', TRUE, 'metal'), \
		(DEFAULT, 'Still Loving You', 'Scorpions', FALSE, 'jazz')");
	executeQueries(queries);
}

int PostgreSQL::insertNewUser(std::string name, std::string email, std::string password)
{
	try
	{
		pqxx::connection conn = openConnection();
		pqxx::work txn{conn};
		txn.exec(fmt::format("INSERT INTO users VALUES (DEFAULT, '{}', '{}', '{}')", name, email, password));
    	txn.commit();

	}
	catch (std::exception const &e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		return -1;
	}
	return 0;
}

bool PostgreSQL::searchUser(std::string name, std::string password)
{
	try
	{
		pqxx::connection conn = openConnection();
		pqxx::work txn{conn};
		int cnt = txn.query_value<int>(
			fmt::format("SELECT count(*) FROM users WHERE name='{}' AND password='{}'", name, password));
    	txn.commit();
		return cnt ? true : false;
	}
	catch (std::exception const &e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		return false;
	}
}

void PostgreSQL::getCategories(std::vector<std::string> & categories)
{
	try
	{
		pqxx::connection conn = openConnection();
		pqxx::work txn{conn};
		for (auto name : txn.query<std::string>("SELECT name FROM categories;"))
			categories.emplace_back(std::get<0>(name));
    	txn.commit();
	}
	catch (std::exception const &e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
	}
}

void PostgreSQL::getCategoryTracks(std::string & category, std::string & tracks)
{
	try
	{
		tracks = "{\"user\": \"user\", \"tracks\": [";
		std::string track;
		pqxx::connection conn = openConnection();
		pqxx::work txn{conn};
		for (auto [id, name, author] : txn.query<int, std::string, std::string>(
			fmt::format("SELECT id, name, author FROM tracks WHERE station=\'{}\';", category)))
		{
			track = fmt::format("{{\"id\": {}, \"name\": \"{}\", \"author\": \"{}\", \"like\": []}},", id, name, author);
			tracks += track;
		}
		tracks.pop_back();
        tracks += "]}";
    	txn.commit();
	}
	catch (std::exception const &e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
	}
}