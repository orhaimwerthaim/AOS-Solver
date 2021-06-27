
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
      MongoDB_Bridge::actionToExecuteCollection = MongoDB_Bridge::db["actionsToExecute"];
      MongoDB_Bridge::moduleResponseColllection = MongoDB_Bridge::db["moduleResponses"];
    }
}

bsoncxx::document::view MongoDB_Bridge::WaitForModuleResponse(std::string moduleName)
{
  MongoDB_Bridge::Init();
  auto filter = document{} << "wasRead" << false << finalize;
   

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

void MongoDB_Bridge::SendActionToExecution(std::string actionName, std::vector<std::string> parameters, std::vector<std::string> parameterNames)
{
  MongoDB_Bridge::Init();
  // auto builder = bsoncxx::builder::stream::document{};
  // bsoncxx::document::value doc_value = builder << "InsertTime" //<<
  //                                                              // bsoncxx::types::b_date(std::chrono::system_clock::now())
  //                                              << "actionName" << actionName
  //                                              << "parameters" << open_array
  //                                              << open_document << "name"
  //                                              << ""
  //                                              << "value"
  //                                              << "" << close_document << close_array << finalize;

  auto builder = bsoncxx::builder::stream::document{};
  bsoncxx::document::value doc_value = builder
                                       << "name"
                                       << "MongoDB"
                                       << "type"
                                       << "database"
                                       << "count" << 1
                                       << "versions" << bsoncxx::builder::stream::open_array
                                       << "v3.2"
                                       << "v3.0"
                                       << "v2.6"
                                       << close_array
                                       << "info" << bsoncxx::builder::stream::open_document
                                       << "x" << 203
                                       << "y" << 102
                                       << bsoncxx::builder::stream::close_document
                                       << bsoncxx::builder::stream::finalize;

  MongoDB_Bridge::actionToExecuteCollection.insert_one(doc_value.view());
}
}
// int main(int argc, char* argv[]) {
// mongocxx::instance instance{}; // This should be done only once.
// mongocxx::uri uri("mongodb://localhost:27017");
// mongocxx::client client(uri);

// mongocxx::database db = client["AOS"];


// mongocxx::collection coll = db["localVariables"];

// auto builder = bsoncxx::builder::stream::document{};
// bsoncxx::document::value doc_value = builder
//   << "name" << "MongoDB"
//   << "type" << "database"
//   << "count" << 1
//   << "versions" << bsoncxx::builder::stream::open_array
//     << "v3.2" << "v3.0" << "v2.6"
//   << close_array
//   << "info" << bsoncxx::builder::stream::open_document
//     << "x" << 203
//     << "y" << 102
//   << bsoncxx::builder::stream::close_document
//   << bsoncxx::builder::stream::finalize;

// // {
// //    "name" : "MongoDB",
// //    "type" : "database",
// //    "count" : 1,
// //    "versions": [ "v3.2", "v3.0", "v2.6" ],
// //    "info" : {
// //                "x" : 203,
// //                "y" : 102
// //             }
// // }

// bsoncxx::document::view view = doc_value.view();
// //coll.insert_one(doc_value);
// coll.insert_one(view);

// mongocxx::change_stream st= coll.watch();
// st.
// st.

// bsoncxx::document::element element = view["name"];
// if(element.type() != bsoncxx::type::k_string) {
//   // Error
// }
// std::string name = element.get_string().value.to_string();
