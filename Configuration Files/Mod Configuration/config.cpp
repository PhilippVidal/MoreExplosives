class CfgPatches
{
    class CustomMOEConfig
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[] = {
            "MoreExplosives_Data",
			"MoreExplosives_GUI",
			"MoreExplosives_Scripts"
        };
    };
};

class CfgMods
{
	class CustomMOEConfig
	{
		dir = "CustomMOEConfig";
		name = "CustomMOEConfig";
		type = "mod";	
	};
};

/////////////////////////////////////
///// General Mod Configuration /////
/////////////////////////////////////

class CfgMoreExplosives 
{
	//Can only object with door-like construction parts be raided? (i.e., any build construction part has 'is_gate = 1' set)
	doorRaidOnlyEnabled = 0;

	//Is custom damage for base building object enabled? 
	//If set to 0 explosives will act like vanilla dayz explosives
	customDamageEnabled = 1;

	//Can base building objects only be raided with MOE explosives? 
	raidOnlyWithMOE = 0;

	//Should locks be deleted after an object has successfully been raided?
	deleteLocks = 0;

	//Should the base be destroyed after an object has been successfully been raided? 
	destroyBaseAfterDestruction = 0;

	//Which base building objects should be cached on 
	//server start-up instead of during server session?
	//Useful for objects that have many different damage zones and 
	//therefore a lot of data to cache, which could lead to performance issues 
	//when loaded during the actual server session  
	baseBuildingEntitiesToPreload[] = {
		"Watchtower"
	};
};


//////////////////////////////
///// Ammo Configuration /////
//////////////////////////////

class CfgAmmo
{
	class MOE_Ammo_ExplosionBase;
	class MOE_Ammo_ExplosionHomemade : MOE_Ammo_ExplosionBase
	{	
		indirectHitRange = 5;
		particle = "explosion_landmine_01";
		class DamageApplied
		{
			type="FragGrenade";
			class Health
			{
				damage=150;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=50;
			};
		};

		class MOE_Settings 
		{
            //How much base damage should be dealt against base building objects?
			damage = 1000;

            //In what range will the base damage be at its highest? 
			fullDamageRange = 2;

            //Rotation for the played explosion particle
			particleOrientation = "0 -90 0";
		};
	};

	class MOE_Ammo_ExplosionMilitary : MOE_Ammo_ExplosionBase
	{	
		indirectHitRange = 7;
		particle = "explosion_landmine_01";
		class DamageApplied
		{
			type="FragGrenade";
			class Health
			{
				damage=300;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=100;
			};
		};

		class MOE_Settings 
		{
			damage = 2000;
			fullDamageRange = 2;
			particleOrientation = "0 -90 0";
		};
	};
};

