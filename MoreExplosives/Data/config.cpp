class CfgPatches
{
    class MoreExplosives_Data
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[] = {
            "DZ_Data"
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
		inputs="MoreExplosives\data\inputs.xml";
		authorID = "0"; 
		version = "0.1"; 
		extra = 0;
		type = "mod";	
	};
};

class CfgVehicles
{
	////////////////////////
	///// Base Classes /////
	////////////////////////

	class Inventory_Base;
	class MOE_ItemBase : Inventory_Base 
	{
		scope=0;
		isMeleeWeapon = 1;
	};

    class MOE_ExplosiveBase : MOE_ItemBase
    {
        overrideDrawArea = "3.0";
        forceFarBubble = "true";
		
		ammoType = "MOE_Ammo_ExplosionBase";

		attachments[] = {
			"Trigger_MOE"
		};

		class MOE_Settings
		{		
			//Times to manipulate the explosive in various ways (in seconds)
			timeToArm = 1;
			timetoDisarm = 1;
			timeToDefuse = 1;
			timeToMount = 1;
			timeToDismount = 1;

			//How likely is the explosive to explode when it becomes ruined? (0 - 1)
			explodeOnRuinedChance = 0;

			//How likely are the attachments to become ruined when the explosive becomes ruined? (0 - 1)
			//Is only relevant for when the explosive is getting ruined by something other than it exploding, 
			//as exploding also deletes the explosive afterwards 
			ruinAttachmentsOnRuinedChance = 0;

			//Does the explosive have to be mounted to be able to explode? (0 or 1)
			hasToBeMounted = 0;

			//Can the explosive take damage while mounted? 
			canTakeDamageWhileMounted = 0;

			//Can the explosive take damage while armed? 
			canTakeDamageWhileArmed = 0;

			//Can the explosive only deal damage to objects that have a door-like part?
			canOnlyRaidDoors = 0;
			
			//Which explosion behaviour does this explosive use?
			explosionBehaviour = "MOE_SingleExplosion";

			//What triggers can be used with this explosive? 
			attachableTriggers[] = {};
			
			//Which tools can be used to manipulate (Arm, Disarm, Defuse) the explosive? 
			//Use "Unarmed" for manipulation without any tools
			armTools[] = {};
			disarmTools[] = {};
			defuseTools[] = {};
		};
    };


	class MOE_SignalSource : MOE_ItemBase
	{
		class MOE_Settings
		{		
			//How many receivers can be connected to this source at once? 
			maxConnectedReceivers = 3;

			//What is the maximum range at which a receiver can be triggered? 
			maxRange = 100;

			//Which types of receivers can be connected to this source?
			connectableReceivers[] = {};

			//How likely is this source to fail when triggering a signal? (0 - 1)
			failureChance = 0;

			//How likely is this source to trigger a signal when it becomes ruined? (0 - 1)
			triggerOnRuinedChance = 0;
		};
	};

	class MOE_DetonatorBase : MOE_SignalSource
	{
		class MOE_Settings : MOE_Settings
		{		
			//Which modes are available on this detonator? (MOE_EDetonatorModes)
			//0 = SAFE
			//1 = ALL
			//2 = SINGLE
			detonatorModes[] = {
				0, 1, 2
			};
		};
	};


	class MOE_ComponentBase : MOE_ItemBase {};

	class MOE_ComponentTriggerBase : MOE_ComponentBase
	{
		inventorySlot[] = {
			"Trigger_MOE"
		};

		class MOE_Settings
		{
			class Light 
			{
				radius = 1;
				brightness = 1;
				colors[] = {
					1, 1, 1
				};
			};
		};
	};

	class MOE_ComponentReceiverBase : MOE_ComponentTriggerBase
	{
		class MOE_Settings : MOE_Settings
		{
			//How many sources can be connected?
			maxConnectedSignalSources = 3;

			//Can a source be connected when the parent explosive is already armed? (0 or 1)
			canBeConnectedToWhenExplosiveArmed = 1;

