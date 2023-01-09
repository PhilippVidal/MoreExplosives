modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);		
		
		//Explosives
		pType.AddItemInHandsProfileIK(
			"MOE_Explosive_Military", 		
			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 					
			pBehavior,		
			"dz/anims/anm/player/ik/gear/DetonatorReceiver.anm");
		
		
		//Detonators
		pType.AddItemInHandsProfileIK(
			"MOE_Detonator_Homemade", 	
			"dz/anims/workspaces/player/player_main/props/player_main_1h_GPSReciever.asi", 	
			pBehavior, 		
			"dz/anims/anm/player/ik/gear/GPSReciever.anm");
		
		pType.AddItemInHandsProfileIK(
			"MOE_Detonator_Military", 		
			"dz/anims/workspaces/player/player_main/props/player_main_1h_GPSReciever.asi", 	
			pBehavior, 		
			"dz/anims/anm/player/ik/gear/GPSReciever.anm");
		
		
		//Receivers
		pType.AddItemInHandsProfileIK(
			"MOE_Receiver_Homemade", 	
			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 
			pBehavior,		
			"dz/anims/anm/player/ik/gear/DetonatorReceiver.anm");
		
		pType.AddItemInHandsProfileIK(
			"MOE_Receiver_Military", 
			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 	
			pBehavior,		
			"dz/anims/anm/player/ik/gear/DetonatorReceiver.anm");
		
		
		//Timers
		pType.AddItemInHandsProfileIK(
			"MOE_Timer_Homemade", 		
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",					
			pBehavior, 		
			"dz/anims/anm/player/ik/gear/paper.anm");

		pType.AddItemInHandsProfileIK(
			"MOE_Timer_Military", 	
			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 	
			pBehavior,		
			"dz/anims/anm/player/ik/gear/DetonatorReceiver.anm");
		
		
		//Crafting Components
		pType.AddItemInHandsProfileIK(
			"MOE_Sulfurpowder", 	
			"dz/anims/workspaces/player/player_main/props/player_main_1h_pills.asi",	
			pBehavior,	
			"dz/anims/anm/player/ik/gear/VitaminBottle.anm");
		
		pType.AddItemInHandsProfileIK(
			"MOE_Charcoalpowder", 
			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 				
			pBehavior, 	
			"dz/anims/anm/player/ik/gear/food_can.anm");
		
		pType.AddItemInHandsProfileIK(
			"MOE_Gunpowder", 		
			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 
			pBehavior,								
			"dz/anims/anm/player/ik/gear/marmalade.anm" );

		pType.AddItemInHandsProfileIK(
			"MOE_C4_Stick", 		
			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 					
			pBehavior,		
			"dz/anims/anm/player/ik/gear/DetonatorReceiver.anm");
		
		
	}

	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterTwoHanded(pType, pBehavior);	
			
		//Explosives
		pType.AddItemInHandsProfileIK(
			"MOE_Explosive_Homemade", 	
			"dz/anims/workspaces/player/player_main/player_main_2h.asi", 
			pBehavior,	
			"dz/anims/anm/player/ik/vehicles/BatteryTruck.anm"); 
	}
}