#include <iostream>
#include <fstream>
#include <fmt/format.h>
#include <Databases/PostgreSQL.h>
#include <spdlog/spdlog.h>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/error/en.h>

// Example from official libpxx github
int PostgreSQL::createTable()
{
	try
	{
		// Connect to the database.  You can have multiple connections open
		// at the same time, even to the same database.
		pqxx::connection c;
		spdlog::info("Connected to {}", c.dbname());

		// Start a transaction.  A connection can only have one transaction
		// open at the same time, but after you finish a transaction, you
		// can start a new one on the same connection.
		pqxx::work tx{c};

		// Query data of two columns, converting them to std::string and
		// int respectively.  Iterate the rows.
		for (auto [name, salary] : tx.query<std::string, int>(
				 "SELECT name, salary FROM employee ORDER BY name"))
		{
			spdlog::info("{} earns {}", name, salary);
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
			spdlog::info("{} earns {}", name, salary);
		}

		// Execute a statement, and check that it returns 0 rows of data.
		// This will throw pqxx::unexpected_rows if the query returns rows.
		spdlog::info("Doubling all employees' salaries...");
		tx.exec("UPDATE employee SET salary = salary*2");

		// Shorthand: conveniently query a single value from the database.
		int my_salary = tx.query_value<int>(
			"SELECT salary FROM employee WHERE name = 'Me'");
		spdlog::info("I now earn {}", my_salary);

		// Or, query one whole row.  This function will throw an exception
		// unless the result contains exactly 1 row.
		auto [top_name, top_salary] = tx.query1<std::string, int>(
			R"(
				SELECT name, salary
				FROM employee
				WHERE salary = max(salary)
				LIMIT 1
			)");
		spdlog::info("Top earner is {} with a salary of {}", top_name, top_salary);

		// If you need to access the result metadata, not just the actual
		// field values, use the "exec" functions.  Most of them return
		// pqxx::result objects.
		pqxx::result res = tx.exec("SELECT * FROM employee");
		spdlog::info("Columns:");
		for (pqxx::row_size_type col = 0; col < res.columns(); ++col)
			spdlog::info("{}", res.column_name(col));

		// Commit the transaction.  If you don't do this, the database will
		// undo any changes you made in the transaction.
		spdlog::info("Making changes definite: ");
		tx.commit();
		spdlog::info("OK.");
	}
	catch (std::exception const &e)
	{
		spdlog::error("ERROR: {}", e.what());
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
	: dbname(std::move(dbname_)), user(std::move(user_)), password(std::move(password_)), host(std::move(host_)), port(std::move(port_))
{
}

pqxx::connection PostgreSQL::openConnection()
{
	/*
	 * Connection string format:
	 * dbname=knowledgebase
	 * user=postgres
	 * password=postgres
	 * host=localhost
	 * port=5432
	 * target_session_attrs=read-write
	 */
	pqxx::connection conn(
		fmt::format(
			"dbname={} user={} password={} host={} port={} target_session_attrs=read-write",
			dbname, user, password, host, port));
	spdlog::info("Connected to {}", conn.dbname());
	return conn;
}

void PostgreSQL::executeQueries(std::vector<std::string> &queries)
{
	try
	{
		pqxx::connection conn = openConnection();
		pqxx::work txn{conn};
		for (std::string &query : queries)
			txn.exec(query);
		txn.commit();
	}
	catch (std::exception const &e)
	{
		spdlog::error("ERROR: {}", e.what());
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
	// queries.emplace_back(
	// 	"INSERT INTO users VALUES \
	// 	(DEFAULT, 'one', 'one@yandex.ru', 'password_one'), \
	// 	(DEFAULT, 'two', 'two@yandex.ru', 'password_two'), \
	// 	(DEFAULT, 'three', 'three@yandex.ru', 'password_three')");
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

bool PostgreSQL::fillTables(const std::string &configPath)
{
	try
	{
		// Check if tables are already populated
		pqxx::connection conn = openConnection();
		pqxx::work txn{conn};

		// Check if categories table already has data
		int categoryCount = txn.query_value<int>("SELECT COUNT(*) FROM categories");
		int trackCount = txn.query_value<int>("SELECT COUNT(*) FROM tracks");

		// If tables already have data, don't fill again
		if (categoryCount > 0 || trackCount > 0)
		{
			spdlog::info("Tables already populated. Skipping fillTables().");
			return false;
		}

		std::vector<std::string> queries;

		if (configPath.empty())
		{
			// Use default data if no config file provided - single query for all categories
			queries.emplace_back(
				"INSERT INTO categories VALUES \
                (DEFAULT, 'bard'), \
                (DEFAULT, 'classic'), \
                (DEFAULT, 'jazz'), \
                (DEFAULT, 'metal'), \
                (DEFAULT, 'pank'), \
                (DEFAULT, 'pop'), \
                (DEFAULT, 'rock')");

			// Single query for all tracks
			queries.emplace_back(
				"INSERT INTO tracks VALUES \
                (DEFAULT, 'Sonne', 'Rammstein', TRUE, 'metal'), \
                (DEFAULT, 'Still Loving You', 'Scorpions', FALSE, 'jazz')");
		}
		else
		{
			// Read data from config file using RapidJSON
			std::ifstream configFile(configPath);
			if (!configFile.is_open())
			{
				spdlog::error("ERROR: Could not open config file: {}", configPath);
				return false;
			}

			rapidjson::IStreamWrapper isw(configFile);
			rapidjson::Document doc;
			doc.ParseStream(isw);

			if (doc.HasParseError())
			{
				spdlog::error("JSON parse error: {} (offset: {})",
							  rapidjson::GetParseError_En(doc.GetParseError()), doc.GetErrorOffset());
				return false;
			}

			// Parse categories from config - build single bulk INSERT
			if (doc.HasMember("categories") && doc["categories"].IsArray())
			{
				const rapidjson::Value &categories = doc["categories"];
				if (categories.Size() > 0)
				{
					std::string categoriesQuery = "INSERT INTO categories VALUES ";
					bool firstCategory = true;

					for (rapidjson::SizeType i = 0; i < categories.Size(); i++)
					{
						if (categories[i].IsString())
						{
							std::string categoryName = categories[i].GetString();
							// Escape single quotes for SQL
							std::string escapedName;
							for (char c : categoryName)
							{
								if (c == '\'')
									escapedName += "''";
								else
									escapedName += c;
							}

							if (!firstCategory)
								categoriesQuery += ", ";
							categoriesQuery += fmt::format("(DEFAULT, '{}')", escapedName);
							firstCategory = false;
						}
					}

					if (!firstCategory) // Only add if we have at least one category
						queries.emplace_back(categoriesQuery);
				}
			}

			// Parse tracks from config - build single bulk INSERT
			if (doc.HasMember("tracks") && doc["tracks"].IsArray())
			{
				const rapidjson::Value &tracks = doc["tracks"];
				if (tracks.Size() > 0)
				{
					std::string tracksQuery = "INSERT INTO tracks VALUES ";
					bool firstTrack = true;

					for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
					{
						const rapidjson::Value &track = tracks[i];
						if (track.IsObject() &&
							track.HasMember("name") && track["name"].IsString() &&
							track.HasMember("author") && track["author"].IsString() &&
							track.HasMember("station") && track["station"].IsString())
						{
							// Get and escape string values
							auto escapeString = [](const rapidjson::Value &value) -> std::string
							{
								std::string result;
								std::string str = value.GetString();
								for (char c : str)
								{
									if (c == '\'')
										result += "''";
									else
										result += c;
								}
								return result;
							};

							std::string name = escapeString(track["name"]);
							std::string author = escapeString(track["author"]);
							std::string station = escapeString(track["station"]);

							// Get like value (default to false if not present)
							bool like = false;
							if (track.HasMember("like") && track["like"].IsBool())
							{
								like = track["like"].GetBool();
							}

							if (!firstTrack)
								tracksQuery += ", ";
							tracksQuery += fmt::format("(DEFAULT, '{}', '{}', {}, '{}')",
													   name, author, like ? "TRUE" : "FALSE", station);
							firstTrack = false;
						}
					}

					if (!firstTrack) // Only add if we have at least one track
						queries.emplace_back(tracksQuery);
				}
			}

			configFile.close();
		}

		executeQueries(queries);
		spdlog::info("Tables populated successfully with {} bulk queries.", queries.size());
		return true;
	}
	catch (const std::exception &e)
	{
		spdlog::error("ERROR in fillTables: {}", e.what());
		return false;
	}
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
		spdlog::error("ERROR: {}", e.what());
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
		spdlog::error("ERROR: {}", e.what());
		return false;
	}
}

void PostgreSQL::getCategories(std::vector<std::string> &categories)
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
		spdlog::error("ERROR: {}", e.what());
	}
}

void PostgreSQL::getCategoryTracks(std::string &category, std::string &tracks)
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
		spdlog::error("ERROR: {}", e.what());
	}
}