			class Light : Light
			{
				colors[] = {
					0, 1, 0
				};

				radius = 1;
				brightness = 1;
			};
		};
	};

	class MOE_ComponentTimerBase : MOE_ComponentTriggerBase
	{
		class MOE_Settings : MOE_Settings
		{
			//Min & max time that can be set on the timer (in seconds)
			minTimer = 15;
			maxTimer = 300;

			//Tick interval at the beginning of the timer (in seconds)
			initialTickrate = 2;

			//How much faster the tickrate becomes on each stage change
			//Example: 
			//	Initial Tickrate = 2 => ticks every 2 seconds
			//  On first stage change => ticks every 1 second
			// 	On seconds stage change => ticks every 0.5 seconds
			tickSpeedIncreaseFactor = 2;

			//Stages for the timer (when tickrate increases, i.e., beeps faster, light blinks faster & changes color)
			//Progress of the timer from 0 to 1
			// Stage 0: 0.0 - 0.5
			// Stage 1: 0.5 - 0.8
			// Stage 2: 0.8 - 1.0
			timerStages[] = {
				0.5,							
				0.8	
			};

			//Sound to use for the beeping effect
			beepingSoundset = "MOE_SoundSet_Beeping";

			class Light : Light
			{
				//Light colors for each timer stage 
				// Stage 0: Green
				// Stage 1: Yellow
				// Stage 2: Red
				colors[] = {
					0, 1, 0,
					1, 1, 0,
					1, 0, 0
				};

				radius = 3;
				brightness = 2;
			};

			//NoiseParams for A.I.
			class Noise
			{			
				strength=200;
				type="sound";	
			};			
		};
	};



	//////////////////////
	///// Explosives /////
	//////////////////////

	class MOE_Explosive_Homemade : MOE_ExplosiveBase
	{
		scope = 2; 
		displayName = "$STR_MOE_Explosive_Homemade_DisplayName";
		descriptionShort="$STR_MOE_Explosive_Homemade_Description";
		model="MoreExplosives\Data\Models\MOE_Explosive_Homemade.p3d";
		weight = 4500;
		itemSize[]={3,3};
		itemBehaviour=2;
		
		memoryPoints[] = {
			"light_timer",
			"light_receiver",
			"explosion_position"
		};

		hiddenSelections[] = {		
			"Receiver_Display",
			"Timer_Colon",
			"Timer_Sec0",
			"Timer_Sec1",
			"Timer_Min0",
			"Timer_Min1",
			"Timer_Led"
		};

		hiddenSelectionsTextures[] = {		
			"MoreExplosives\Data\Models\Textures\broken_radio_co.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\colon_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\circuitboard_homemade_led_co.paa"
		};

		hiddenSelectionsMaterials[] = {
			"dz\weapons\attachments\optics\data\tritium.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\circuitboard_homemade_led_off.rvmat"
		};
	
		class AnimationSources
		{
			class HideBody
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};

			class HideTimer
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};

