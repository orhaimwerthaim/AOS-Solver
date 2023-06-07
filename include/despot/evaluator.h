#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <despot/core/globals.h>
#include <despot/core/pomdp.h>
#include <despot/pomdpx/pomdpx.h>
#include <despot/util/util.h>
#include <despot/model_primitives/collectValuableToys/enum_map_collectValuableToys.h>
#include <unistd.h>
#include <algorithm>
#include <cctype> 
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;
namespace despot {

/* =============================================================================
 * EvalLog class
 * =============================================================================*/

class EvalLog {
private:
  std::vector<std::string> runned_instances;
	std::vector<int> num_of_completed_runs;
	std::string log_file_; 
public:
	static time_t start_time;

	static double curr_inst_start_time;
	static double curr_inst_target_time; // Targetted amount of time used for each step
	static double curr_inst_budget; // Total time in seconds given for current instance
	static double curr_inst_remaining_budget; // Remaining time in seconds for current instance
	static int curr_inst_steps;
	static int curr_inst_remaining_steps;
	static double allocated_time;
	static double plan_time_ratio;

	EvalLog(std::string log_file);

	void Save();
	void IncNumOfCompletedRuns(std::string problem);
	int GetNumCompletedRuns() const;
	int GetNumRemainingRuns() const;
	int GetNumCompletedRuns(std::string instance) const;
	int GetNumRemainingRuns(std::string instance) const;
	double GetUsedTimeInSeconds() const;
	double GetRemainingTimeInSeconds() const;

	// Pre-condition: curr_inst_start_time is initialized
	void SetInitialBudget(std::string instance);
	double GetRemainingBudget(std::string instance) const;
};

struct alpha_vector
{
	int action;
	vector<double> values;
};

struct policy{
    vector<alpha_vector> vectors;
    std::string policyFile;
    std::string currrentState;
	std::map<std::string, std::string> obsStrToNum;
	bool wasInit = false;

    std::map<int, int> state_hash_to_vec_ind;

	int get_best_action_by_alpha_vector(map<int, int> belief_hash_count)
	{
		int max_action = -1;
		int max_value = -99999999;
		for(alpha_vector vec : vectors)
		{
			double value=0;
			int count = 0;
			for (auto it = belief_hash_count.cbegin(); it != belief_hash_count.cend() /* not hoisted */; /* no increment */)
			{  
				if (state_hash_to_vec_ind.find(it->first) == state_hash_to_vec_ind.end())
				{
					belief_hash_count.erase(it++);    // or "it = m.erase(it)" since C++11
				}
				else
				{
					int ind = state_hash_to_vec_ind[it->first];
					value += vec.values[ind]*it->second;
					count += it->second;
					++it;
				}
			}
			value = count > 0 ? value/(double)count : -99999999;
			if(value > max_value)
			{
				max_value = value;
				max_action = vec.action;
			}
		}
		return max_action;
	}
	
	std::string find_policy_file()
	{
		string best_policy_fname("out.policy");
		char tmp[256];
		getcwd(tmp, 256);
		std::string workingDirPath(tmp);
		workingDirPath = workingDirPath.substr(0, workingDirPath.find("build"));
		std::string pathToPoliciesDir(workingDirPath);
		pathToPoliciesDir.append("sarsop/src");
		vector<string> files;
		for (const auto & entry : fs::directory_iterator(pathToPoliciesDir))
		{
			string ends_with(".policy");
			string file_p(entry.path().u8string());
			if(file_p.size() >= ends_with.size() &&
					file_p.compare(file_p.size() - ends_with.size(), ends_with.size(), ends_with) == 0)
					{
						string fname(file_p.substr(pathToPoliciesDir.size()+1));
						if(fname == best_policy_fname) return file_p;
						files.push_back(fname);
					}
		}
		int max_time = -1;
		string best_nfile;
		for(string s:files)
		{
			if(s.size()>4)
			{
				string t(s.substr(4));
				if(t.find("_") >=0) 
				{
					t = t.substr(t.find("_")+1);
					if(t.find(".") >=0) 
					{
						if(t.find(".") >= 0)
						{
							t = t.substr(0,t.find("."));
							int time = stoi(t);
							if(max_time < time)
							{
								max_time=time;
								best_nfile = s;
							}
						}
					}
				}
			}
		}
		if(max_time > 0)
		{
			pathToPoliciesDir.append("/");
			string new_fname(pathToPoliciesDir);
			new_fname.append(best_policy_fname);
			string bf_path(pathToPoliciesDir);
			bf_path.append(best_nfile);
			//std::filesystem::copy(bf_path, new_fname);
			return bf_path;
		}
		throw string("exception ! No sarsop policy found!");
	} 
 

