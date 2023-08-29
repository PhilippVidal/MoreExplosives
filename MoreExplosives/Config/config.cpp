//This config contains the default configuration for the mod 
//You can use it as a reference for which settings are available
//Don't just copy-paste the entire thing, that would be useless
class CfgPatches
{
    class MoreExplosives_Config
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
	class MoreExplosives
	{
		dir = "MoreExplosives";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "More Explosives";
		credits = "Handsome Nipple";
		author = "Handsome Nipple";	
		authorID = "0"; 
		version = "0.1"; 
		extra = 0;
		type = "mod";	
	};
};

/////////////////////////////////////
///// General Mod Configuration /////
/////////////////////////////////////

class CfgMoreExplosives 
{
	//Can base building objects only be raided with MOE explosives? 
	raidOnlyWithMOE = 0;

	//Is raiding only possible during the set timeslots? 
	raidSchedulingEnabled = 0;

	//Can only object with door-like construction parts be raided? (i.e., any built construction part has 'is_gate = 1' set)
	doorRaidOnlyEnabled = 0;

	//Should locks be deleted after an object has successfully been raided? 
	//Without this enabled vanilla dayz locks will drop to the ground when the object/part gets destroyed
	deleteLocks = 0;

	//Should the base be destroyed after an object has successfully been raided? 
	//Some parts on base building objects are marked as being a "base" for the object (e.g., the two wooden logs on each side of vanilla dayz fences)
	//Those parts are generally unaffected by damage, but enabling this option will delete them automatically once all other parts are destroyed
	destroyBaseAfterDestruction = 0;


	//Should explosives be attached to whatever target (if any) they are placed on?
	//For example explosives that are placed on the gate of a fence will move with the gate when it is opened
	//Currently does not work with BBP
	parentExplosivesToTarget = 1;


	//0 = No Area Damage will be dealt to base building objects (also turn on damagePlacementTargetDirectly for this!)
	//1 = Any base building object hit by a vanilla dayz explosion will be hit
	//2 = Any base building object that is within the set radius will be hit (manual sphere check)
	areaDamageMode = 1;

	//Which damage system should be used to damage base building objects? 
	//0 = DayZ	(Vanilla)
	//1 = MOE	(Modified)
	selectedDamageSystem = 1;

	//Which base building objects should be cached on 
	//server start-up instead of during the server session?
	//Useful for objects that have many different damage zones and 
	//therefore a lot of data to cache, which could lead to performance issues 
	//when loaded during the actual server session  
	baseBuildingEntitiesToPreload[] = {
		"Watchtower"
	};

	//Only relevant if raid scheduling is enabled 
	class RaidScheduling
	{
		//Offset in hours from UTC (Coordinated Universal Time)
		//Does not automatically account for summer/winter/daylight savings time!
		//e.g. UTC Time = 5pm, Local Time = 7pm => offset = 2
		timeZoneOffset = 1;
		
		//Override this if you want to easily apply the same time slots to each day 
		class Weekday
		{
			timeSlots[] = {};
		};

		class Monday : Weekday
		{
			//Example: 	raiding possible from 1:30 am till 9:49 am 
			//			and from noon till 6 pm
			//timeSlots[] = {
			//	1.5, 9.81667,    //1:30 am till 9:49 am
			//	12.0, 18.0	     //noon till 6 pm
			//};
		};

		class Tuesday : Weekday {};

		class Wednesday : Weekday {};

		class Thursday : Weekday {};

		class Friday : Weekday {};

		class Saturday : Weekday {};

		class Sunday : Weekday {};
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

			//How likely is an explosion once the explosive becomes ruined? [0.0 - 1.0]
			explodeOnRuinedChance = 0.0;
			//How likely is it for the attachments (i.e. an attached trigger) to become ruined? [0.0 - 1.0]
			ruinAttachmentsOnRuinedChance = 1.0;

			//Does the explosive have to be mounted before it can explode? 
			hasToBeMounted = 0;
			//Can the explosive take damage while mounted or armed?
			canTakeDamageWhileMounted = 0;
			canTakeDamageWhileArmed = 0;
			//Can this explosive only be used to raid doors? (based on whether an object has a construction part with is_gate = 1)
			canOnlyRaidDoors = 0;
			
			//Can this explosive only damage the target is was placed on, no other base building object?
			canOnlyDamagePlacementTarget = 0;
			//Can this explosive only be mounted during a raid schedule slot?
			canOnlyMountDuringSchedule = 0;
			
