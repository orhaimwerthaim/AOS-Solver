#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace despot {

enum eLogLevel
{
	Off=0,FATAL=1,ERROR=2,WARN=3,INFO=4,DEBUG=5,TRACE=6
};

struct Config {
    int solverId;
	bool internalSimulation; 
	int search_depth;
	double discount;
	unsigned int root_seed;
	double time_per_move;  // CPU time available to construct the search tree
	int num_scenarios;
	double pruning_constant;
	double xi; // xi * gap(root) is the target uncertainty at the root.
	int sim_len; // Number of steps to run the simulation for.
  std::string default_action;
	int max_policy_sim_len; // Maximum number of steps for simulating the default policy
	double noise;
	bool manualControl;\
    int verbosity;
    bool saveBeliefToDB;
    bool handsOnDebug;
	bool solveProblemWithClosedPomdpModel;
	std::string fixedGraphPolicyDotFilePath;
    std::string fixedPolicyFilePath;
	std::string pomdpFilePath;
	int numOfSamplesPerActionStateWhenLearningTheModel;
	//double sarsopTimeLimitInSeconds;// if sarsopTimeLimitInSeconds <= 0 there is no time limit.
	int limitClosedModelHorizon_stepsAfterGoalDetection;
    bool useSavedSarsopPolicy;
    std::string domainHash;
	Config() : 
        handsOnDebug(true),
        manualControl(false),
        useSavedSarsopPolicy(false),
        limitClosedModelHorizon_stepsAfterGoalDetection(-1),
        //sarsopTimeLimitInSeconds(60),//not working well in sarsop
        numOfSamplesPerActionStateWhenLearningTheModel(1),
        fixedGraphPolicyDotFilePath("sarsop/src/autoGen.dot"), //the path ../sarsop/src/autoGen.dot because working dir is /build/ so we need go one directory backwards.
        fixedPolicyFilePath("sarsop/src/out.policy"),
		pomdpFilePath("sarsop/examples/POMDP/auto_generate.pomdp"),
        solveProblemWithClosedPomdpModel(true),
        solverId(54),
		search_depth(9),
		discount(0.99),
		domainHash("E837E05CB3A95D512814D6C913EBB1116CF571A8583857AC2D8CCFEFD53DC4CA"),
        root_seed(42),
		time_per_move(2),
		num_scenarios(500),
		pruning_constant(0),
		xi(0.95),
		sim_len(1000),
		default_action(""),
		max_policy_sim_len(10),
		noise(0.1),
        //Off=0,FATAL=1,ERROR=2,WARN=3,INFO=4,DEBUG=5,TRACE=6 
		verbosity(4),
		internalSimulation(true),
        saveBeliefToDB(true)
		{
		
	}
};

} // namespace despot

#endif