			class HideWiringTimer
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};

			class HideReceiver
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};

			class HideWiringReceiver
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};

			class HideReceiverDisplay
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};
		};	

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\explosive_homemade.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\explosive_homemade.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\explosive_homemade_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\explosive_homemade_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\explosive_homemade_destruct.rvmat"
							}
						}
					};
				};
			};
		};
    };

	class MOE_Explosive_Military : MOE_ExplosiveBase
	{
		scope = 2; 
		displayName = "$STR_MOE_Explosive_Military_DisplayName";
		descriptionShort="$STR_MOE_Explosive_Military_Description";
		model="MoreExplosives\Data\Models\MOE_C4_Military.p3d";
		rotationFlags=16;
		weight = 2200;
		itemSize[]={3,2};
		itemBehaviour=1;
		ammoType = "MOE_Ammo_ExplosionMilitary";
		
		memoryPoints[] = {
			"light_trigger",			
			"explosion_position"
		};

		hiddenSelections[] = {	
			"Trigger_Led",	
			"Timer_Sec0",
			"Timer_Sec1",
			"Timer_Min0",
			"Timer_Min1",
			"Timer_Colon"
		};

		hiddenSelectionsTextures[] = {		
			"MoreExplosives\Data\Models\Textures\c4_military_co.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digit_off_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\colon_off_ca.paa"
		};

		hiddenSelectionsMaterials[] = {
			"MoreExplosives\Data\Models\Textures\c4_military.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\HomemadeExplosive\digits.rvmat",
		};

		class AnimationSources
		{
			class HideTrigger
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};

			class HideWiringTrigger
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};

			class HideTimerDisplay
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};

			class RotateTriggerAntenna
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};
		};	

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_destruct.rvmat"
							}
						}
					};
				};
			};
		};
    };
	
	

	//////////////////////
	///// Detonators /////
	//////////////////////

	class MOE_Detonator_Homemade : MOE_DetonatorBase
	{
		scope=2;
		displayName="$STR_MOE_Detonator_Homemade_DisplayName";
		descriptionShort="$STR_MOE_Detonator_Homemade_Description";
		model="MoreExplosives\Data\Models\MOE_BrokenRadio.p3d";
		weight=850;
		rotationFlags=1;
		itemSize[]={1,2};
		itemBehaviour=1;

		inventorySlot[] = {
			"WalkieTalkie"
		};
		attachments[] = {
			"BatteryD"
		};

		repairableWithKits[]={7};
		repairCosts[]={20};
		soundImpactType="plastic";

		class EnergyManager
		{
			hasIcon=1;
			autoSwitchOffWhenInCargo=1;
			energyUsagePerSecond=0.003;
			plugType=1;
			attachmentAction=1;
			updateInterval=3;
		};
		
		class AnimationSources
		{
			class HideReceiverTape
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};

			class HideReceiverDisplay
			{
				source="user";
				animPeriod=0;
				initPhase=1;
			};
		};

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};

	class MOE_Detonator_Military : MOE_DetonatorBase
	{
		scope=2;
		displayName="$STR_MOE_Detonator_Military_DisplayName";
		descriptionShort="$STR_MOE_Detonator_Military_Description";
		model="MoreExplosives\Data\Models\MOE_MilitaryDetonator.p3d";
		itemSize[]={1,2};
		rotationFlags=1;
		weight=500;
		inventorySlot[] = {
			"WalkieTalkie"
		};
		attachments[] =	{
			"BatteryD"
		};

		repairableWithKits[]={7};
		repairCosts[]={30};
		soundImpactType="plastic";

		class EnergyManager
		{
			hasIcon=1;
			autoSwitchOffWhenInCargo=1;
			energyUsagePerSecond=0.003;
			plugType=1;
			attachmentAction=1;
			updateInterval=2;
		};

		hiddenSelections[] = {		
			"Mode_Val",
			"Con_Val0",
			"Con_Val1",
			"Sel_Val0",
			"Sel_Val1",
			"Dist_Val0",
			"Dist_Val1",
			"Dist_Val2"
		};

		hiddenSelectionsTextures[] = {		
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\Mode_ALL_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\0_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\0_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\0_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\0_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\0_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\0_ca.paa",
			"MoreExplosives\Data\Models\Textures\Displays\MilitaryDetonator\0_ca.paa"

		};

		hiddenSelectionsMaterials[] = {
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat",
			"MoreExplosives\Data\Models\Textures\Displays\DisplayText.rvmat"
		};

		class AnimationSources
		{
			class HideDisplay
			{
				source="user";
				animPeriod=0;
				initPhase=1;
			};

			class HideDirection
			{
				source="user";
				animPeriod=0;
				initPhase=0;
			};

			class RotateLever
			{
				source="user";
				animPeriod=0.5;
				initPhase=1;
			};

			class RotateLatch
			{
				source="user";
				animPeriod=0.5;
				initPhase=0;
			};

			class RotateDirection
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};
		};

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\detonator_military.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\detonator_military.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\detonator_military_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\detonator_military_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\detonator_military_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};


	/////////////////////
	///// Receivers /////
	/////////////////////

	class MOE_Receiver_Homemade: MOE_ComponentReceiverBase
	{
		scope=2;
		displayName="$STR_MOE_Receiver_Homemade_DisplayName";
		descriptionShort="$STR_MOE_Receiver_Homemade_Description";
		model="MoreExplosives\Data\Models\MOE_BrokenRadio.p3d";
		weight=950;
		rotationFlags=1;
		itemSize[]={1,2};
		itemBehaviour=1;
		repairableWithKits[]={7};
        repairCosts[]={20};

		class AnimationSources
		{
			class HideReceiverTape
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};

			class HideReceiverDisplay
			{
				source="user";
				animPeriod=0;
				initPhase=1;
			};
		};

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\broken_radio_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};

	class MOE_Receiver_Military: MOE_ComponentReceiverBase
	{
		scope=2;
		displayName="$STR_MOE_Receiver_Military_DisplayName";
		descriptionShort="$STR_MOE_Receiver_Military_Description";
		model="MoreExplosives\Data\Models\MOE_C4_Receiver.p3d";
		weight=450;
		rotationFlags=16;
		itemSize[]={1,2};
		itemBehaviour=1;
		repairableWithKits[]={7};
        repairCosts[]={30};

		class MOE_Settings
		{
			maxConnectedSignalSources = 4;
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

		class AnimationSources
		{
			class RotateAntenna
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};
		};

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};


	//////////////////
	///// Timers /////
	//////////////////

	class MOE_Timer_Homemade : MOE_ComponentTimerBase
	{
		scope=2;
		displayName="$STR_MOE_Timer_Homemade_DisplayName";
		descriptionShort="$STR_MOE_Timer_Homemade_Description";
		model="MoreExplosives\Data\Models\MOE_CircuitBoard_Homemade.p3d";
		weight=690;
		rotationFlags=1;
		itemSize[]={2,2};
		repairableWithKits[]={7};
        repairCosts[]={20};

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\circuitboard_homemade.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\circuitboard_homemade.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\circuitboard_homemade_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\circuitboard_homemade_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\circuitboard_homemade_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	
	class MOE_Timer_Military : MOE_ComponentTimerBase
	{
		scope=2;
		displayName="$STR_MOE_Timer_Military_DisplayName";
		descriptionShort="$STR_MOE_Timer_Military_Description";
		model="MoreExplosives\Data\Models\MOE_C4_Receiver.p3d";
		weight=450;
		rotationFlags=16;
		itemSize[]={1,2};
		repairableWithKits[]={7};
        repairCosts[]={30};

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};



	///////////////////////////////
	///// Crafting Components /////
	///////////////////////////////

	class MOE_C4_Stick : MOE_ItemBase
	{
		scope = 2;
		displayName = "$STR_MOE_C4_Stick_DisplayName";
		descriptionShort="$STR_MOE_C4_Stick_Description";
		model="MoreExplosives\Data\Models\MOE_C4_Stick.p3d";
		weight = 750;
		itemSize[]={1,3};
		itemBehaviour=1; 
		rotationFlags=16;

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\c4_military_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};

	class MOE_Gunpowder : MOE_ItemBase
    {
        scope = 2;
		displayName = "$STR_MOE_Gunpowder_Displayname";
		descriptionShort="$STR_MOE_Gunpowder_Description";
		model="MoreExplosives\Data\Models\MOE_Gunpowder.p3d";
		weight = 180;
		stackedUnit="g";
		quantityBar=1;
		varQuantityInit=1000;
		varQuantityMin=0;
		varQuantityMax=1000;	
		varQuantityDestroyOnMin=1;
		itemSize[]={2,3};
		itemBehaviour=1; 

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_destruct.rvmat"
							}
						}
					};
				};
			};
		};
    };

	class MOE_Sulfurpowder : MOE_ItemBase
    {
        scope = 2;
		displayName = "$STR_MOE_Sulfurpowder_Displayname";
		descriptionShort="$STR_MOE_Sulfurpowder_Description";
		model="MoreExplosives\Data\Models\MOE_Sulfurpowder.p3d";
		weight = 100;
		stackedUnit="g";
		quantityBar=1;
		varQuantityInit=500;
		varQuantityMin=0;
		varQuantityMax=500;	
		varQuantityDestroyOnMin=1;
		itemSize[]={1,2};
		itemBehaviour=1; 

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_destruct.rvmat"
							}
						}
					};
				};
			};
		};
    };

	class MOE_Charcoalpowder : MOE_ItemBase
    {
        scope = 2;
		displayName = "$STR_MOE_Charcoalpowder_Displayname";
		descriptionShort="$STR_MOE_Charcoalpowder_Description";
		model="MoreExplosives\Data\Models\MOE_Charcoalpowder.p3d";
		weight = 100;
		stackedUnit="g";
		quantityBar=1;
		varQuantityInit=500;
		varQuantityMin=0;
		varQuantityMax=500;	
		itemSize[]={2,2};
		itemBehaviour=1; 

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"MoreExplosives\Data\Models\Textures\bottles_destruct.rvmat"
							}
						}
					};
				};
			};
		};
    };



	class HouseNoDestruct;
    class MOE_ExplosionObject : HouseNoDestruct
	{
		scope=2;
		model="\DZ\data\lightpoint.p3d";
	};
};

