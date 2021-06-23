#include "robocup_cleanroom.h"
#include <despot/core/pomdp.h> 

#include <despot/solver/pomcp.h>


using namespace std;

namespace despot {

/* ==============================================================================
 *CleanRoomBelief class
 * ==============================================================================*/
int CleanRoomBelief::num_particles = 50000;


CleanRoomBelief::CleanRoomBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	cleanroom_(static_cast<const CleanRoom*>(model)) {
}

void CleanRoomBelief::Update(int action, OBS_TYPE obs) {
	history_.Add(action, obs);

	vector<State*> updated;
	double reward;
	OBS_TYPE o;
	int cur = 0, N = particles_.size(), trials = 0;
	while (updated.size() < num_particles && trials < 10 * num_particles) {
		State* particle = cleanroom_->Copy(particles_[cur]);
		bool terminal = cleanroom_->Step(*particle, Random::RANDOM.NextDouble(),
			action, reward, o);

		if (!terminal && o == obs)
			//|| cleanroom_->LocalMove(*particle, history_, obs)) 
			{
			updated.push_back(particle);
		} else {
			cleanroom_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		cleanroom_->Free(particles_[i]);

	particles_ = updated;

	for (int i = 0; i < particles_.size(); i++)
		particles_[i]->weight = 1.0 / particles_.size();
}

/* ==============================================================================
 * CleanRoom class
 * ==============================================================================*/

CleanRoom::CleanRoom(){
	// NumObservations = 1 << 10;
	// See ghost N
	// See ghost E
	// See ghost S
	// See ghost W
	// Can move N
	// Can move E
	// Can move S
	// Can move W
	// Smell food
	// Hear ghost
}






int CleanRoom::NumActions() const {
	return globals::NUM_OF_ACTIONS;
}

double CleanRoom::ObsProb(OBS_TYPE obs, const State& state, int action) const {
	return 0.9;
}

 


std::default_random_engine CleanRoom::generator;

//generated for line: sampleDiscrete(tName,{0.25, 0.25,0.25,0.25});
std::discrete_distribution<> CleanRoom::dist1{0.25, 0.25,0.25,0.25};


State* CleanRoom::CreateStartState(string tyep) const {
	CleanRoomState* startState = memory_pool_.Allocate();
	startState->person1 = tPerson();
	startState->person2 = tPerson();
	startState->person3 = tPerson();
	startState->person4 = tPerson();

	startState->tPersonObjects.push_back(startState->person1);
	startState->tPersonObjects.push_back(startState->person2);
	startState->tPersonObjects.push_back(startState->person3);
	startState->tPersonObjects.push_back(startState->person4);

	tName name1 = tName();
	tName name2 = tName();
	tName name3 = tName();
	tName name4 = tName();

	startState->tNameObjects.push_back(startState->name1);
	startState->tNameObjects.push_back(startState->name2);
	startState->tNameObjects.push_back(startState->name3);
	startState->tNameObjects.push_back(startState->name4);

	startState->reported = false;
	
	startState->name1.enumName = eName1; startState->name1.actual_name = "Ronen";
	startState->name2.enumName = eName2; startState->name2.actual_name = "Tal";
	startState->name3.enumName = eName3; startState->name3.actual_name = "Or";
	startState->name4.enumName = eName4; startState->name4.actual_name = "Shashank";
	for (int i = 0; i < startState->tPersonObjects.size();i++)//generated for line: "forEach":[{"globalArrayByType":"tPerson", "itemNameInCode":"_person"}],
	{
		tPerson &_person = startState->tPersonObjects[i];
		_person.nameConfirmed = false;_person.reported=false;
		_person.name = &startState->tNameObjects[dist1(generator)];//generated for line: sampleDiscrete(tName,{0.25, 0.25,0.25,0.25});
	}
 
	return startState;
}



Belief* CleanRoom::InitialBelief(const State* start, string type) const {
	int N = CleanRoomBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}

	return new CleanRoomBelief(particles, this);
}
 

 

POMCPPrior* CleanRoom::CreatePOMCPPrior(string name) const { 
		return new UniformPOMCPPrior(this);
}

void CleanRoom::PrintState(const State& state, ostream& ostr) const {
	const CleanRoomState& farstate = static_cast<const CleanRoomState&>(state);
	ostr << "called PrintState(): state printed"<<endl;
	
}

void CleanRoom::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const CleanRoomState& farstate = static_cast<const CleanRoomState&>(state);
	
	ostr << "called PrintObs(): obs printed"<<endl;
}

void CleanRoom::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void CleanRoom::PrintAction(int action, ostream& out) const {
	out << "action:" << action << endl;
}

State* CleanRoom::Allocate(int state_id, double weight) const {
	CleanRoomState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* CleanRoom::Copy(const State* particle) const {
	CleanRoomState* state = memory_pool_.Allocate();
	*state = *static_cast<const CleanRoomState*>(particle);
	state->SetAllocated();
	return state;
}

void CleanRoom::Free(State* particle) const {
	memory_pool_.Free(static_cast<CleanRoomState*>(particle));
}

int CleanRoom::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

bool CleanRoom::Step(State& state, double rand_num, int action, double& reward,
	OBS_TYPE& observation) const {
	bool isNextStateFinal = false;
	Random random(rand_num);
	int moduleExecutionTime = -1;
	bool meetPrecondition = false;

	CleanRoomState& farstate = static_cast<CleanRoomState&>(state);
	CheckPreconditions(farstate, reward, meetPrecondition);
	if (!meetPrecondition)
	{
		return false;
	}

	State *s_initState = Copy(&state);
	CleanRoomState &initState = static_cast<CleanRoomState &>(*s_initState);

	
	SampleModuleExecutionTime(farstate, rand_num, action, moduleExecutionTime);

	ExtrinsicChangesDynamicModel(initState, farstate, rand_num, action, reward, moduleExecutionTime);

	State *s_afterExState = Copy(&state);
	CleanRoomState &afterExState = static_cast<CleanRoomState &>(*s_afterExState);

	ModuleDynamicModel(initState, afterExState, farstate, rand_num, action, reward,
					   observation, moduleExecutionTime);
	//CleanRoomState& afterExtrinsicChanges = static_cast<CleanRoomState&>(Copy(&state));
	
	Free(s_initState);
	Free(s_afterExState);
	return CheckIfFinalState(farstate);
}

void CleanRoom::CheckPreconditions(const CleanRoomState& farstate, double &reward, bool &meetPrecondition) const
	{
	}

void CleanRoom::SampleModuleExecutionTime(const CleanRoomState& farstate, double rand_num, int action, int &moduleExecutionTime) const
	{
		int i = 2;
	}

void CleanRoom::ExtrinsicChangesDynamicModel(const CleanRoomState& initState, CleanRoomState& afterExState, double rand_num, int action, double& reward,
		const int &moduleExecutionTime)  const
	{	
		int i = 2;
	}

	void CleanRoom::ModuleDynamicModel(const CleanRoomState &initState, const CleanRoomState &afterExState, CleanRoomState &nextState, double rand_num, int action, double &reward,
									  OBS_TYPE &observation, const int &moduleExecutionTime) const
	{
		int i = 2;
	}

bool CleanRoom::CheckIfFinalState(const CleanRoomState &farstate) const
{
	return false;
}
}// namespace despot
