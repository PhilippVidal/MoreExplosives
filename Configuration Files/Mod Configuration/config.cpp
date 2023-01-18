//This is a simple template that can be used to configure the mod 
//Replace `YourServer` with the name of your server 
//The following files can be referenced as examples: 
//	https://github.com/PhilippVidal/MoreExplosives/blob/main/MoreExplosives/Config/config.cpp
// 	https://github.com/PhilippVidal/MoreExplosives/blob/main/MoreExplosives/Data/config.cpp
//For a comprehensive list of all available settings visit: 
// 	https://github.com/PhilippVidal/MoreExplosives/wiki/Installation-&-Configuration#mod-settings

class CfgPatches
{
    class YourServer_MOEConfig
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[] = {
            "MoreExplosives_Config" //Make this mod load after the default mod configuration
        };
    };
};



/////////////////////////////////////
///// General Mod Configuration /////
/////////////////////////////////////

//Uncomment this if you want to change any of the general mod settings
//class CfgMoreExplosives 
//{
	//Add your changes here...

	//Uncomment this if you want to add time slots for raiding
//	class RaidScheduling
//	{

		//class Weekday;	//Base class for each day

		//Add your changes here...

		//Example:
		//class Monday : Weekday
		//{
			//Add your changes here...
		//};
//	};
//};



////////////////////////////////////////
///// Ammo/Explosion Configuration /////
////////////////////////////////////////

//Uncomment this if you want to make changes to the ammo types used (e.g., base damage, range, etc.)
//class CfgAmmo
//{
//	class MOE_Ammo_ExplosionBase;	//Base class used for all MOE ammo types

	//Add your changes here...

	//Example:
	//class AmmoType_You_Want_To_Change : MOE_Ammo_ExplosionBase
	//{
		//Add your changes here...
	//};
//};



////////////////////////////////
///// Object Configuration /////
////////////////////////////////

//Uncomment this if you want to make changes to any of the mod items or other base building objects
//class CfgVehicles
//{
	//Base classes for the MOE mod items 
	//class MOE_ItemBase;
	//class MOE_ExplosiveBase;
	//class MOE_DetonatorBase;
	//class MOE_ComponentReceiverBase;
	//class MOE_ComponentTimerBase;
	

	//Add your changes here...

	//Examples:
	//class Explosive_You_Want_To_Change : MOE_ExplosiveBase
	//{
		//Add your changes here...
	//};

	//class Timer_You_Want_To_Change : MOE_ComponentTimerBase
	//{
		//Add your changes here...
	//};

//};

