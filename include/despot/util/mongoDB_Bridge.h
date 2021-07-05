  



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

        static std::vector<bsoncxx::document::view> WaitForActionResponse(std::string moduleName);
        static void SendActionToExecution(int actionId, std::string actionName, std::string actionParameters);
        static void RegisterAction(int actionId, std::string actionName, std::string actionParameters);
        static void UpdateActionResponse(std::string actionName, std::string actionResponse);
        

        static bool isInit;
        static mongocxx::instance instance; // This should be done only once.
        static mongocxx::uri uri;
        static mongocxx::client client;

        static mongocxx::database db;
        static mongocxx::collection actionToExecuteCollection;
        static mongocxx::collection actionsCollection;
        static mongocxx::collection moduleResponseColllection;
        static mongocxx::collection localVariableColllection;
       
};
}