			//On which targets can the explosive be mounted? 
			//0 = Everywhere
			//1 = Only on Base Building objects
			//2 = Only on selected objects
			mountingMode = 0;
			
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
			timetoDisarm = 5;
			timeToDefuse = 20;
			timeToMount = 20;
			timeToDismount = 20;

			explodeOnRuinedChance = 0;
			ruinAttachmentsOnRuinedChance = 1.0;

			hasToBeMounted = 0;
			canTakeDamageWhileMounted = 0;
			canTakeDamageWhileArmed = 0;
			canOnlyRaidDoors = 0;

			canOnlyDamagePlacementTarget = 0;
			canOnlyMountDuringSchedule = 0;
	
			mountingMode = 0;
			
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
			
			//Which explosive types can be mounted on this object? 
			//if not defined defaults to 0 (false)
			//requires proper mounting mode to be set on explosive
			class MountableExplosives
			{
				MOE_Explosive_Homemade = 1;
				MOE_Explosive_Military = 1;
			};
			
			//Which parts can the explosive be mounted on? (0 or 1)
			//Having a sub-class for a specific explosive type defined enables this feature (hence why it's commented-out by default)
			//Does not work when mounting mode is set to "Everywhere"
			//Part names can be found under "CfgVehicles <CLASSNAME> Construction <MAIN_PART_NAME> <PART_NAME>"
			//class MountableParts
			//{
			//	class MOE_Explosive_Homemade
			//	{
			//		wall_metal_up = 1;
			//	};
			//};

			//Manually determined order in which the damage zones are damaged
			//zones that are not listed get appended at the end
			damageZonePriorities[]= 
			{
				"Wall_Metal_Up",
				"Wall_Metal_Down",
				"Wall_Wood_Up",
				"Wall_Wood_Down",
				"Wall_Base_Up",
				"Wall_Base_Down"
			};
		};

