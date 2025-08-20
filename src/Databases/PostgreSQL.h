#include <cstdlib>
#include <pqxx/pqxx>

class PostgreSQL 
{
public:
	PostgreSQL() = default;
	PostgreSQL(
		std::string dbname_,
		std::string user_,
		std::string password_,
		std::string host_,
		std::string port_);
	~PostgreSQL() = default;

	int createTable();
	
	pqxx::connection openConnection();
	void createTables();
	bool fillTables(const std::string& configPath = "");

	void executeQueries(std::vector<std::string> & queries);
	void getCategories(std::vector<std::string> & categories);
	void getCategoryTracks(std::string & category, std::string & tracks);

	int insertNewUser(std::string name, std::string email, std::string password);
	bool searchUser(std::string name, std::string password);

	std::string dbname;
	std::string user;
	std::string password;
	std::string host;
	std::string port;
};