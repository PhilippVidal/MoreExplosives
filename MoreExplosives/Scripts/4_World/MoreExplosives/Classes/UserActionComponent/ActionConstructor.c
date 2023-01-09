modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
		
		//=============================
		//========= Explosive =========
		//=============================
		//Arm, Disarm, Defuse
		actions.Insert(MOE_ActionManipulateExplosive);					//In world
		actions.Insert(MOE_ActionManipulateExplosiveInHands);			//In hands - unarmed 
		actions.Insert(MOE_ActionManipulateExplosiveInHandsWithTool);	//In hands - with tool

		//Placement 
		actions.Insert(MOE_ActionMountExplosive);
		actions.Insert(MOE_ActionDismountExplosive);
		actions.Insert(MOE_ActionToggleExplosivePlacement);

		//=============================
		//========= Detonator =========
		//=============================
		actions.Insert(MOE_ActionConnectReceiver);
		actions.Insert(MOE_ActionDisconnectReceiver);		
		actions.Insert(MOE_ActionTriggerSignal);
		actions.Insert(MOE_ActionCycleSelectedReceiver);
		actions.Insert(MOE_ActionCycleDetonatorMode);
		
		//=============================
		//=========== Timer ===========
		//=============================
		actions.Insert(MOE_ActionManipulateTimer);
		actions.Insert(MOE_ActionManipulateTimerInHands);
		
		//=============================
		//========= Triggers ==========
		//=============================
		actions.Insert(MOE_ActionOpenTriggerComponentUI);
		actions.Insert(MOE_ActionOpenTriggerComponentUIInHands);
		actions.Insert(MOE_ActionConvertItem);
		
    }
}