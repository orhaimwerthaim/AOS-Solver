#include <string>
#include <iostream> 

namespace despot
{
	enum teNames
	{
		eName1,
		eName2,
		eName3,
		eName4,
		eEmpty
	};

	struct tName;
	struct tPerson
	{
		bool observed, nameConfirmed, reported;
		std::string person_description, observed_location_description;
		bool av_observed_location;

		bool male, willing_to_leave, confirmed_willing_to_leave;
		tName* name;

		tPerson(); 
	};


struct tName
{
	teNames enumName;
	std::string actual_name;
	tName();
};
} // namespace despot
