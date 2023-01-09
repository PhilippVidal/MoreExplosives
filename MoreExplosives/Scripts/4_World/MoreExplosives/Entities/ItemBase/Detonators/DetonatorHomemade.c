class MOE_Detonator_Homemade : MOE_DetonatorBase
{
	protected const string HIDE_DISPLAY	= "HideReceiverDisplay";
	
	override string GetItemConversionType(int typeIdentifier = 0)
	{
		return "MOE_Receiver_Homemade";
	}

    override bool CanBeConvertedToItem(int typeIdentifier = 0)
	{
		return true;
	}
	
	protected void UpdateDisplayState(bool isOn)
	{
		SetAnimationPhase(HIDE_DISPLAY, !isOn);
	}

	override void OnWorkStart()
	{
		UpdateDisplayState(true);
	}
	
	override void OnWorkStop()
	{		
		UpdateDisplayState(false);
	}
	
	override bool IsTurnedOn()
	{
		return super.IsTurnedOn() && GetCompEM() && GetCompEM().IsWorking();
	}

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
		AddAction(MOE_ActionConvertItem);
	}	
	
	//Copied from GPSReceiver
	override void OnMovedInsideCargo(EntityAI container);
}