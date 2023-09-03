
    #ifndef TORCH_MODEL_HPP
#define TORCH_MODEL_HPP
#include <torch/script.h>
#include <iostream> 
#include <memory> 
#include <despot/core/pomdp.h>
#include <chrono>
using namespace std;

namespace torch_model{
 torch::jit::script::Module module;
bool wasInit=false;
int counter=0;
void Init()
{
  wasInit=true;
  std::string path ("/home/or/AOS/AOS-Solver/mdpNN.pt");
      try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load(path);
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model\n";
    return;
  }
}

    
            int getActionFromNN(despot::State* state)
{
  if(!wasInit)
  {
    counter=0;
    torch_model::Init();
  } 
   counter++;
   std::vector<torch::jit::IValue> inputs;
   float state_as_vec[]={
        ((float)state->tGreen.location/(float)1)
    ,((float)state->tGreen.reward/(float)1)
    ,((float)state->observed_reward/(float)1)
    ,((float)state->tBlue.location/(float)1)
    ,((float)state->tBlue.reward/(float)1)
    ,((float)state->tBlack.location/(float)1)
    ,((float)state->tBlack.reward/(float)1)
    ,((float)state->tRed.location/(float)1)
    ,((float)state->tRed.reward/(float)1)
    ,((float)state->pickActionsLeft/(float)1)
    ,((float)state->robotLocation/(float)1)
}; 
            torch::Tensor f = torch::from_blob(state_as_vec, {1,11}); 
            inputs.push_back(f);
             torch::jit::IValue output = module.forward(inputs);
             torch::Tensor t0 = output.toTuple()->elements()[0].toTensor();
             auto sizes = t0.sizes();
             int action = sizes.size() == 1 ? t0.argmax(0).item().toInt() : t0.argmax(1).item().toInt();

             cout << endl << endl << "sizes.size():" << sizes.size() << endl;
    cout << endl << endl << "t0:" << t0 << endl << endl;
    cout << endl << endl << "action:" << action << endl << endl;
    cout << "state->tGreen.location:" << state->tGreen.location << endl;
    cout << "state->tGreen.reward:" << state->tGreen.reward << endl;
    cout << "state->tBlue.location:" << state->tBlue.location << endl;
    cout << "state->tBlue.reward:" << state->tBlue.reward << endl;
    cout << "state->tBlack.location:" << state->tBlack.location << endl;
    cout << "state->tBlack.reward:" << state->tBlack.reward << endl;
    cout << "state->tRed.location:" << state->tRed.location << endl;
    cout << "state->tRed.reward:" << state->tRed.reward << endl;
    cout << "state->observed_reward:" << state->observed_reward << endl;
    cout << "state->robotLocation:" << state->robotLocation << endl;
    cout << "state->pickActionsLeft:" << state->pickActionsLeft << endl<< endl<< endl;
    return action;
    }
    }
#endif //TORCH_MODEL_HPP
