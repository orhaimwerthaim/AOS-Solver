
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
        ((float)state->grid/(float)1)
    ,((float)state->isRobotTurn/(float)1)
}; 
            torch::Tensor f = torch::from_blob(state_as_vec, {1,2}); 
            inputs.push_back(f);
             torch::jit::IValue output = module.forward(inputs);
             torch::Tensor t0 = output.toTuple()->elements()[0].toTensor();
             auto sizes = t0.sizes();
             int action = sizes.size() == 1 ? t0.argmax(0).item().toInt() : t0.argmax(1).item().toInt();
    return action;
    }
    }
#endif //TORCH_MODEL_HPP
