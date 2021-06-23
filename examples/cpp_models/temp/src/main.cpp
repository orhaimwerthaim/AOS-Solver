

#include <iostream>
#include <random>
#include <map>
#include <string>
#include <unordered_map>
#include "cpplinq.hpp"


// int computes_a_sum ()
// {
// using namespace cpplinq;
// int ints = {3,1,4,1,5,9,2,6,5,4};

// // Computes the sum of all even numbers in the sequence above
// return 
//         from_array (ints)
//     >>  where ([]()(int i) {return i%2 ==0;})     // Keep only even numbers
//     >>  sum ()                                  // Sum remaining numbers
//     ;
// }


enum test
{
  a,
  b,
  c
};
std::string distribution1_keys[] = {"a", "b","c"};
std::default_random_engine generator;
std::discrete_distribution<> distribution{2, 200, 1000};
std::unordered_map<std::string, std::discrete_distribution<>> dist;
std::string sampleStr()
{
  return distribution1_keys[distribution(generator)];
}


int main(int argc, char* argv[]) {
  std:
    //std::cout << computes_a_sum() << std::endl;
    // const int nrolls = 10000; // number of experiments
    // const int nstars = 100;   // maximum number of stars to distribute

    for (int i = 0; i < 20; i++)
    {

      std::cout << sampleStr() << std::endl;
  }
    // int i = 0.2000;
    // //std::discrete_distribution<> distribution{2, 2, 1, 1.0, 0.200};

    // int p[10]={};

    // for (int i=0; i<nrolls; ++i) {
    //   int number = distribution(generator);
    //   std::cout << "sampled:" << number <<std::endl;
    //   ++p[number];
    // }

    // std::cout << "a discrete_distribution:" << std::endl;
    // for (int i=0; i<10; ++i)
    //   std::cout << i << ": " << std::string(p[i]*nstars/nrolls,'*') << std::endl;

    return 0;
}
