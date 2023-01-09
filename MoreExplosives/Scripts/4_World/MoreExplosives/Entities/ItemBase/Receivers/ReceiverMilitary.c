class MOE_Receiver_Military : MOE_ComponentReceiverBase
{
    override string GetItemConversionType(int typeIdentifier = 0)
	{
		return "MOE_Timer_Military";
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