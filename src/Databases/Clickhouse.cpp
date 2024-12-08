#include <clickhouse/client.h>
#include <Databases/Clickhouse.h>
#include <iostream>

using namespace clickhouse;


void Clickhouse::createTableUser()
{
    /// Initialize client connection.
    clickhouse::Client client(ClientOptions().SetHost("localhost").SetPassword("123"));

    /// Create a database
    client.Execute("CREATE DATABASE IF NOT EXISTS knowledgebase ENGINE = Memory");
    /// Create a table
    client.Execute("CREATE TABLE IF NOT EXISTS knowledgebase.users (\
                        id UInt64,\
                        name String,\
                        email String,\
                        password String)\
                    ENGINE = Memory");

    /// Insert some values.
    {
        Block block;

        auto id = std::make_shared<ColumnUInt64>();
        id->Append(1);
        id->Append(2);
        id->Append(3);

        auto name = std::make_shared<ColumnString>();
        name->Append("one");
        name->Append("two");
        name->Append("three");

        auto email = std::make_shared<ColumnString>();
        email->Append("one@yandex.ru");
        email->Append("two@yandex.ru");
        email->Append("three@yandex.ru");

        auto password = std::make_shared<ColumnString>();
        password->Append("password_one");
        password->Append("password_two");
        password->Append("password_three");

        block.AppendColumn("id"  , id);
        block.AppendColumn("name", name);
        block.AppendColumn("email", email);
        block.AppendColumn("password", password);

        client.Insert("knowledgebase.users", block);
    }

    /// Select values inserted in the previous step.
    client.Select("SELECT * FROM knowledgebase.users", [] (const Block& block) {
        for (size_t i = 0; i < block.GetRowCount(); ++i) 
        {
            std::cout << block[0]->As<ColumnUInt64>()->At(i) << " "
                        << block[1]->As<ColumnString>()->At(i) << "\n";
        }
    });

    /// Delete table.
    client.Execute("DROP TABLE knowledgebase.users");
}