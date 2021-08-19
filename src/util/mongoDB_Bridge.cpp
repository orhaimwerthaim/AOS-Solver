
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
#include <unistd.h>

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
  mongocxx::collection MongoDB_Bridge::globalVariablesAssignmentsColllection;

  mongocxx::collection MongoDB_Bridge::beliefStatesColllection;
  int MongoDB_Bridge::currentActionSequenceId = 1;
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
      MongoDB_Bridge::globalVariablesAssignmentsColllection = MongoDB_Bridge::db["GlobalVariablesAssignments"];
      MongoDB_Bridge::localVariableColllection = MongoDB_Bridge::db["localVariables"];
      MongoDB_Bridge::actionsCollection = MongoDB_Bridge::db["Actions"];

      MongoDB_Bridge::beliefStatesColllection = MongoDB_Bridge::db["BeliefStates"];
    }
}

// void MongoDB_Bridge::UpdateActionResponse(std::string actionName, std::string actionResponse)
// {
//   auto filter = document{} << "wasRead" << false << "Module" << actionName << finalize;
//   //auto update = document{} << "$set" << open_document << "wasRead" << true << "moduleResponseText" << actionResponse << close_document << finalize;
//   std::stringstream ss;
//   ss<< "{\"$set\" : {\"wasRead\":true, \"moduleResponseText\" : \"" << actionResponse << "\"}}"; 
  
//   MongoDB_Bridge::moduleResponseColllection.update_one(filter.view(), bsoncxx::from_json(ss.str()));
// }

std::map<std::string, bool> MongoDB_Bridge::WaitForActionResponse(bsoncxx::oid actionForExecuteId, std::string& actionTextObservation)
{

  std::map<std::string, bool> globalVarUpdates;
  std::vector<bsoncxx::document::view> moduleLocalVars;
  MongoDB_Bridge::Init();
  auto filter = document{} << "ActionForExecutionId" << actionForExecuteId << finalize;
  bool actionFinished = false;

  while (!actionFinished)
  {
    mongocxx::cursor cursor = MongoDB_Bridge::moduleResponseColllection.find({filter});
    for(auto doc : cursor) 
    {
      actionFinished = true;
      actionTextObservation = doc["ModuleResponseText"].get_utf8().value.to_string();
      // bsoncxx::document::element element2 = res["responseText"];
      // auto s = element2.get_utf8().value;
      // std::string str = s.to_string();

      // MongoDB_Bridge::moduleResponseColllection.update_one(doc, document{} << "$set" << open_document <<
      //                "wasRead" << true << close_document << finalize);

      //return *(&doc);

    }
    if(actionFinished)
    {
      usleep(500000);
        auto filter2 = document{} << "UpdatingActionSequenceId" << MongoDB_Bridge::currentActionSequenceId << finalize;
        mongocxx::cursor cursor2 = MongoDB_Bridge::globalVariablesAssignmentsColllection.find({filter2});
        for(auto doc : cursor2) 
        {
          std::string globalVariableName = doc["GlobalVariableName"].get_utf8().value.to_string();
          bool isInit = doc["IsInitialized"].get_bool();
          globalVarUpdates[globalVariableName] = isInit;
        }
        break;
    }
  }

  MongoDB_Bridge::currentActionSequenceId++;
  return globalVarUpdates;
}

bsoncxx::oid MongoDB_Bridge::SendActionToExecution(int actionId, std::string actionName, std::string actionParameters)
{
  MongoDB_Bridge::Init(); 
  auto now = std::chrono::system_clock::now();
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = !actionParameters.empty() ? (
                                                                //        builder << "ActionID" << actionId 
                                                                //                << "ActionName" << actionName
                                                                //                << "ActionSequenceId" << MongoDB_Bridge::currentActionSequenceId
                                                                //                << "RequestCreateTime" << bsoncxx::types::b_date(now)
                                                                //                << "Parameters" << open_array
                                                                //                << [&](bsoncxx::builder::stream::array_context<> arr)
                                                                //        { arr << bsoncxx::from_json(actionParameters); }
                                                                //                << close_array << finalize)
                                                                //  : (
                                                                //        builder << "ActionID" << actionId 
                                                                //                << "ActionName" << actionName
                                                                //                << "ActionSequenceId" << MongoDB_Bridge::currentActionSequenceId
                                                                //                << "RequestCreateTime" << bsoncxx::types::b_date(now)
                                                                //                 << "Parameters" << open_array
                                                                //                << close_array << finalize);
                                                                builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "ActionSequenceId" << MongoDB_Bridge::currentActionSequenceId
                                                                               << "RequestCreateTime" << bsoncxx::types::b_date(now)
                                                                               << "Parameters" <<  bsoncxx::from_json(actionParameters)
                                                                               << finalize)
                                                                 : (
                                                                       builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "ActionSequenceId" << MongoDB_Bridge::currentActionSequenceId
                                                                               << "RequestCreateTime" << bsoncxx::types::b_date(now)
                                                                                << "Parameters" << open_document 
                                                                                << close_document << finalize);

  auto retVal =MongoDB_Bridge::actionToExecuteCollection.insert_one(doc_value.view());

  bsoncxx::oid oid = retVal->inserted_id().get_oid().value;
  return oid;
}

void MongoDB_Bridge::SaveBeliefState(std::string belief)
{
MongoDB_Bridge::Init(); 
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = bsoncxx::from_json(belief); 
                                                                 
MongoDB_Bridge::beliefStatesColllection.insert_one(doc_value.view());
}

void MongoDB_Bridge::RegisterAction(int actionId, std::string actionName, std::string actionParameters, std::string actionDescription)
{
  MongoDB_Bridge::Init(); 
  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = !actionParameters.empty() ? (
                                                                       builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "ActionDecription" << actionDescription
                                                                               << "ActionConstantParameters" << open_array
                                                                               << [&](bsoncxx::builder::stream::array_context<> arr)
                                                                       { arr << bsoncxx::from_json(actionParameters); }
                                                                               << close_array << finalize)
                                                                 : (
                                                                       builder << "ActionID" << actionId 
                                                                               << "ActionName" << actionName
                                                                               << "ActionDecription" << actionDescription
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