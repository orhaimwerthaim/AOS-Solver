
#include <despot/solver/pomcp.h>
#include <despot/util/logging.h>
#include <iostream>
#include <fstream>
 
 

using namespace std;

namespace despot {
/* =============================================================================
 * POMCPPrior class
 * =============================================================================*/

POMCPPrior::POMCPPrior(const DSPOMDP* model) :
	model_(model) {
	double x = (40 / Globals::config.search_depth) > 1 ? (40 / Globals::config.search_depth) : 1;
	exploration_constant_ = (model->GetMaxReward() - model->GetMinRewardAction().value) * x;
}
map<int,int> POMCPPrior::preferred_actions_cache_;
POMCPPrior::~POMCPPrior() {
}

const vector<int>& POMCPPrior::preferred_actions() const {
	return preferred_actions_;
}

const vector<int>& POMCPPrior::legal_actions() const {
	return legal_actions_;
}

const vector<double>& POMCPPrior::weighted_preferred_actions() const {
	return weighted_preferred_actions_;
}

int POMCPPrior::GetAction(const State& state, const DSPOMDP* model) {
    int stateHash = Prints::GetHash(Prints::PrintState(state));
    map<int,int>::iterator it = POMCPPrior::preferred_actions_cache_.find(stateHash);

    if ( it != POMCPPrior::preferred_actions_cache_.end() ) {
        return it->second;
        }

	ComputePreference(state,model);
    int action = -1;

	if (weighted_preferred_actions_.size() != 0)
    {
		action = Random::RANDOM.NextCategory(weighted_preferred_actions_);
        POMCPPrior::preferred_actions_cache_.insert({stateHash, action});
        return action;
    }
	if (preferred_actions_.size() != 0)
		return Random::RANDOM.NextElement(preferred_actions_);

	if (legal_actions_.size() != 0)
		return Random::RANDOM.NextElement(legal_actions_);

	return Random::RANDOM.NextInt(model_->NumActions());
}

/* =============================================================================
 * UniformPOMCPPrior class
 * =============================================================================*/

UniformPOMCPPrior::UniformPOMCPPrior(const DSPOMDP* model) :
	POMCPPrior(model) {
}

UniformPOMCPPrior::~UniformPOMCPPrior() {
}

void UniformPOMCPPrior::ComputePreference(const State& state, const DSPOMDP* model) {
}
double UniformPOMCPPrior::HeuristicValue(const State& state, const State& prev_state){return 0.0;}

/* =============================================================================
 * POMCP class
 * =============================================================================*/

POMCP::POMCP(const DSPOMDP* model, POMCPPrior* prior, Belief* belief) :
	Solver(model, belief),
	root_(NULL) {
	reuse_ = false;
	prior_ = prior;
	assert(prior_ != NULL);
}

void POMCP::reuse(bool r) {
	reuse_ = r;
}

ValuedAction POMCP::Search(double timeout) {
	double start_cpu = clock(), start_real = get_time_second();

	if (root_ == NULL) {
		State* state = belief_->Sample(1)[0];
		root_ = CreateVNode(0, state, prior_, model_);
		model_->Free(state);
	}
	 
	static const int actArr[] = {};
	//static const int actArr[] = {0,4,1,5};//when an action sequence is required to simulate
	
	std::vector<int> actionSeq (actArr, actArr + sizeof(actArr) / sizeof(actArr[0]) );
	 
	std::vector<int>*	simulatedActionSequence = actionSeq.size() > 0 ? &actionSeq : NULL;	 
	 

	int hist_size = history_.Size();
	bool done = false;
	int num_sims = 0;

    std::vector<State*> particles_ = (static_cast<const ParticleBelief*>(belief_))->particles_;//new sample
	while (true) {
        //New sample START
        int pos = std::rand()% particles_.size();
        State* particle = model_->Copy(particles_[pos]);
        //logd << "[POMCP::Search] Starting simulation " << num_sims << endl;

		float r = (float) std::rand()/RAND_MAX;	
		bool byMDP = r < Globals::config.treePolicyByMdpRate;;
		Simulate(particle, root_, model_, prior_, simulatedActionSequence, byMDP);
        Globals::config.stream_id= Globals::config.stream_id+1 >= Globals::config.total_streams ? 0 
            : Globals::config.stream_id+1;
        Globals::config.stream_step=0;
        num_sims++;
        model_->Free(particle);
        if ((clock() - start_cpu) / CLOCKS_PER_SEC >= timeout) {
				done = true;
                cout<<"Number of simulated traces:"<<num_sims << ", Simulated steps:" <<
                    Globals::config.steps_counter << ", Rollouts:" << Globals::config.rollout_counter<< 
                    ", heuristic Calcs:" << Globals::config.heuristic_counter<<
                    ", heuristic cache Calcs:" << Globals::config.heuristic_cache_counter << endl;
                Globals::config.steps_counter=0;
                Globals::config.rollout_counter=0;
                Globals::config.heuristic_counter=0;
				break;
			}
	}

	ValuedAction astar = OptimalAction(root_);
	 
    if(0 > 0)
    {
	    //std::string dot = POMCP::GenerateDotGraph(root_,0, model_);
        std::string dot = POMCP::GenerateDebugJson(root_,0, model_);
	}
    // delete root_;
	return astar;
}



ValuedAction POMCP::Search() {
	return Search(Globals::config.time_per_move);
}

void POMCP::belief(Belief* b) {
	belief_ = b;
	history_.Truncate(0);
  prior_->PopAll();
	delete root_;
	root_ = NULL;
}


void POMCP::Update(int action, OBS_TYPE obs, std::map<std::string, std::string> localVariablesFromAction)
{
	double start = get_time_second();

	if (reuse_) {
		VNode* node = root_->Child(action)->Child(obs);
		root_->Child(action)->children().erase(obs);
		delete root_;

		root_ = node;
		if (root_ != NULL) {
			root_->parent(NULL);
		}
	} else {
		delete root_;
		root_ = NULL;
	}

	prior_->Add(action, obs);
	history_.Add(action, obs);
	belief_->Update(action, obs, localVariablesFromAction);

	//logi << "[POMCP::Update] Updated belief, history and root with action "
	//	<< action << ", observation " << obs
	//	<< " in " << (get_time_second() - start) << "s" << endl;
}

int POMCP::UpperBoundAction(const VNode* vnode, double explore_constant)
{
	return UpperBoundAction(vnode, explore_constant, NULL, NULL);
}

void POMCP::Update(int action, OBS_TYPE obs) 
{
	std::map<std::string, std::string> updatesFromAction;
	POMCP::Update(action, obs, updatesFromAction);
}

int POMCP::UpperBoundAction(const VNode* vnode, double explore_constant, const DSPOMDP* model, Belief* belief) {
	const vector<QNode*>& qnodes = vnode->children();
	double best_ub = Globals::NEG_INFTY;
	int best_action = -1;

	/*
	 int total = 0;
	 for (int action = 0; action < qnodes.size(); action ++) {
	 total += qnodes[action]->count();
	 double ub = qnodes[action]->value() + explore_constant * sqrt(log(vnode->count() + 1) / qnodes[action]->count());
	 cout << action << " " << ub << " " << qnodes[action]->value() << " " << qnodes[action]->count() << " " << vnode->count() << endl;
	 }
	 */
	//TODO:: activate line below only on debug mode:
	//if(model)
	//	logi << model->PrintStateStr(*belief->Sample(1)[0]);

	for (int action = 0; action < qnodes.size(); action++) {
		if (qnodes[action]->count() == 0)
			return action;
		
		//TODO::remove 
		//std::string s= model->GetActionDescription(action);
		
		
		double ub = qnodes[action]->value()
			+ explore_constant
				* sqrt(log(vnode->count() + 1) / qnodes[action]->count());

		if (ub > best_ub) {
			best_ub = ub;
			best_action = action;
		}
		//logi << "[POMCP::UpperBoundAction]:Depth:" << vnode->depth() << "Action:"<< action <<",N:" << vnode->count() << ",V:" << vnode->value() << endl;
		//if (vnode->depth() < 1 && model)
		//	logi << "[POMCP::UpperBoundAction]:Depth:"<< vnode->depth() <<",N:" << vnode->count() <<",V:" << vnode->value() << model->GetActionDescription(action) << ",UCB:"<< ub<< endl;   

			// if(model)
			// logd << "[POMCP::UpperBoundAction]: Best Action is: "<< model->GetActionDescription(best_action) << "|With value:"<<best_ub <<endl;	
	}
	
	assert(best_action != -1);
	//if(model)
	//	logi << "[POMCP::UpperBoundAction]:Selected Action:"<< model->GetActionDescription(best_action) <<endl;
	return best_action;
}

ValuedAction POMCP::OptimalAction(const VNode* vnode) {
	const vector<QNode*>& qnodes = vnode->children();
	ValuedAction astar(-1, Globals::NEG_INFTY);
	for (int action = 0; action < qnodes.size(); action++) {
		// cout << action << " " << qnodes[action]->value() << " " << qnodes[action]->count() << " " << vnode->count() << endl;
		if (qnodes[action]->value() > astar.value) {
			astar = ValuedAction(action, qnodes[action]->value());
		}
	}
	// assert(atar.action != -1);
	return astar;
}

int POMCP::Count(const VNode* vnode) {
	int count = 0;
	for (int action = 0; action < vnode->children().size(); action++)
		count += vnode->Child(action)->count();
	return count;
}

VNode* POMCP::CreateVNode(int depth, const State* state, POMCPPrior* prior,
	const DSPOMDP* model) {
	VNode* vnode = new VNode(0, 0.0, depth);
/*
	prior->ComputePreference(*state);

	const vector<int>& preferred_actions = prior->preferred_actions();
	const vector<int>& legal_actions = prior->legal_actions();

	int large_count = 1000000;
	double neg_infty = -1e10;

	if (legal_actions.size() == 0) { // no prior knowledge, all actions are equal
*/		for (int action = 0; action < model->NumActions(); action++) {
			QNode* qnode = new QNode(vnode, action);
			qnode->count(0);
			qnode->value(0);

			vnode->children().push_back(qnode);
		}
/*	} else {
		for (int action = 0; action < model->NumActions(); action++) {
			QNode* qnode = new QNode(vnode, action);
			qnode->count(large_count);
			qnode->value(neg_infty);

			vnode->children().push_back(qnode);
		}

		for (int a = 0; a < legal_actions.size(); a++) {
			QNode* qnode = vnode->Child(legal_actions[a]);
			qnode->count(0);
			qnode->value(0);
		}

		for (int a = 0; a < preferred_actions.size(); a++) {
			int action = preferred_actions[a];
			QNode* qnode = vnode->Child(action);
			qnode->count(prior->SmartCount(action));
			qnode->value(prior->SmartValue(action));
		}
	}
*/
	return vnode;
}

double POMCP::Simulate(State* particle, RandomStreams& streams, VNode* vnode,
	const DSPOMDP* model, POMCPPrior* prior) {
	return 0;
}

// static
double POMCP::Simulate(State* particle, VNode* vnode, const DSPOMDP* model,
	POMCPPrior* prior, std::vector<int>* simulateActionSequence, bool byMDP) {
	assert(vnode != NULL);
	if (vnode->depth() >= Globals::config.search_depth)
		return 0;

	double explore_constant = prior->exploration_constant();

	int action = simulateActionSequence && simulateActionSequence->size() > vnode->depth() ? (*simulateActionSequence)[vnode->depth()] : UpperBoundAction(vnode, explore_constant);
	
	if(byMDP)
	{
		action = prior->GetAction(*particle, model);		
    } 
	double reward;
	OBS_TYPE obs;
	bool terminal = model->Step(*particle, action, reward, obs);

	QNode* qnode = vnode->Child(action);
	if (!terminal) {
        if(qnode->count() > Globals::config.rollouts_count)
		{
            prior->Add(action, obs);
            map<OBS_TYPE, VNode*>& vnodes = qnode->children();
            if (vnodes[obs] != NULL) {
                reward += Globals::Discount()
                    * Simulate(particle, vnodes[obs], model, prior,simulateActionSequence, byMDP);
            } else { // Rollout upon encountering a node not in curren tree, then add the node
                vnodes[obs] = CreateVNode(vnode->depth() + 1, particle, prior,
                    model);
                reward += Globals::Discount()
* Rollout(particle, vnode->depth() + 1, model, prior,simulateActionSequence);
 
            }
            prior->PopLast();
		}
		else
		{
			reward += Globals::Discount()
* Rollout(particle, vnode->depth() + 1, model, prior,simulateActionSequence);					
		}
	}

	qnode->Add(reward);
	vnode->Add(reward);

	return reward;
}

// static
double POMCP::Rollout(State* particle, RandomStreams& streams, int depth,
	const DSPOMDP* model, POMCPPrior* prior) {
	return 0;
}

// static
double POMCP::Rollout(State* particle, int depth, const DSPOMDP* model,
	POMCPPrior* prior, std::vector<int>* simulateActionSequence) {
    Globals::config.rollout_counter++;
	if (depth >= Globals::config.search_depth) {

return 0; 
	}

	//int action = prior->GetAction(*particle, model);
	
	
    
        int action = simulateActionSequence && simulateActionSequence->size() > depth ? (*simulateActionSequence)[depth] : prior->GetAction(*particle, model);
        
	double reward;
	OBS_TYPE obs;
	bool terminal = model->Step(*particle, action, reward, obs);
	if (!terminal) {
		prior->Add(action, obs);
		reward += Globals::Discount() * Rollout(particle, depth + 1, model, prior,simulateActionSequence);
		prior->PopLast();
	}

	return reward;
}

ValuedAction POMCP::Evaluate(VNode* root, vector<State*>& particles,
	RandomStreams& streams, const DSPOMDP* model, POMCPPrior* prior) {
	double value = 0;

	for (int i = 0; i < particles.size(); i++)
		particles[i]->scenario_id = i;

	for (int i = 0; i < particles.size(); i++) {
		State* particle = particles[i];
		VNode* cur = root;
		State* copy = model->Copy(particle);
		double discount = 1.0;
		double val = 0;
		int steps = 0;

		// Simulate until all random numbers are consumed
		while (!streams.Exhausted()) {
			int action =
				(cur != NULL) ?
					UpperBoundAction(cur, 0) : prior->GetAction(*particle, model);

			double reward;
			OBS_TYPE obs;
			bool terminal = model->Step(*copy, streams.Entry(copy->scenario_id),
				action, reward, obs);

			val += discount * reward;
			discount *= Globals::Discount();

			if (!terminal) {
				prior->Add(action, obs);
				streams.Advance();
				steps++;

				if (cur != NULL) {
					QNode* qnode = cur->Child(action);
					map<OBS_TYPE, VNode*>& vnodes = qnode->children();
					cur = vnodes.find(obs) != vnodes.end() ? vnodes[obs] : NULL;
				}
			} else {
				break;
			}
		}

		// Reset random streams and prior
		for (int i = 0; i < steps; i++) {
			streams.Back();
			prior->PopLast();
		}

		model->Free(copy);

		value += val;
	}

	return ValuedAction(UpperBoundAction(root, 0), value / particles.size());
}

/* =============================================================================
 * DPOMCP class
 * =============================================================================*/

DPOMCP::DPOMCP(const DSPOMDP* model, POMCPPrior* prior, Belief* belief) :
	POMCP(model, prior, belief) {
	reuse_ = false;
}

void DPOMCP::belief(Belief* b) {
	belief_ = b;
	history_.Truncate(0);
  prior_->PopAll();
}

ValuedAction DPOMCP::Search(double timeout) {
	ValuedAction astar;
	return astar;
}

// static
VNode* DPOMCP::ConstructTree(vector<State*>& particles, RandomStreams& streams,
	const DSPOMDP* model, POMCPPrior* prior, History& history, double timeout) {
	prior->history(history);
	VNode* root = CreateVNode(0, particles[0], prior, model);

	for (int i = 0; i < particles.size(); i++)
		particles[i]->scenario_id = i;

	//logi << "[DPOMCP::ConstructTree] # active particles before search = "
	//	<< model->NumActiveParticles() << endl;
	double start = clock();
	int num_sims = 0;
	while (true) {
	//	logd << "Simulation " << num_sims << endl;

		int index = Random::RANDOM.NextInt(particles.size());
		State* particle = model->Copy(particles[index]);
		Simulate(particle, streams, root, model, prior);
		num_sims++;
		model->Free(particle);

		if ((clock() - start) / CLOCKS_PER_SEC >= timeout) {
			break;
		}
	}

	//logi << "[DPOMCP::ConstructTree] OptimalAction = " << OptimalAction(root)
	//	<< endl << "# Simulations = " << root->count() << endl
	//	<< "# active particles after search = " << model->NumActiveParticles()
	//	<< endl;

	return root;
}

void DPOMCP::Update(int action, OBS_TYPE obs, std::map<std::string, std::string> localVariablesFromAction) {
	double start = get_time_second();

	history_.Add(action, obs);
	belief_->Update(action, obs, localVariablesFromAction);;

	//logi << "[DPOMCP::Update] Updated belief, history and root with action "
	//	<< action << ", observation " << obs
	//	<< " in " << (get_time_second() - start) << "s" << endl;
}
/*
std::string POMCP::GenerateDotGraph(VNode* root, int depthLimit, const DSPOMDP* model)
{
	stringstream ssNodes;
	stringstream ssEdges; 
	
	ssNodes << "digraph plan {" << endl;
	ssEdges << "";
	int currentNodeID = 0;
	POMCP::GenerateDotGraphVnode(root, currentNodeID, ssNodes, ssEdges, depthLimit, model_);

	ssNodes << ssEdges.str() << "}" << endl;

	ofstream MyFile("/home/or/AOS/debug.dot");
	MyFile << ssNodes.str();
	MyFile.close();
	//run: "dot -Tpdf  debug.dot > debug.pdf"
	system("(cd /home/or/AOS;dot -Tpdf  debug.dot > debug.pdf)");
	return ssNodes.str();
}

void POMCP::GenerateDotGraphVnode(VNode* vnode, int& currentNodeID, stringstream &ssNodes, stringstream &ssEdges, int depthLimit, const DSPOMDP* model)
{ 
	
	int _nodeID = currentNodeID;

	std::string stateDesc = "";
	try 
	{
		if(vnode->belief() != NULL)
			std::vector<State *> vs = vnode->belief()->Sample(1);
		//stateDesc = model_->PrintStateStr(*());
	} 
	catch (std::exception& e)
	{ 
		stateDesc = "";
		
		 }
	// if(vnode->particles().size() == 0)
	// 	stateDesc = "";
	// else 
	// 	stateDesc = model_->PrintStateStr(*(vnode->particles()[0]));
	// ssNodes << _nodeID << "[label=\"belief[0]:" << stateDesc << "\",style=filled,fillcolor=black,fontcolor=white];" << endl;

	if(depthLimit >= 0 && vnode->depth() >= depthLimit)
	{
		return;
	}

	for (int i = 0; i < vnode->children().size(); i++)
	{
		QNode *child = vnode->children()[i];
		int childId = ++currentNodeID;
		int N = child->count();
		std::string childNodeColor = N < 10000 ? "blue" : "purple";
		double V = child->value();
		int action = i;
		ssNodes << childId << "[label=\"action:" << model->GetActionDescription(action) << ","
				<< "N:" << N << ", V:" << V << "\",style=filled,fillcolor="<< childNodeColor <<",fontcolor=white];" << endl;
		ssEdges << "\"" << _nodeID << "\" -> \"" << childId << "\" [ label=\"\" , penwidth=2, color=\"black\"]" << endl;

		for (std::map<OBS_TYPE, VNode *>::iterator it = child->children().begin(); it != child->children().end(); ++it)
		{
			OBS_TYPE obs = it->first;
			VNode *vnodeChild = it->second;
			int vnodeChildId = ++currentNodeID;
			ssEdges << "\"" << childId << "\" -> \"" << vnodeChildId << "\" [ label=\""
					<< "Observation:" << model_->PrintObs(action, obs) << "\" , penwidth=2, color=\"black\"]" << endl;
			POMCP::GenerateDotGraphVnode(vnodeChild, currentNodeID, ssNodes, ssEdges, depthLimit, model);
		}
	}
}
*/

std::string POMCP::GenerateDebugJson(VNode* root, int depthLimit, const DSPOMDP* model)
{
	stringstream ssNodes;
	stringstream ssEdges; 
	
	ssNodes << "{\"observation\":\"root\"" << endl;
	ssEdges << "";
	int currentNodeID = 0;
	POMCP::GenerateDebugJsonVnode(root, ssNodes, depthLimit, model_);

	
	ofstream MyFile("/home/or/AOS/debug.json");
	MyFile << ssNodes.str();
	MyFile.close();
	//run: "dot -Tpdf  debug.dot > debug.pdf"
	system("(cd /home/or/AOS;dot -Tpdf  debug.dot > debug.pdf)");
	return ssNodes.str();
}

void POMCP::GenerateDebugJsonVnode(VNode* vnode, stringstream &ss, int depthLimit, const DSPOMDP* model)
{
	std::string stateDesc = "";
	try 
	{
		if(vnode->belief() != NULL)
			std::vector<State *> vs = vnode->belief()->Sample(1);
		//stateDesc = model_->PrintStateStr(*());
	} 
	catch (std::exception& e)
	{ 
		stateDesc = "";
		
		 }
	if(depthLimit >= 0 && vnode->depth() >= depthLimit)
	{
		ss << "}";
		return;
	}
	ss << ",\"actions\":[";
	for (int i = 0; i < vnode->children().size(); i++)
	{
		if(i > 0)ss << ",";
		ss << "{";
		QNode *child = vnode->children()[i];
		int N = child->count();
		double V = child->value();
		int action = i;
		
		ss << "\"action\":"<< "\"" << model->GetActionDescription(action)<< "\","<< endl;
		ss << "\"count\":"<< N << "," << "\"value\":"<< V << ","<< endl;

		ss << "\"observations\":[";
		int j = 0;
		for (std::map<OBS_TYPE, VNode *>::iterator it = child->children().begin(); it != child->children().end(); ++it,j++)
		{
			OBS_TYPE obs = it->first;
			if(j > 0)
				ss << ",";
			ss << "{\"observation\":"
					<< "\"" << model_->PrintObs(action, obs) << "\"" << endl;

			VNode *vnodeChild = it->second; 
			POMCP::GenerateDebugJsonVnode(vnodeChild, ss, depthLimit, model);

		}
		ss << "]";
		ss << "}";
	}
	ss << "]";
	ss << "}";
}

 
} // namespace despot