	void load_policy_line(std::string line)
	{
		
		if (line.rfind("<Vector action", 0) != 0) return;
		else
		{
			alpha_vector vec; 
			std::string str1(line);
			std::string temp("<Vector action=\"");
			str1 = str1.substr(temp.size());
			str1 = str1.substr(0,str1.find("\""));
			vec.action= stoi(str1);
			std::string str(line);
			str = str.substr(str.find(">")+1);
			str = str.substr(0,str.find("<"));

			std::stringstream sstr(str);
			std::vector<std::string> s_hashes;
			int ind = 0;
			while(sstr.good())
			{
				std::string substr;
				getline(sstr, substr, ' ');
				std::string s_val = substr.substr(0, substr.find(" "));
				remove(s_val.begin(), s_val.end(), ' '); 
				if(s_val.size() > 0)
				{
				double value = stod(s_val);
				vec.values.push_back(value);
				}
			}
			vectors.push_back(vec);
		}
	}

	void init_policy_vec()
	{ 
		if(wasInit)
			return;
		wasInit = true;

		char tmp[256];
		getcwd(tmp, 256);
		std::string workingDirPath(tmp);
		workingDirPath = workingDirPath.substr(0, workingDirPath.find("build"));
		std::string stateMapFile(workingDirPath);
		stateMapFile.append("sarsop/src/state_to_hash.txt");
		std::ifstream ifs1(stateMapFile);
		std::string state_map( (std::istreambuf_iterator<char>(ifs1) ),
                       (std::istreambuf_iterator<char>()    ) );
		std::stringstream sstr(state_map);
		std::vector<std::string> s_hashes;
		int ind = 0;
		while(sstr.good())
		{
			std::string substr;
			getline(sstr, substr, ',');
			int hash =  stoi(substr.substr(0, substr.find(":")));
			state_hash_to_vec_ind.insert({hash, ind++});
		}


		std::string policyFilePath(find_policy_file());
		std::ifstream ifs(policyFilePath);
		std::string line;

// std::string state_map2( (std::istreambuf_iterator<char>(ifs) ),
//                        (std::istreambuf_iterator<char>()    ) );

		while (std::getline(ifs, line))
		{
			load_policy_line(line);
		} 
	}

