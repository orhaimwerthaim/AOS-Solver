//https://download.pytorch.org/whl/torch_stable.html
//torch-1.13.1+cpu-cp39-cp39-linux_x86_64
#ifndef TORCH_MODEL_HPP
#define TORCH_MODEL_HPP
#include <torch/script.h>
#include <iostream> 
#include <memory>
#include <despot/core/pomdp.h>
//#include "aos_domain_for_python.hpp"
#include <chrono>
//#include "mongoDB_Bridge.h"
//using namespace aos_model2;
using namespace std;

namespace torch_model{
 torch::jit::script::Module module;
bool wasInit=false;
void Init()
{
  wasInit=true;
//std::string path ("/home/or/ML_logs/iros:PPO:C15595340062B36513B57E8397AF59D2D59B654B5D33225FDFF7E2F74ACF45D5.pt");
std::string path ("/home/or/ML_logs/collectValuableToys:DQN:6184D29EA165B94AD86F8FC37A5EF4B23CA4782145EBE9F9891FDB2BCB6942B8.pt");
  // if (argc != 2) {
  //   std::cerr << "usage: example-app <path-to-exported-script-module>\n";
  //   return -1;
  // }

  try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load(path);
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model\n";
    return;
  }
}


int getActionFromNN(despot::State* state)//, torch::jit::script::Module module)
{
  if(!wasInit)
  {
    torch_model::Init();
  } 
   std::vector<torch::jit::IValue> inputs;
   float state_as_vec[]={
    (int)state->pickActionsLeft,
   (int)state->robotArm,
   (int)state->robotLocation,
   (int)state->toy1.location,
   (int)state->toy1.reward,
   (int)state->toy2.location,
   (int)state->toy2.reward,
   (int)state->toy3.location,
   (int)state->toy3.reward,
   (int)state->toy4.location,
   (int)state->toy4.reward}; 
  //  float state_as_vec[]={(int)state->Cell1.content, state->Cell1.location, 
  //                         (int)state->Cell2.content, state->Cell2.location,
  //                         (int)state->Cell3.content, state->Cell3.location,
  //                         (int)state->Cell4.content, state->Cell4.location,
  //                         (int)state->Cell5.content, state->Cell5.location,
  //                         (int)state->Cell6.content, state->Cell6.location,
  //                         (int)state->Cell7.content, state->Cell7.location,
  //                         (int)state->Cell8.content, state->Cell8.location,
  //                         (int)state->Cell9.content, state->Cell9.location,
  //                         (int)state->humanSymbol, (int)state->isRobotTurn};
   torch::Tensor f = torch::from_blob(state_as_vec, {1,11}); 
   inputs.push_back(f);
  torch::jit::IValue output = module.forward(inputs);
 // std::cout << "output:" << output << endl;
  torch::Tensor t0 = output.toTuple()->elements()[0].toTensor();
 // std::cout << "t0:" << t0 << endl;
 // std::cout << "t0.size():" << t0.sizes() << endl;
  auto sizes = t0.sizes();
  //torch::Tensor t1 = output.toTuple()->elements()[1].toTensor();
  int action = sizes.size() == 1 ? t0.argmax(0).item().toInt() : t0.argmax(1).item().toInt();
  

//std::cout << "--------------------\n" << output << "\n";
  return action;
}


// void test(torch::jit::script::Module module)
//  {

//   State* state = CreateStartState();
//   std::cout << Prints::PrintState(*state);
//   bool finalState=false;
//   double reward;
//   OBS_TYPE observation;

// int count=0;
// auto Start = std::chrono::high_resolution_clock::now();
// 	while (1)
// 	{
//     int action = getActionFromNN(state, module);
//     count++;
// 		auto End = std::chrono::high_resolution_clock::now();
// 		std::chrono::duration<double, std::milli> Elapsed = End - Start;
// 		if (Elapsed.count() >= 1000.0)
// 			break;
// 	}

//   while(!finalState)
//   {
//     int action = getActionFromNN(state, module);
//     std::tuple<double, OBS_TYPE, bool> res = Step(*state,  action);
//     std::cout << Prints::PrintState(*state);
//     reward =  std::get<0>(res);
//     observation  =  std::get<1>(res);
//     finalState =  std::get<2>(res);

//   }

  
// }



// int run_test() {
// bool shutDown = false;
// 	bool isFirst = false;
// 	int solverId = 1;

// // despot::MongoDB_Bridge::Init();
// // 	despot::MongoDB_Bridge::GetSolverDetails(shutDown, isFirst, solverId);


//   std::cout << "PyTorch version: "
//     << TORCH_VERSION_MAJOR << "."
//     << TORCH_VERSION_MINOR << "."
//     << TORCH_VERSION_PATCH << std::endl;
//   //std::string path ("/home/or/ML_logs/collectValuableToys:PPO:A38F1E58E235A6F4BDAFD379AC7AF0A3EE5F5ECF5436400786F8744A1779D7BE.pt");
//   std::string path ("/home/or/ML_logs/iros:PPO:C15595340062B36513B57E8397AF59D2D59B654B5D33225FDFF7E2F74ACF45D5.pt");
//   // if (argc != 2) {
//   //   std::cerr << "usage: example-app <path-to-exported-script-module>\n";
//   //   return -1;
//   // }

//   torch::jit::script::Module module;
//   try {
//     // Deserialize the ScriptModule from a file using torch::jit::load().
//     module = torch::jit::load(path);
//   }
//   catch (const c10::Error& e) {
//     std::cerr << "error loading the model\n";
//     return -1;
//   }
//   std::cout << "Model "<< path<<" loaded fine\n";





// // torch::Tensor indata1 = torch::ones({12,64}, torch::kLong);
// // torch::Tensor h0 = torch::from_blob(std::vector(1* 64 * 200, 0.0).data(), {1, 64, 200});
// // torch::Tensor c0 = torch::from_blob(std::vector(1* 64 * 200, 0.0).data(), {1, 64, 200});
// // torch::jit::IValue tuple = torch::ivalue::Tuple::create({h0, c0});
// // at::Tensor output = module->forward({indata1, tuple}).toTensor();
//  test(module);
//   // Create a vector of inputs.
//   std::vector<torch::jit::IValue> inputs; 
//    //inputs.push_back(torch::randn({1, 1, 2, 11}));
// // inputs.push_back(torch::randn({1, 1, 1, 11}));
// //   // Execute the model and turn its output into a tensor.
// //   torch::jit::IValue output = module.forward(inputs);
// //   torch::Tensor t0 = output.toTuple()->elements()[0].toTensor();
// // torch::Tensor t1 = output.toTuple()->elements()[1].toTensor();
// //    std::cout << output << "\n";
 
  
//  // int y_hat = t0.argmax(1).item().toInt();
//   //std::cout << "Predicted class: " << y_hat <<"\n";
// }

}
#endif //TORCH_MODEL_HPP