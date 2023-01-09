class MOE_Timer_Military : MOE_ComponentTimerBase 
{
    override string GetItemConversionType(int typeIdentifier = 0)
	{
		return "MOE_Receiver_Military";
	}

    override bool CanBeConvertedToItem(int typeIdentifier = 0)
	{
		return true;
	}

    override void SetActions()
	{
		super.SetActions();
		AddAction(MOE_ActionConvertItem);
	}
}