	void init_policy()
	{
		if(wasInit)
			return;
		wasInit = true;

		char tmp[256];
		getcwd(tmp, 256);
		std::string workingDirPath(tmp);
		workingDirPath = workingDirPath.substr(0, workingDirPath.find("build"));
		std::string policyFilePath(workingDirPath);
		policyFilePath.append(Globals::config.fixedGraphPolicyDotFilePath);
		std::ifstream ifs(policyFilePath);
		std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
        policyFile = content;
        
		currrentState = "root"; 
        std::string pomdpFilePath(workingDirPath);
		pomdpFilePath.append(Globals::config.pomdpFilePath);
		std::ifstream pf(pomdpFilePath);
		std::string pomContent( (std::istreambuf_iterator<char>(pf) ),
                       (std::istreambuf_iterator<char>()    ) );

		std::string t = "observations:";
		int obsInd = pomContent.find(t) + t.size();
		while(obsInd > t.size())
		{
            //remove spaces
			while(pomContent[obsInd] == ' ')
				obsInd++;

            //when we read all the observations, stop!
			if(pomContent[obsInd] == '\n')
				break;
            
            //to remove the observation prefix 'o<obs_num>_'
            // while(pomContent[obsInd] != '_')
			// 	obsInd++;
            // obsInd++;

			int endObs = pomContent.find(" ", obsInd);
			obsStrToNum.insert({pomContent.substr(obsInd, endObs - obsInd), std::to_string(obsStrToNum.size())});
			obsInd = endObs;
		}
	}
};


/* =============================================================================
 * Evaluator class
 * =============================================================================*/

/** Interface for evaluating a solver's performance by simulating how it runs
 * in a real.
 */
class Evaluator {
	private:
	std::vector<int> action_sequence_to_sim;
    void SaveBeliefToDB();
protected:
    policy fixedPolicy;
	DSPOMDP* model_;
	std::string belief_type_;
	Solver* solver_;
	clock_t start_clockt_;
	State* state_;
	int step_;
	double target_finish_time_;
	std::ostream* out_;

	std::vector<double> discounted_round_rewards_;
	std::vector<double> undiscounted_round_rewards_;
	double reward_;
	double total_discounted_reward_;
	double total_undiscounted_reward_;

public:
	Evaluator(DSPOMDP* model, std::string belief_type, Solver* solver,
		clock_t start_clockt, std::ostream* out);
	virtual ~Evaluator();

	inline void out(std::ostream* o) {
		out_ = o;
	}

	inline void rewards(std::vector<double> rewards) {
		undiscounted_round_rewards_ = rewards;
	}

	inline std::vector<double> rewards() {
		return undiscounted_round_rewards_;
	}

	inline int step() {
		return step_;
	}
	inline double target_finish_time() {
		return target_finish_time_;
	}
	inline void target_finish_time(double t) {
		target_finish_time_ = t;
	}
	inline Solver* solver() {
		return solver_;
	}
	inline void solver(Solver* s) {
		solver_ = s;
	}
	inline DSPOMDP* model() {
		return model_;
	}
	inline void model(DSPOMDP* m) {
		model_ = m;
	}

	virtual inline void world_seed(unsigned seed) {
	}

	virtual int Handshake(std::string instance) = 0; // Initialize simulator and return number of runs.
	virtual void InitRound() = 0;

	bool RunStep(int step, int round);

	virtual double EndRound() = 0; // Return total undiscounted reward for this round. 
    virtual bool ExecuteAction(int action, double& reward, OBS_TYPE& obs, std::map<std::string, std::string>& localVariablesFromAction, std::string& obsStr) = 0;
	//IcapsResponseModuleAndTempEnums CalculateModuleResponse(std::string moduleName);
	virtual void ReportStepReward();
	virtual double End() = 0; // Free resources and return total reward collected

	virtual void UpdateTimePerMove(double step_time) = 0;

	double AverageUndiscountedRoundReward() const;
	double StderrUndiscountedRoundReward() const;
	double AverageDiscountedRoundReward() const;
	double StderrDiscountedRoundReward() const;
};

/* =============================================================================
 * POMDPEvaluator class
 * =============================================================================*/

/** Evaluation by simulating using a DSPOMDP model.*/
class POMDPEvaluator: public Evaluator {
protected:
	Random random_;
    policy fixedPolicy;
public:
	POMDPEvaluator(DSPOMDP* model, std::string belief_type, Solver* solver,
		clock_t start_clockt, std::ostream* out, double target_finish_time = -1,
		int num_steps = -1);
	~POMDPEvaluator();

	virtual inline void world_seed(unsigned seed) {
		random_ = Random(seed);
	}

	int Handshake(std::string instance);
	void InitRound();
	double EndRound();
	bool ExecuteAction(int action, double& reward, OBS_TYPE& obs, std::map<std::string, std::string>& localVariablesFromAction, std::string& obsStr);
	double End();
	void UpdateTimePerMove(double step_time);
};

} // namespace despot

#endif
