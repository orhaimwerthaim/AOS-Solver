  
#ifndef MONGO_BRIDGE_H
#define MONGO_BRIDGE_H
 

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
        static int WaitForManualAction();
        static std::map<std::string, std::string> WaitForActionResponse(bsoncxx::oid actionForExecuteId, std::string &actionTextObservation);
        static void UpdateSolverDetails(bool isFirst, int solverId);
        static void GetSolverDetails(bool& shutDown, bool& isFirst, int solverId);
        static bsoncxx::oid SendActionToExecution(int actionId, std::string actionName, std::string actionParameters);
        static void RegisterAction(int actionId, std::string actionName, std::string actionParameters, std::string actionDescription);
        static void SaveBeliefState(std::string currentAction, std::string currentBelief);
        static void SaveSimulatedState(std::string currentSimulatedState);
        static void SaveClosedModelBeliefState(std::string currentBeliefStr);
        static std::string SampleFromBeliefState(int skipStates, int takeStates);
        static void SaveInternalActionResponse(std::string actionName, bsoncxx::oid actionForExecuteId, std::string observationText);
        static void AddLog(std::string logMsg, int logLevel); 
        //     static void UpdateActionResponse(std::string actionName, std::string actionResponse);

        static bool isInit;
        static mongocxx::instance instance; // This should be done only once.
        static mongocxx::uri uri;
        static mongocxx::client client;

        static mongocxx::database db;
        static mongocxx::collection SolversCollection; 
        static  mongocxx::collection manualActionsForSolverCollection;
        static mongocxx::collection logsCollection;
        static mongocxx::collection actionToExecuteCollection;
        static mongocxx::collection actionsCollection;
        static mongocxx::collection moduleResponseColllection;
        static mongocxx::collection localVariableColllection;
        static mongocxx::collection globalVariablesAssignmentsColllection;
        static mongocxx::collection beliefStatesColllection;
        static mongocxx::collection SimulatedStatesColllection;
        static mongocxx::collection closedModelBeliefStatesColllection;
        static int currentActionSequenceId;
        static std::chrono::milliseconds firstSolverIsAliveDateTime;
    };
}
#endif