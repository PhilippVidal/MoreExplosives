class MOE_Detonator_Military : MOE_DetonatorBase
{

	protected const string HIDE_DISPLAY		= "HideDisplay";
	protected const string HIDE_DIRECTION 	= "HideDirection";
	protected const string ROTATE_DIRECTION = "RotateDirection";
 
	protected const string ROTATE_LEVER 	= "RotateLever";
	protected const string ROTATE_LATCH 	= "RotateLatch";

	protected const string SELECTION_DIRECTION 	= "Dir_Arrow";
	protected const string SELECTION_MODE 		= "Mode_Val";
	protected const ref array<string> SELECTION_CONNECTIONS = {
		"Con_Val0",
		"Con_Val1"
	};
	
	protected const ref array<string> SELECTION_SELECTED = {
		"Sel_Val0",
		"Sel_Val1"
	};
	
	protected const ref array<string> SELECTION_DISTANCE = {
		"Dist_Val0",
		"Dist_Val1",
		"Dist_Val2"
	};
	
	protected static const string TEXTURE_DIGIT_FORMAT = "MoreExplosives\\Data\\Models\\Textures\\Displays\\MilitaryDetonator\\%1_ca.paa";
	
	protected const ref array<string> TEXTURE_DETONATORMODE = {
		string.Format(TEXTURE_DIGIT_FORMAT, "Mode_SAFE"),
		string.Format(TEXTURE_DIGIT_FORMAT, "Mode_ALL"),
		string.Format(TEXTURE_DIGIT_FORMAT, "Mode_SINGLE")
	};
	
	protected const string TEXTURE_MINUS = string.Format(TEXTURE_DIGIT_FORMAT, "Minus");
	
	override void OnWorkStart()
	{
		super.OnWorkStart();
		if(GetGame().IsServer())
		{
			UpdateDisplayState(true);
			UpdateLeverLatchState(true);			
		}	
	}
	
	override void OnWorkStop()
	{		
		super.OnWorkStop();
		if(GetGame().IsServer())
		{
			UpdateDisplayState(false);
			UpdateLeverLatchState(false);	
		}			
	}
	
	protected void UpdateDisplayState(bool isOn)
	{
		SetAnimationPhase(HIDE_DISPLAY, !isOn);
	}
	
	override bool ReadObjectUpdateData(ParamsReadContext ctx, bool isFullSync)
	{
		bool value = super.ReadObjectUpdateData(ctx, isFullSync);		
		UpdateDisplayedConnections();
		UpdateDisplayedSelectedReceiver();
		return value;
	}
	
	override void OnDetonatorModeSwitched(int mode)
	{
		super.OnDetonatorModeSwitched(mode);
		UpdateDirectionDisplay();
	}
	
	protected void UpdateDirectionDisplay()
	{
		if(GetSelectedDetonatorMode() != MOE_EDetonatorModes.SINGLE)
		{
			SetAnimationPhase(HIDE_DIRECTION, 1);		
			return;
		}
		
		SetAnimationPhase(HIDE_DIRECTION, 0);
	}
	
	protected void UpdateLeverLatchState(bool isOn)
	{
		SetAnimationPhase(ROTATE_LEVER, !isOn);
		SetAnimationPhase(ROTATE_LATCH, isOn);
	}
	
	override void PlayTriggerAnimation()
	{
		SetAnimationPhase(ROTATE_LEVER, 1);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ResetTriggerAnimation, 700);
	}
	
	protected void ResetTriggerAnimation()
	{
		SetAnimationPhase(ROTATE_LEVER, 0);
	}
	
	override bool IsTurnedOn()
	{
		return super.IsTurnedOn() && GetCompEM() && GetCompEM().IsWorking();
	}
	
	
	
	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);
		if(GetGame().IsClient())
		{
			UpdateDisplayedDirection();
			UpdateDisplayedDistance();	
			UpdateDisplayedConnections();
		}
	}
	
	//Copied from GPSReceiver
	override void OnMovedInsideCargo(EntityAI container);
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}	
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		UpdateDisplayedDetonatorMode();
		UpdateDisplayedSelectedReceiver();
	}

	protected void UpdateDisplayedDirection()
	{
		vector forwardDirection = vector.Forward;
		EntityAI rootEntity = GetHierarchyRoot();
		if(rootEntity)
		{
			PlayerBase player;
			if(CastTo(player, rootEntity))
			{
				forwardDirection = MiscGameplayFunctions.GetHeadingVector(player);
			}
			else if(rootEntity == this)
			{
				forwardDirection = GetDirectionUp();
			}
			else
			{
				forwardDirection = rootEntity.GetDirection();
			}		
		}

		float angle = GetAngleToReceiver(GetSelectedReceiverIndex(), forwardDirection);
		if(angle < 0)
		{
			angle = 360 + angle;
		}
		
		SetAnimationPhaseNow(ROTATE_DIRECTION, angle / 360);
	}
	
	protected void UpdateDisplayedDistance()
	{	
		if(GetSelectedDetonatorMode() != MOE_EDetonatorModes.SINGLE)
		{			
			SetMinusOnSelections(SELECTION_DISTANCE);
			return;
		}
		
		int distance = GetDistanceToReceiver(GetSelectedReceiverIndex());
		if(distance < 0)
		{
			SetMinusOnSelections(SELECTION_DISTANCE);
			return;
		}
		
		distance -= (distance % 10);		
		SetDigitsOnSelections(SELECTION_DISTANCE, TEXTURE_DIGIT_FORMAT, distance, 999);	
	}
	
	protected void UpdateDisplayedConnections()
	{
		SetDigitsOnSelections(SELECTION_CONNECTIONS, TEXTURE_DIGIT_FORMAT, GetConnectionCount());		
	}
	
	protected void UpdateDisplayedSelectedReceiver()
	{
		if( (GetSelectedDetonatorMode() != MOE_EDetonatorModes.SINGLE) || GetConnectionCount() == 0)
		{	
			SetMinusOnSelections(SELECTION_SELECTED);		
			return;
		}
		
		SetDigitsOnSelections(SELECTION_SELECTED, TEXTURE_DIGIT_FORMAT, GetSelectedReceiverIndex() + 1);		
	}
	
	protected void UpdateDisplayedDetonatorMode()
	{
		SetObjectTexture( GetHiddenSelectionIndex(SELECTION_MODE),  TEXTURE_DETONATORMODE[Math.Clamp(GetSelectedDetonatorMode(), 0, TEXTURE_DETONATORMODE.Count() - 1)] );
	}

	
	protected void SetDigitsOnSelections(array<string> selections, string digitPathFormat, int value, int max = 99)
	{
		int selectionCount = selections.Count();
		if(!selections || selectionCount == 0)
		{
			return;
		}
		
		//automatic max calculation if max < 0
		if(max < 0)
		{
			max = Math.Pow(10, selectionCount) - 1;
		}
		
		array<int> digits = HDSN_MiscFunctions.GetDigitsArray(Math.Clamp(value, 0, max));
		
		//Fill with 0s
		int difference = selectionCount - digits.Count();
		for(int i = 0; i < difference; i++)
		{
			digits.Insert(0);
		}
		
		for(int idx = 0; idx < selectionCount; idx++)
		{
			SetObjectTexture(GetHiddenSelectionIndex(selections[idx]),  string.Format( digitPathFormat, digits[idx]) );
		}
	}
	
	protected void SetMinusOnSelections(array<string> selections)
	{
		for(int i = 0; i < selections.Count(); i++)
		{
			SetObjectTexture( GetHiddenSelectionIndex( selections[i] ),  TEXTURE_MINUS);
		}
	}
}