class CfgAmmo
{
	class DefaultAmmo;
	class MOE_Ammo_ExplosionBase : DefaultAmmo
	{
		hit = 0;
		indirectHit=1;

		//Explosion Radius
		indirectHitRange=1;

		indirectHitRangeMultiplier=1;
		explosive=1;
		typicalSpeed=3;
		initSpeed=3;
		simulation="shotShell";
		simulationStep=0.050000001;
		soundSetExplosion[] = {
			"Landmine_Explosion_SoundSet",
			"Landmine_Explosion_Tail_SoundSet"
		};

		//Default DayZ damage values for the vanilla damage system
		class DamageApplied
		{
			type="FragGrenade";
			class Health
			{
				damage=1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
		
		//NoiseParams for A.I.
		class NoiseExplosion
		{
			strength=300;
			type="sound";
		};	

		class MOE_Settings 
		{
			//Base damage value for custom damage calculation 
			//Final Damage Value = BaseDamage * EntityMultiplier * DamageZoneMultiplier * DistanceMultiplier
			damage = 0;
			//Radius in which to deal full damage (DistanceMultiplier = 1)
			fullDamageRange = 1;
			//Local orientation offset for the explosion particle (Angles -> Yaw, Pitch, Roll)
			//Most particles point "up" by default, this offset aligns the effect with the face-direction of the explosive
			particleOrientation = "0 -90 0";
		};
	};
};

class CfgSlots 
{
	class SLOT_TRIGGER_MOE
	{
		name="Trigger_MOE";
        displayName="$STR_MOE_Slot_Trigger_DisplayName";
		ghostIcon="set:dayz_gui image:walkie_talkie";
	};
};

class cfgSoundSets
{
	class MOE_SoundSet_Beeping
	{
		soundShaders[] =	{
			"MOE_SoundShader_Beeping"
		};
		sound3DProcessingType="character3DProcessingType";
		volumeCurve="characterAttenuationCurve";
		spatial=1;
		doppler=0;
		loop=0;
	};
};

class cfgSoundShaders
{
	class MOE_SoundShader_Beeping
	{
		samples[]=
		{		
			{
				"MoreExplosives\Data\sounds\Beeping",
				1
			}
		};
		volume=1;
		range=100;
		limitation=0;
	};
};
