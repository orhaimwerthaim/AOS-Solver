
#include <despot/util/mongoDB_Bridge.h>
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


using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

namespace despot {
  bool MongoDB_Bridge::isInit = false;
  mongocxx::client MongoDB_Bridge::client;
  mongocxx::instance MongoDB_Bridge::instance;
  mongocxx::database MongoDB_Bridge::db;
  mongocxx::collection MongoDB_Bridge::actionToExecuteCollection;
  mongocxx::collection MongoDB_Bridge::moduleResponseColllection;
  void MongoDB_Bridge::Init()
  {
    if (!MongoDB_Bridge::isInit)
    {
      MongoDB_Bridge::isInit = true;
       
       // MongoDB_Bridge::instance = mongocxx::instance{};
      mongocxx::uri uri("mongodb://localhost:27017");
      MongoDB_Bridge::client = mongocxx::client(uri);

      MongoDB_Bridge::db = MongoDB_Bridge::client["AOS"];
      MongoDB_Bridge::actionToExecuteCollection = MongoDB_Bridge::db["ActionsForExecution"];
      MongoDB_Bridge::moduleResponseColllection = MongoDB_Bridge::db["ModuleResponses"];
    }
}

bsoncxx::document::view MongoDB_Bridge::WaitForModuleResponse(std::string moduleName)
{
  MongoDB_Bridge::Init();
  auto filter = document{} << "wasRead" << false << "module" << moduleName << finalize;
   

  while (true)
  {
    mongocxx::cursor cursor = MongoDB_Bridge::moduleResponseColllection.find({filter});
    for(auto doc : cursor) {
    {
      MongoDB_Bridge::moduleResponseColllection.update_one(doc, document{} << "$set" << open_document <<
                        "wasRead" << true << close_document << finalize);
      return *(&doc);
    }
}
  }
  return (document{} << "null" << true << finalize).view();
}

void MongoDB_Bridge::SendActionToExecution(std::string actionName, std::vector<std::string> parameterValues, std::vector<std::string> parameterNames)
{
  MongoDB_Bridge::Init(); 
  auto now = std::chrono::system_clock::now();
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = builder << "actionName" << actionName
  << "RequestCreateTime" << bsoncxx::types::b_date(now)
  << "wasHandled" << false


  
  << "HandleTime" << bsoncxx::types::b_date(now - std::chrono::hours(1)) 
  << "parameters" << open_array
<< [&](bsoncxx::builder::stream::array_context<> arr) {
        for (int i = 0; i < parameterValues.size(); i++)
        {
            arr << open_document << parameterNames[i] << parameterValues[i] << close_document;
        }
    } << close_array << finalize;

 
  MongoDB_Bridge::actionToExecuteCollection.insert_one(doc_value.view());
}
} 