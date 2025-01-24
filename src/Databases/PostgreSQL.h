#include <pqxx/pqxx>

class PostgreSQL 
{
public:
	PostgreSQL() = default;
	~PostgreSQL() = default;

	int createTable();
	
	static pqxx::connection openConnection();
	static void createTables();
	static void fillTables();

	static void executeQueries(std::vector<std::string> & queries);
	static void getCategories(std::vector<std::string> & categories);
	static void getCategoryTracks(std::string & category, std::string & tracks);

	static int insertNewUser(std::string name, std::string email, std::string password);
	static bool searchUser(std::string name, std::string password);
};