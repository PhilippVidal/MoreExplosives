class CfgPatches
{
    class CustomMOEConfig
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[] = {
            "MoreExplosives_Config"
        };
    };
};

/////////////////////////////////////
///// General Mod Configuration /////
/////////////////////////////////////

class CfgMoreExplosives 
{
	//Can only object with door-like construction parts be raided? (i.e., any built construction part that has 'is_gate = 1' set)
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
			damage = 1000;
			fullDamageRange = 2;
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


////////////////////////////////
///// Object Configuration /////
////////////////////////////////

class CfgVehicles
{
	///////////////////////
	///// MOE Objects /////
	///////////////////////

	class MOE_ItemBase;
	class MOE_ExplosiveBase;
	class MOE_DetonatorBase;
	class MOE_ComponentReceiverBase;
	class MOE_ComponentTimerBase;
	
	//////////////////////
	///// Explosives /////
	//////////////////////

	class MOE_Explosive_Homemade : MOE_ExplosiveBase
	{
		ammoType = "MOE_Ammo_ExplosionHomemade";

		class MOE_Settings
		{		
			timeToArm = 15;
			timetoDisarm = 15;
			timeToDefuse = 20;
			timeToMount = 20;
			timeToDismount = 20;

			explodeOnRuinedChance = 0.0;
			ruinAttachmentsOnRuinedChance = 1.0;

			hasToBeMounted = 0;
			canTakeDamageWhileMounted = 0;
			canTakeDamageWhileArmed = 0;
			canOnlyRaidDoors = 0;
			
			explosionBehaviour = "MOE_SingleExplosion";

			attachableTriggers[] = {
				"MOE_Timer_Homemade",
				"MOE_Receiver_Homemade"
			};		

			armTools[] = {
				"Unarmed"
			};
			disarmTools[] = {
				"Unarmed"
			};
			defuseTools[] = {
				"Unarmed"
			};	
		};
    };

	class MOE_Explosive_Military : MOE_ExplosiveBase
	{
		ammoType = "MOE_Ammo_ExplosionMilitary";

		class MOE_Settings
		{		
			timeToArm = 15;
			timetoDisarm = 15;
			timeToDefuse = 20;
			timeToMount = 20;
			timeToDismount = 20;

			explodeOnRuinedChance = 0;
			ruinAttachmentsOnRuinedChance = 1.0;

			hasToBeMounted = 0;
			canTakeDamageWhileMounted = 0;
			canTakeDamageWhileArmed = 0;
			canOnlyRaidDoors = 0;

			explosionBehaviour = "MOE_SingleExplosion";

			attachableTriggers[] = {
				"MOE_Timer_Military",
				"MOE_Receiver_Military"
			};

			armTools[] = {
				"Unarmed"
			};

			disarmTools[] = {
				"Unarmed"
			};

			defuseTools[] = {
				"Unarmed"
			};	
		};
    };

	//////////////////////
	///// Detonators /////
	//////////////////////

	class MOE_Detonator_Homemade : MOE_DetonatorBase
	{
		class MOE_Settings
		{		
			maxConnectedReceivers = 2;
			maxRange = 300;
			
			connectableReceivers[] = {
				"MOE_Receiver_Homemade"
			};

			failureChance = 0;
			triggerOnRuinedChance = 1.0;

			detonatorModes[] = {
				0, 1
			};
		};
	};

	class MOE_Detonator_Military : MOE_DetonatorBase
	{
		class MOE_Settings
		{		
			maxConnectedReceivers = 5;
			maxRange = 12000;
			connectableReceivers[] = {
				"MOE_Receiver_Military"
			};

			detonatorModes[] = {
				0, 1, 2
			};
		};
	};


	/////////////////////
	///// Receivers /////
	/////////////////////

	class MOE_Receiver_Homemade: MOE_ComponentReceiverBase
	{
		class MOE_Settings
		{
			maxConnectedSignalSources = 3;
			canBeConnectedToWhenExplosiveArmed = 1;

