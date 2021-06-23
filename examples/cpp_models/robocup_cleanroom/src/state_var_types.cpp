#include <string>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include "state_var_types.h"

using namespace std;

namespace despot {


tPerson::tPerson() {
	observed = false;
	nameConfirmed = false;
	reported = false;
	person_description = "";
	observed_location_description = "";
	av_observed_location = false;
	male = true;
	willing_to_leave = false;
	confirmed_willing_to_leave = false;
	name = NULL;
}
tName::tName(){
	enumName = eEmpty;
	actual_name = "";
}

}// namespace despot
