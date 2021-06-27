  



#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
namespace despot {
    class MongoDB_Bridge
    {
	public:
        static void Init();

        static bsoncxx::document::view WaitForModuleResponse(std::string moduleName);
        static void SendActionToExecution(std::string actionName, std::vector<std::string> parameters, std::vector<std::string> parameterNames);

        static bool isInit;
        static mongocxx::instance instance; // This should be done only once.
        static mongocxx::uri uri;
        static mongocxx::client client;

        static mongocxx::database db;
        static mongocxx::collection actionToExecuteCollection;
        static mongocxx::collection moduleResponseColllection;
};
}