		class DamageSystem
		{
			class DamageZones
			{
				class Wall_Gate
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 0;
							MOE_Explosive_Military = 0;
						};
					};
				};

				class Wall_Base_Down
				{
					class MOE_Settings
					{
						//Which construction parts (not damage zones!) have to 
						//be destroyed (i.e., not build) for this zone to be damageable
						//i.e., as long as "wall_metal_down" or "wall_wood_down" are build, this zone will not be damaged
						requiredDestroyedParts[]={
							"wall_metal_down",
							"wall_wood_down"
						};

						//Damagezone multipliers for each explosive type
						//if not defined defaults to 1
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 13;
							MOE_Explosive_Military = 13;
						};
					};
				};
				class Wall_Base_Up : Wall_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"wall_metal_up",
							"wall_wood_up"
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
			
			//class MountableExplosives
			//{
			//	MOE_Explosive_Homemade = 1;
			//	MOE_Explosive_Military = 1;
			//};

			damageZonePriorities[]= {
				"Level_1_Wall_1_Metal_Up",
				"Level_1_Wall_1_Metal_Down",
				"Level_1_Wall_1_Wood_Up",
				"Level_1_Wall_1_Wood_Down",
				"Level_1_Wall_1_Base_Up",
				"Level_1_Wall_1_Base_Down",

				"Level_1_Wall_2_Metal_Up",
				"Level_1_Wall_2_Metal_Down",
				"Level_1_Wall_2_Wood_Up",
				"Level_1_Wall_2_Wood_Down",
				"Level_1_Wall_2_Base_Up",
				"Level_1_Wall_2_Base_Down",

				"Level_1_Wall_3_Metal_Up",
				"Level_1_Wall_3_Metal_Down",
				"Level_1_Wall_3_Wood_Up",
				"Level_1_Wall_3_Wood_Down",
				"Level_1_Wall_3_Base_Up",
				"Level_1_Wall_3_Base_Down",

				
				"Level_2_Wall_1_Metal_Up",
				"Level_2_Wall_1_Metal_Down",
				"Level_2_Wall_1_Wood_Up",
				"Level_2_Wall_1_Wood_Down",
				"Level_2_Wall_1_Base_Up",
				"Level_2_Wall_1_Base_Down",

				"Level_2_Wall_2_Metal_Up",
				"Level_2_Wall_2_Metal_Down",
				"Level_2_Wall_2_Wood_Up",
				"Level_2_Wall_2_Wood_Down",
				"Level_2_Wall_2_Base_Up",
				"Level_2_Wall_2_Base_Down",

				"Level_2_Wall_3_Metal_Up",
				"Level_2_Wall_3_Metal_Down",
				"Level_2_Wall_3_Wood_Up",
				"Level_2_Wall_3_Wood_Down",
				"Level_2_Wall_3_Base_Up",
				"Level_2_Wall_3_Base_Down",


				"Level_3_Wall_1_Metal_Up",
				"Level_3_Wall_1_Metal_Down",
				"Level_3_Wall_1_Wood_Up",
				"Level_3_Wall_1_Wood_Down",
				"Level_3_Wall_1_Base_Up",
				"Level_3_Wall_1_Base_Down",

				"Level_3_Wall_2_Metal_Up",
				"Level_3_Wall_2_Metal_Down",
				"Level_3_Wall_2_Wood_Up",
				"Level_3_Wall_2_Wood_Down",
				"Level_3_Wall_2_Base_Up",
				"Level_3_Wall_2_Base_Down",

				"Level_3_Wall_3_Metal_Up",
				"Level_3_Wall_3_Metal_Down",
				"Level_3_Wall_3_Wood_Up",
				"Level_3_Wall_3_Wood_Down",
				"Level_3_Wall_3_Base_Up",
				"Level_3_Wall_3_Base_Down"
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
						requiredDestroyedParts[]={
							"level_1_wall_1_base_down",
							"level_1_wall_1_base_up",
							"level_1_wall_2_base_down",
							"level_1_wall_2_base_up",
							"level_1_wall_3_base_down",
							"level_1_wall_3_base_up"
						};

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
						requiredDestroyedParts[]={
							"level_1_wall_1_wood_down",
							"level_1_wall_1_metal_down"
						};

						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 13;
							MOE_Explosive_Military = 13;
						};
					};
				};
				class Level_1_Wall_1_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_1_wall_1_wood_up",
							"level_1_wall_1_metal_up"
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
				class Level_1_Wall_2_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_1_wall_2_wood_down",
							"level_1_wall_2_metal_down"
						};
					};
				};
				class Level_1_Wall_2_Base_Up: Level_1_Wall_1_Base_Up
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_1_wall_2_wood_up",
							"level_1_wall_2_metal_up"
						};
					};
				};
				class Level_1_Wall_3_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_1_wall_3_wood_down",
							"level_1_wall_3_metal_down"
						};
					};
				};
				class Level_1_Wall_3_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_1_wall_3_wood_up",
							"level_1_wall_3_metal_up"
						};
					};
				};
				class Level_2_Stairs: Level_1_Stairs
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_2_wall_1_base_down",
							"level_2_wall_1_base_up",
							"level_2_wall_2_base_down",
							"level_2_wall_2_base_up",
							"level_2_wall_3_base_down",
							"level_2_wall_3_base_up"
						};
					};
				};
				class Level_2_Wall_1_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_2_wall_1_wood_down",
							"level_2_wall_1_metal_down"
						};
					};
				};
				class Level_2_Wall_1_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_2_wall_1_wood_up",
							"level_2_wall_1_metal_up"
						};
					};
				};
				class Level_2_Wall_2_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_2_wall_2_wood_down",
							"level_2_wall_2_metal_down"
						};
					};
				};
				class Level_2_Wall_2_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_2_wall_2_wood_up",
							"level_2_wall_2_metal_up"
						};
					};
				};
				class Level_2_Wall_3_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_2_wall_3_wood_down",
							"level_2_wall_3_metal_down"
						};
					};
				};
				class Level_2_Wall_3_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_2_wall_3_wood_up",
							"level_2_wall_3_metal_up"
						};
					};
				};
				class Level_3_Wall_1_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_3_wall_1_wood_down",
							"level_3_wall_1_metal_down"
						};
					};
				};
				class Level_3_Wall_1_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_3_wall_1_wood_up",
							"level_3_wall_1_metal_up"
						};
					};
				};
				class Level_3_Wall_2_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_3_wall_2_wood_down",
							"level_3_wall_2_metal_down"
						};
					};
				};
				class Level_3_Wall_2_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_3_wall_2_wood_up",
							"level_3_wall_2_metal_up"
						};
					};
				};
				class Level_3_Wall_3_Base_Down: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_3_wall_3_wood_down",
							"level_3_wall_3_metal_down"
						};
					};
				};
				class Level_3_Wall_3_Base_Up: Level_1_Wall_1_Base_Down
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"level_3_wall_3_wood_up",
							"level_3_wall_3_metal_up"
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

