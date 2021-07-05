
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
#include <sstream>

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
  mongocxx::collection MongoDB_Bridge::localVariableColllection;
  mongocxx::collection MongoDB_Bridge::actionsCollection;

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
      MongoDB_Bridge::localVariableColllection = MongoDB_Bridge::db["localVariables"];
      MongoDB_Bridge::actionsCollection = MongoDB_Bridge::db["Actions"];
    }
}

void MongoDB_Bridge::UpdateActionResponse(std::string actionName, std::string actionResponse)
{
  auto filter = document{} << "wasRead" << false << "module" << actionName << finalize;
  //auto update = document{} << "$set" << open_document << "wasRead" << true << "moduleResponseText" << actionResponse << close_document << finalize;
  std::stringstream ss;
  ss<< "{\"$set\" : {\"wasRead\":true, \"moduleResponseText\" : \"" << actionResponse << "\"}}"; 
  
  MongoDB_Bridge::moduleResponseColllection.update_one(filter.view(), bsoncxx::from_json(ss.str()));
}

std::vector<bsoncxx::document::view> MongoDB_Bridge::WaitForActionResponse(std::string actionName)
{
  std::vector<bsoncxx::document::view> moduleLocalVars;
  MongoDB_Bridge::Init();
  auto filter = document{} << "wasRead" << false << "module" << actionName << finalize;
  bool actionFinished = false;

  while (!actionFinished)
  {
    mongocxx::cursor cursor = MongoDB_Bridge::moduleResponseColllection.find({filter});
    for(auto doc : cursor) 
    {
      actionFinished = true;
      //MongoDB_Bridge::moduleResponseColllection.update_one(doc, document{} << "$set" << open_document <<
      //                "wasRead" << true << close_document << finalize);

      //return *(&doc);

    }
    if(actionFinished)
    {
        auto filter2 = document{} << "module" << actionName << finalize;
        mongocxx::cursor cursor2 = MongoDB_Bridge::localVariableColllection.find({filter2});
        for(auto doc : cursor2) 
        {
          moduleLocalVars.push_back(doc);
        }
        return *(&moduleLocalVars);
    }
  }
}

void MongoDB_Bridge::SendActionToExecution(int actionId, std::string actionName, std::string actionParameters)
{
  MongoDB_Bridge::Init(); 
  auto now = std::chrono::system_clock::now();
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = !actionParameters.empty() ? (
                                                                       builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "RequestCreateTime" << bsoncxx::types::b_date(now)
                                                                               << "WasHandled" << false
                                                                               << "HandleTime" << bsoncxx::types::b_date(now - std::chrono::hours(1))
                                                                               << "Parameters" << open_array
                                                                               << [&](bsoncxx::builder::stream::array_context<> arr)
                                                                       { arr << bsoncxx::from_json(actionParameters); }
                                                                               << close_array << finalize)
                                                                 : (
                                                                       builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "RequestCreateTime" << bsoncxx::types::b_date(now)
                                                                               << "WasHandled" << false
                                                                               << "HandleTime" << bsoncxx::types::b_date(now - std::chrono::hours(1))
                                                                               << "Parameters" << open_array
                                                                               << close_array << finalize);


  MongoDB_Bridge::actionToExecuteCollection.insert_one(doc_value.view());
}

void MongoDB_Bridge::RegisterAction(int actionId, std::string actionName, std::string actionParameters)
{
  MongoDB_Bridge::Init(); 
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = !actionParameters.empty() ? (
                                                                       builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "ActionConstantParameters" << open_array
                                                                               << [&](bsoncxx::builder::stream::array_context<> arr)
                                                                       { arr << bsoncxx::from_json(actionParameters); }
                                                                               << close_array << finalize)
                                                                 : (
                                                                       builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "ActionConstantParameters" << open_array
                                                                               << close_array << finalize);
auto filter = document{} << "ActionID" << actionId << finalize;
  mongocxx::options::replace option;
  option.upsert(true);
  MongoDB_Bridge::actionsCollection.replace_one(filter.view(), doc_value.view(), option);
}
} 

//Generate document with array dynamically
/*
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
*/