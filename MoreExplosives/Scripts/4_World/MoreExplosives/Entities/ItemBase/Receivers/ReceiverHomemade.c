class MOE_Receiver_Homemade : MOE_ComponentReceiverBase 
{
	override string GetItemConversionType(int typeIdentifier = 0)
	{
		return "MOE_Detonator_Homemade";
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