#include "DBOperations.h"

bool DBOperations::check_name(char *name)
{
    auto clients_db = db["clients"];

    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = clients_db.find_one(document{} << "name" << name << finalize);
	if (maybe_result)
	{
		std::cout << bsoncxx::to_json(*maybe_result) << "\n";
		return true;
	}
	return false;
}

bool DBOperations::authenticate(char *name, char *password)
{
    auto clients_db = db["clients"];

	bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = clients_db.find_one(document{}
																						 << "name" << name
																						 << "password" << password
																						 << finalize);

	cout << "\033[;33m\nIn Authentication  \033[0m\n";
	if (maybe_result)
	{
		std::cout << bsoncxx::to_json(*maybe_result) << "\n";
		return true;
	}
	return false;
}

void DBOperations::registerClient(char *name, char *password)
{
    auto builder = bsoncxx::builder::stream::document{};
    auto clients_db = db["clients"];

    bsoncxx::document::value doc_value = builder
                                            << "name" << name
                                            << "password" << password
                                            << "status"
                                            << "offline"
                                            << finalize;
    clients_db.insert_one(doc_value.view());
}

void DBOperations::storeGroupMessages(char *name, char *buff_out) 
{
    time_t now = time(0);
	auto builder = bsoncxx::builder::stream::document{};
    auto chat_room_messages = db["chatroom"];

    bsoncxx::document::value doc_value = builder
                                        << "from" << name
                                        << "message" << buff_out
                                        << "time" << ctime(&now)
                                        << finalize;
chat_room_messages.insert_one(doc_value.view());
}

void DBOperations::storePrivateMessages(char *senderName, char *receiverName, char* buff_out)
{
    time_t now = time(0);
	auto builder = bsoncxx::builder::stream::document{};
    auto client_to_client_messages = db["singleton"];

    	bsoncxx::document::value doc_value = builder
                                            << "from" << senderName
                                            << "to" << receiverName
                                            << "message" << buff_out
                                            << "time" << ctime(&now)
                                            << finalize;
    client_to_client_messages.insert_one(doc_value.view());
}