#define BUILDING_TRACE ECC_GameTraceChannel3

#define TEAM_NEUTRAL_PASSIVE 255
#define TEAM_NEUTRAL_AGGRESSIVE 254

#define GENERATE_FLAG_GETTER(fieldName, flagType, flagName)	\
	bool Get##flagName##Flag() const	\
	{	\
		return fieldName & static_cast<uint8>(flagType::##flagName);	\
	}

#define GENERATE_FLAG_SETTER(fieldName, flagType, flagName)	\
	void Set##flagName##Flag(bool isSet)	\
	{	\
		if (isSet)	\
		{	\
			fieldName |= static_cast<uint8>(flagType::##flagName);	\
		}	\
		else\
		{	\
			fieldName &= ~static_cast<uint8>(flagType::##flagName);	\
		}	\
	}	\