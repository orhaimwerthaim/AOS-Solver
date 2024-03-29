#include <despot/evaluator.h>
#include <despot/util/mongoDB_Bridge.h>
#include <despot/model_primitives/example/enum_map_example.h>
#include <despot/model_primitives/example/actionManager.h> 
#include <despot/model_primitives/example/closed_model_policy.h>
#include <nlohmann/json.hpp>
using namespace std;


namespace despot {

/* =============================================================================
 * EvalLog class
 * =============================================================================*/

time_t EvalLog::start_time = 0;
double EvalLog::curr_inst_start_time = 0;
double EvalLog::curr_inst_target_time = 0;
double EvalLog::curr_inst_budget = 0;
double EvalLog::curr_inst_remaining_budget = 0;
int EvalLog::curr_inst_steps = 0;
int EvalLog::curr_inst_remaining_steps = 0;
double EvalLog::allocated_time = 1.0;
double EvalLog::plan_time_ratio = 1.0;

EvalLog::EvalLog(string log_file) :
	log_file_(log_file) {
	ifstream fin(log_file_.c_str(), ifstream::in);
	if (!fin.good() || fin.peek() == ifstream::traits_type::eof()) {
		time(&start_time);
	} else {
		fin >> start_time;

		int num_instances;
		fin >> num_instances;
		for (int i = 0; i < num_instances; i++) {
			string name;
			int num_runs;
			fin >> name >> num_runs;
			runned_instances.push_back(name);
			num_of_completed_runs.push_back(num_runs);
		}
	}
	fin.close();
}

void EvalLog::Save() {
	ofstream fout(log_file_.c_str(), ofstream::out);
	fout << start_time << endl;
	fout << runned_instances.size() << endl;
	for (int i = 0; i < runned_instances.size(); i++)
		fout << runned_instances[i] << " " << num_of_completed_runs[i] << endl;
	fout.close();
}

void EvalLog::IncNumOfCompletedRuns(string problem) {
	bool seen = false;
	for (int i = 0; i < runned_instances.size(); i++) {
		if (runned_instances[i] == problem) {
			num_of_completed_runs[i]++;
			seen = true;
		}
	}

	if (!seen) {
		runned_instances.push_back(problem);
		num_of_completed_runs.push_back(1);
	}
}

int EvalLog::GetNumCompletedRuns() const {
	int num = 0;
	for (int i = 0; i < num_of_completed_runs.size(); i++)
		num += num_of_completed_runs[i];
	return num;
}

int EvalLog::GetNumRemainingRuns() const {
	return 80 * 30 - GetNumCompletedRuns();
}

int EvalLog::GetNumCompletedRuns(string instance) const {
	for (int i = 0; i < runned_instances.size(); i++) {
		if (runned_instances[i] == instance)
			return num_of_completed_runs[i];
	}
	return 0;
}

int EvalLog::GetNumRemainingRuns(string instance) const {
	return 30 - GetNumCompletedRuns(instance);
}

double EvalLog::GetUsedTimeInSeconds() const {
	time_t curr;
	time(&curr);
	return (double) (curr - start_time);
}

double EvalLog::GetRemainingTimeInSeconds() const {
	return 24 * 3600 - GetUsedTimeInSeconds();
}

// Pre-condition: curr_inst_start_time is initialized
void EvalLog::SetInitialBudget(string instance) {
	curr_inst_budget = 0;
	if (GetNumRemainingRuns() != 0 && GetNumRemainingRuns(instance) != 0) {
		cout << "Num of remaining runs: curr / total = "
			<< GetNumRemainingRuns(instance) << " / " << GetNumRemainingRuns()
			<< endl;
		curr_inst_budget = (24 * 3600 - (curr_inst_start_time - start_time))
			/ GetNumRemainingRuns() * GetNumRemainingRuns(instance);
		if (curr_inst_budget < 0)
			curr_inst_budget = 0;
		if (curr_inst_budget > 18 * 60)
			curr_inst_budget = 18 * 60;
	}
}

double EvalLog::GetRemainingBudget(string instance) const {
	return curr_inst_budget
		- (get_time_second() - EvalLog::curr_inst_start_time);
}

/* =============================================================================
 * Evaluator class
 * =============================================================================*/

Evaluator::Evaluator(DSPOMDP* model, string belief_type, Solver* solver,
	clock_t start_clockt, ostream* out) :
	model_(model),
	belief_type_(belief_type),
	solver_(solver),
	start_clockt_(start_clockt),
	target_finish_time_(-1),
	out_(out) {
        
}

Evaluator::~Evaluator() {
}


void Evaluator::SaveBeliefToDB()
{
	if(Globals::config.saveBeliefToDB)
	{
		vector<State*> temp = solver_->belief()->Sample(20);
		Prints::SaveBeliefParticles(temp);
	}
	Prints::SaveSimulatedState(state_);
}

bool Evaluator::RunStep(int step, int round) {
	bool byExternalPolicy = Globals::config.solveProblemWithClosedPomdpModel;
    bool shutDown = false;
	bool isFirst = false;
	int solverId = Globals::config.solverId;

	MongoDB_Bridge::GetSolverDetails(shutDown, isFirst, solverId);

	if(byExternalPolicy)
	{
         
	}

	if(shutDown && !Globals::config.handsOnDebug)
	{
		return true;
	}
	else
	{
		MongoDB_Bridge::UpdateSolverDetails(isFirst, solverId);
	}

	if (target_finish_time_ != -1 && get_time_second() > target_finish_time_) {
		if (Globals::config.verbosity >= eLogLevel::TRACE && out_)
			*out_ << "Exit. (Total time "
				<< (get_time_second() - EvalLog::curr_inst_start_time)
				<< "s exceeded time limit of "
				<< (target_finish_time_ - EvalLog::curr_inst_start_time) << "s)"
				<< endl
				<< "Total time: Real / CPU = "
				<< (get_time_second() - EvalLog::curr_inst_start_time) << " / "
				<< (double(clock() - start_clockt_) / CLOCKS_PER_SEC) << "s"
				<< endl;
		exit(1);
	}

	double step_start_t = get_time_second();
    double start_t = get_time_second();
	int action = -1;

    if(byExternalPolicy)
    {   
		vector<State*> particles = solver_->belief()->GetParticles();
		map<int, int> states_hash_count;
		for(State* par : particles)
		{
			int hash = State::GetStateHash(*par);
			states_hash_count[hash] +=1;
		}
         
		
    }
    else if(action_sequence_to_sim.size() == 0)
	{
        action = solver_->Search().action;
	}
	if(action_sequence_to_sim.size() > 0)
	{
		action = action_sequence_to_sim[0];
		action_sequence_to_sim.erase(action_sequence_to_sim.begin());
		if(action_sequence_to_sim.size() == 0)
		{
			action_sequence_to_sim.push_back(-1);
		}
	}
	if(Globals::config.manualControl)
	{
		action = MongoDB_Bridge::WaitForManualAction();
		if (action < 0)
		return false;
	}

	if(Globals::config.verbosity >= eLogLevel::INFO)
	{
		std::string actionDesc = Prints::PrintActionDescription(action);
		std::string logMsg("Solver Selected Action: " + actionDesc);
		MongoDB_Bridge::AddLog(logMsg, eLogLevel::INFO);
	}

	double end_t = get_time_second();
	logi << "[RunStep] Time spent in " << typeid(*solver_).name()
		<< "::Search(): " << (end_t - start_t) << endl;

	double reward;
	OBS_TYPE obs;
	start_t = get_time_second();

	//TODO:: remove prints
    *out_ << "-----------------------------------Round " << round
				<< " Step " << step << "-----------------------------------"
				<< endl;
	logi << "--------------------------------------EXECUTION---------------------------------------------------------------------------" << endl;
	if (Globals::config.verbosity >= eLogLevel::INFO && out_) {
		*out_ << endl << "Action = ";
		model_->PrintAction(action, *out_);
	}
    logi << endl
		 << "Before:" << endl;
    model_->PrintState(*state_);
	std::map<std::string, std::string> localVariablesFromAction;

	if(MongoDB_Bridge::currentActionSequenceId == 0)
	{
		Evaluator::SaveBeliefToDB();
	}
    std::string obsStr;
	bool terminal = ExecuteAction(action, reward, obs, localVariablesFromAction, obsStr);
    
	if(Globals::config.verbosity >= eLogLevel::INFO)
	{
		std::string logMsg("Received observation:"+ Prints::PrintObs(obs));
		MongoDB_Bridge::AddLog(logMsg, eLogLevel::INFO);
	}
    logi << endl
		 << "After:" << endl;
	model_->PrintState(*state_);
	logi << endl << "Reward:" << reward << endl <<  "Observation:" << enum_map_example::vecResponseEnumToString[(ExampleResponseModuleAndTempEnums)obs] << endl;
	end_t = get_time_second();
	logi << "[RunStep] Time spent in ExecuteAction(): " << (end_t - start_t)
		<< endl;
	logi << "-------------------------------------END-EXECUTION---------------------------------------------------------------------------" << endl;
	start_t = get_time_second();
	
	

	ReportStepReward();

 

	end_t = get_time_second();

	*out_<<endl;

	start_t = get_time_second();
	
    solver_->Update(action, obs, localVariablesFromAction);
	Evaluator::SaveBeliefToDB();
	
	end_t = get_time_second();
	logi << "[RunStep] Time spent in Update(): " << (end_t - start_t) << endl;

	step_++;

    BeliefStateVariables bv = BeliefStateVariables(solver_->belief()->Sample(1000));
	if(bv.__isTermianl_mean > 0.9 && bv.__isTermianl_std < 0.25)
	{
		return true;
	}
	return false;
}

double Evaluator::AverageUndiscountedRoundReward() const {
	double sum = 0;
	for (int i = 0; i < undiscounted_round_rewards_.size(); i++) {
		double reward = undiscounted_round_rewards_[i];
		sum += reward;
	}
	return undiscounted_round_rewards_.size() > 0 ? (sum / undiscounted_round_rewards_.size()) : 0.0;
}

double Evaluator::StderrUndiscountedRoundReward() const {
	double sum = 0, sum2 = 0;
	for (int i = 0; i < undiscounted_round_rewards_.size(); i++) {
		double reward = undiscounted_round_rewards_[i];
		sum += reward;
		sum2 += reward * reward;
	}
	int n = undiscounted_round_rewards_.size();
	return n > 0 ? sqrt(sum2 / n / n - sum * sum / n / n / n) : 0.0;
}


double Evaluator::AverageDiscountedRoundReward() const {
	double sum = 0;
	for (int i = 0; i < discounted_round_rewards_.size(); i++) {
		double reward = discounted_round_rewards_[i];
		sum += reward;
	}
	return discounted_round_rewards_.size() > 0 ? (sum / discounted_round_rewards_.size()) : 0.0;
}

double Evaluator::StderrDiscountedRoundReward() const {
	double sum = 0, sum2 = 0;
	for (int i = 0; i < discounted_round_rewards_.size(); i++) {
		double reward = discounted_round_rewards_[i];
		sum += reward;
		sum2 += reward * reward;
	}
	int n = discounted_round_rewards_.size();
	return n > 0 ? sqrt(sum2 / n / n - sum * sum / n / n / n) : 0.0;
}

void Evaluator::ReportStepReward() {
	if (Globals::config.verbosity >= eLogLevel::INFO && out_)
		*out_ << "- Reward = " << reward_ << endl
			<< "- Current rewards:" << endl
			<< "  discounted / undiscounted = " << total_discounted_reward_
			<< " / " << total_undiscounted_reward_ << endl;
}

/* =============================================================================
 * POMDPEvaluator class
 * =============================================================================*/

POMDPEvaluator::POMDPEvaluator(DSPOMDP* model, string belief_type,
	Solver* solver, clock_t start_clockt, ostream* out,
	double target_finish_time, int num_steps) :
	Evaluator(model, belief_type, solver, start_clockt, out),
	random_((unsigned) 0) {
	target_finish_time_ = target_finish_time;

	if (target_finish_time_ != -1) {
		EvalLog::allocated_time = (target_finish_time_ - get_time_second())
			/ num_steps;
		Globals::config.time_per_move = EvalLog::allocated_time;
		EvalLog::curr_inst_remaining_steps = num_steps;
	}
}

POMDPEvaluator::~POMDPEvaluator() {
}

int POMDPEvaluator::Handshake(string instance) {
	return -1; // Not to be used
}

void POMDPEvaluator::InitRound() {
	step_ = 0;

	double start_t, end_t;
	// Initial state
	state_ = model_->CreateStartState();
	logi << "[POMDPEvaluator::InitRound] Created start state." << endl;
	if (Globals::config.verbosity >= eLogLevel::INFO && out_) {
		*out_ << "Initial state: " << endl;
		model_->PrintState(*state_, *out_);
		*out_ << endl;
	}

	// Initial belief
	start_t = get_time_second();
	delete solver_->belief();
	end_t = get_time_second();
	logi << "[POMDPEvaluator::InitRound] Deleted old belief in "
		<< (end_t - start_t) << "s" << endl;

	start_t = get_time_second();
	Belief* belief = model_->InitialBelief(state_, belief_type_);
	end_t = get_time_second();
	logi << "[POMDPEvaluator::InitRound] Created intial belief "
		<< typeid(*belief).name() << " in " << (end_t - start_t) << "s" << endl;

	solver_->belief(belief);

	total_discounted_reward_ = 0;
	total_undiscounted_reward_ = 0;
}

double POMDPEvaluator::EndRound() {
	if (Globals::config.verbosity >= eLogLevel::INFO && out_) {
		std::string logMsg("Total undiscounted simulated reward:" + to_string(total_undiscounted_reward_));
		MongoDB_Bridge::AddLog(logMsg, eLogLevel::INFO);
		*out_ << "Total discounted reward = " << total_discounted_reward_ << endl
			<< "Total undiscounted reward = " << total_undiscounted_reward_ << endl;
	}

	discounted_round_rewards_.push_back(total_discounted_reward_);
	undiscounted_round_rewards_.push_back(total_undiscounted_reward_);

	return total_undiscounted_reward_;
}

bool POMDPEvaluator::ExecuteAction(int action, double& reward, OBS_TYPE& obs, std::map<std::string, std::string>& localVariablesFromAction, std::string& obsStr) {
	MongoDB_Bridge::currentActionSequenceId++;
	ActionDescription &actDesc = *ActionManager::actions[action];
    ActionType acType(actDesc.actionType);
	std::string actionParameters = actDesc.GetActionParametersJson_ForActionExecution();
		
	std::string actionName = enum_map_example::vecActionTypeEnumToString[acType];
	  
	bsoncxx::oid actionId = MongoDB_Bridge::SendActionToExecution(actDesc.actionId, actionName, actionParameters);

	double random_num = random_.NextDouble();
    bool terminal = false;

    terminal = model_->Step(*state_, random_num, action, reward, obs);//it is outside the 'if' below since we save the simulated state anyway
	if(Globals::IsInternalSimulation())
	{
		obsStr = enum_map_example::vecResponseEnumToString[(ExampleResponseModuleAndTempEnums)obs];
		MongoDB_Bridge::SaveInternalActionResponse(actionName, actionId, obsStr);
		reward_ = reward;
		total_discounted_reward_ += Globals::Discount(step_) * reward;
		total_undiscounted_reward_ += reward;
 
		return terminal;
	}
	else
	{
        terminal = false; 
		obsStr = "";
		localVariablesFromAction = MongoDB_Bridge::WaitForActionResponse(actionId, obsStr);

		obs = enum_map_example::vecStringToResponseEnum[obsStr];
	}
    return terminal;
}

double POMDPEvaluator::End() {
	return 0; // Not to be used
}

void POMDPEvaluator::UpdateTimePerMove(double step_time) {
	if (target_finish_time_ != -1) {
		if (step_time < 0.99 * EvalLog::allocated_time) {
			if (EvalLog::plan_time_ratio < 1.0)
				EvalLog::plan_time_ratio += 0.01;
			if (EvalLog::plan_time_ratio > 1.0)
				EvalLog::plan_time_ratio = 1.0;
		} else if (step_time > EvalLog::allocated_time) {
			double delta = (step_time - EvalLog::allocated_time)
				/ (EvalLog::allocated_time + 1E-6);
			if (delta < 0.02)
				delta = 0.02; // Minimum reduction per step
			if (delta > 0.05)
				delta = 0.05; // Maximum reduction per step
			EvalLog::plan_time_ratio -= delta;
			// if (EvalLog::plan_time_ratio < 0)
			// EvalLog::plan_time_ratio = 0;
		}

		EvalLog::curr_inst_remaining_budget = target_finish_time_
			- get_time_second();
		EvalLog::curr_inst_remaining_steps--;

		if (EvalLog::curr_inst_remaining_steps <= 0) {
			EvalLog::allocated_time = 0;
		} else {
			EvalLog::allocated_time =
				(EvalLog::curr_inst_remaining_budget - 2.0)
					/ EvalLog::curr_inst_remaining_steps;

			if (EvalLog::allocated_time > 5.0)
				EvalLog::allocated_time = 5.0;
		}

		Globals::config.time_per_move = EvalLog::plan_time_ratio
			* EvalLog::allocated_time;
	}
}

} // namespace despot
