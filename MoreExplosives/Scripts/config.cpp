class CfgPatches
{
    class MoreExplosives_Scripts
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]=
        {
			"MoreExplosives_Data",
			"MoreExplosives_GUI"
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
		
		dependencies[] = { 
			"Game", 
			"World", 
			"Mission" 
		};
		
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {
					"MoreExplosives/Scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {
					"MoreExplosives/Scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {
					"MoreExplosives/Scripts/5_Mission"
				};
			};
		};
	};
};
