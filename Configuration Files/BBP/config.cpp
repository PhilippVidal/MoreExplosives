class CfgPatches
{
    class CustomMOEConfig_BBP
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[] = {
			"MoreExplosives_Config",
            "BBP_Vanilla_Dmg"
        };
    };
};

////////////////////////////////
///// Object Configuration /////
////////////////////////////////

class CfgVehicles
{
    class Inventory_Base;
	class BBP_WALL_BASE: Inventory_Base
	{
		class MOE_Settings
		{
			class DamageMultipliers
			{
				MOE_Explosive_Homemade = 1;
				MOE_Explosive_Military = 1;
			};

			damageZonePriorities[] = {
				"t3_door",
				"t2_door",
				"t1_door",
				"window_bars",
				"t3_wall_concrete_bricks_2",
				"t3_wall_concrete_bricks",
				"t2_metal_2",
				"t2_metal",
				"t1_wall_logs_horizontal",
				"t1_wall_planks"
			};
		};

		class DamageSystem
		{
			class DamageZones
			{
				class BBP_Tier_1
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 11;
							MOE_Explosive_Military = 11;
						};
					};
				};
				class BBP_Tier_2
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 5.5;
							//slightly less damage on military explosive so it does not destroy the wall with a single explosion
							MOE_Explosive_Military = 3.5;
						};
					};
				};
				class BBP_Tier_3
				{
					class MOE_Settings
					{
						class DamageMultipliers
						{
							MOE_Explosive_Homemade = 9.4;
							MOE_Explosive_Military = 9.4;
						};
					};
				};	
				class t1_wall_logs_horizontal: BBP_Tier_1
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"t2_metal"
						};
					};
				};
				class t1_wall_planks: BBP_Tier_1
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"t2_metal_2"
						};
					};
				};
				class t1_door: BBP_Tier_1
				{
					class MOE_Settings : MOE_Settings
					{
						requiredDestroyedParts[]={
							"t2_door"
						};
					};
				};
			};
		};
	};

};

