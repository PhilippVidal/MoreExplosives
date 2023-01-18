class CfgPatches
{
    //Change this to your server's name 
    class ServerName_MOEConfigTemplate
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[] = {
			"MoreExplosives_Config"
        };
    };
};

class CfgMoreExplosives
{
    doorRaidOnlyEnabled = 1;
    deleteLocks = 1;
    raidOnlyWithMOE = 1;

    raidSchedulingEnabled = 1;

    class RaidScheduling
    {
        timeZoneOffset = 1;

        class Weekday;
        class Friday : Weekday 
        {
            timeSlots[] = {
                12.0, 24.0
            };
        };
    };
};

class CfgAmmo
{
    class MOE_Ammo_ExplosionBase;
    class MOE_Ammo_ExplosionMilitary : MOE_Ammo_ExplosionBase
	{	
		indirectHitRange = 14;

		class MOE_Settings 
		{
			damage = 5000;
			fullDamageRange = 7;
		};
	};
}

class CfgVehicles
{
    class MOE_ExplosiveBase;
    class MOE_ComponentTimerBase;


    class MOE_Explosive_Military : MOE_ExplosiveBase
    {
        class MOE_Settings
        {
            timeToMount = 60;
            hasToBeMounted = 1;
            canTakeDamageWhileMounted = 0;

            attachableTriggers[] += {
				"MOE_Timer_Homemade",
				"MOE_Receiver_Homemade"
			};
        };
    };


    class MOE_Timer_Homemade : MOE_ComponentTimerBase
    {
        class MOE_Settings
		{
			minTimer = 120;
			maxTimer = 120;			
		};
    }; 

    class MOE_Timer_Military : MOE_ComponentTimerBase
    {
        class MOE_Settings
		{
			minTimer = 90;
			maxTimer = 90;			
		};
    }; 

};
