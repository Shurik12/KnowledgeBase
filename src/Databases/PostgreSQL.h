class PostgreSQL 
{
public:
	PostgreSQL() = default;
	~PostgreSQL() = default;

	static int createTable();
	static int createTableUser();
	static int insertNewUser(std::string name, std::string email, std::string password);
	static bool searchUser(std::string name, std::string password);
};