			class Light 
			{
				colors[] = {
					0, 0, 1
				};

				radius = 1;
				brightness = 3;
			};	
		};
	};

	class MOE_Receiver_Military: MOE_ComponentReceiverBase
	{
		class MOE_Settings
		{
			maxConnectedSignalSources = 5;
			canBeConnectedToWhenExplosiveArmed = 1;

			class Light 
			{
				colors[] = {
					0, 1, 0
				};

				radius = 1;
				brightness = 3;
			};	
		};
	};


	//////////////////
	///// Timers /////
	//////////////////

	class MOE_Timer_Homemade : MOE_ComponentTimerBase
	{
		class MOE_Settings
		{
			minTimer = 30;
			maxTimer = 300;
			initialTickrate = 2;
			tickSpeedIncreaseFactor = 2;
			timerStages[] = {
				0.5,						
				0.75,	
				0.875	
			};

			beepingSoundset = "MOE_SoundSet_Beeping";

			class Light 
			{
				colors[] = {
					0, 1, 0,
					1, 1, 0,
					1, 0, 0
				};

				radius = 2;
				brightness = 3;
			};

			class Noise
			{			
				strength=100;
				type="sound";	
			};			
		};
	};
	
	class MOE_Timer_Military : MOE_ComponentTimerBase
	{
		class MOE_Settings
		{
			minTimer = 30;
			maxTimer = 300;
			initialTickrate = 1.5;
			tickSpeedIncreaseFactor = 2;
			timerStages[] = {
				0.5,						
				0.75,	
				0.875	
			};

			beepingSoundset = "MOE_SoundSet_Beeping";

			class Light 
			{
				colors[] = {
					0, 1, 0,
					1, 1, 0,
					1, 0, 0
				};

				radius = 2;
				brightness = 3;
			};

			class Noise
			{			
				strength=100;
				type="sound";	
			};			
		};
	};



	/////////////////////////////////
	///// Base Building Objects /////
	/////////////////////////////////

	class Inventory_Base;
	class BaseBuildingBase;
	class Fence : BaseBuildingBase
	{
		class MOE_Settings
		{
			//Entity multipliers for each explosive type
			//if not defined defaults to 1
			class DamageMultipliers
			{
				MOE_Explosive_Homemade = 1;
				MOE_Explosive_Military = 1;
			};
		};

		class DamageSystem
		{
			class DamageZones
			{
				class Wall_Base_Down
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 13;
							MOE_Explosive_Military = 13;
						};
					};
				};

				class Wall_Wood_Down
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 8.5;
							MOE_Explosive_Military = 8.5;
						};
					};
				};

				class Wall_Metal_Down
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 6.25;
							MOE_Explosive_Military = 6.25;
						};
					};
				};

				class Wall_Platform
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 19;
							MOE_Explosive_Military = 19;
						};
					};
				};
			};
		};
	};

	class Watchtower : BaseBuildingBase
	{	
		class MOE_Settings
		{
			class DamageMultipliers
			{
				MOE_Explosive_Homemade = 1;
				MOE_Explosive_Military = 1;
			};
		};

		class DamageSystem
		{
			class DamageZones
			{
				class Level_1_Stairs
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 8.5;
							MOE_Explosive_Military = 8.5;
						};
					};
				};

				class Level_1_Wall_1_Base_Down
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 13;
							MOE_Explosive_Military = 13;
						};
					};
				};

				class Level_1_Wall_1_Wood_Down
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 8.5;
							MOE_Explosive_Military = 8.5;
						};
					};
				};

				class Level_1_Wall_1_Metal_Down
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 6.25;
							MOE_Explosive_Military = 6.25;
						};
					};
				};

				class Level_3_Roof
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 10;
							MOE_Explosive_Military = 10;
						};
					};
				};
			};
		};
		

	};
};

