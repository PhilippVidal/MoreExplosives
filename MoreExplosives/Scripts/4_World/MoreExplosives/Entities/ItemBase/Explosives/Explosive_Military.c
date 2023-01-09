class MOE_Explosive_Military : MOE_ExplosiveBase
{
	//Hide & Rotate Animations
	protected const string HIDE_TRIGGER				= "HideTrigger";
	protected const string HIDE_TRIGGER_WIRING 		= "HideWiringTrigger";
	protected const string ROTATE_TRIGGER_ANTENNA 	= "RotateTriggerAntenna";	

	//Selections, Materials and Textures
	protected const string SELECTION_TRIGGER_LED 	= "Trigger_Led";
	protected const string SELECTION_TRIGGER_COLON	= "Timer_Colon";
	protected const ref array<string> SELECTION_TRIGGER_SEC = {
		"Timer_Sec0",
		"Timer_Sec1"
	};
	
	protected const ref array<string> SELECTION_TRIGGER_MIN = {
		"Timer_Min0",
		"Timer_Min1"
	};
	
	//LED 
	protected const string MATERIAL_LED_ON 	= "MoreExplosives\\Data\\Models\\Textures\\led_on.rvmat";
	protected const string TEXTURE_LED_OFF 	= "MoreExplosives\\Data\\Models\\Textures\\c4_military_co.paa";
	protected const string MATERIAL_LED_OFF 	= "MoreExplosives\\Data\\Models\\Textures\\c4_military.rvmat";

	override void UpdateTimerVisuals(notnull MOE_ComponentTimerBase timer, int timerValueOverride = -1)
	{
		int remainingTime;
		if(timerValueOverride < 0)
		{		
			remainingTime = (int)timer.GetRemainingTime();
		}
		else 
		{
			remainingTime = timerValueOverride;
		}

		SetDigitsOnSelections( SELECTION_TRIGGER_SEC, HDSN_MiscFunctions.GetSeconds(remainingTime), 59 );
		SetDigitsOnSelections( SELECTION_TRIGGER_MIN, HDSN_MiscFunctions.GetMinutes(remainingTime) );
		
		int selectionIdx = GetHiddenSelectionIndex(SELECTION_TRIGGER_COLON);
		SetObjectTexture(selectionIdx,  "MoreExplosives\\Data\\Models\\Textures\\Displays\\HomemadeExplosive\\colon_ca.paa");
	}
	
	override void UpdateReceiverVisuals(notnull MOE_ComponentReceiverBase receiver)
	{
		SetDigitsToOff(SELECTION_TRIGGER_SEC);
		SetDigitsToOff(SELECTION_TRIGGER_MIN);	
		
		int selectionIdx = GetHiddenSelectionIndex(SELECTION_TRIGGER_COLON);
		SetObjectTexture(selectionIdx,  "MoreExplosives\\Data\\Models\\Textures\\Displays\\HomemadeExplosive\\colon_off_ca.paa");
	}
	
	override void OnTriggerLightTurnedOn()
	{
		super.OnTriggerLightTurnedOn();
		//SetLedState(true);
	}
	
	override void OnTriggerLightTurnedOff()
	{
		super.OnTriggerLightTurnedOff();
		//SetLedState(false);
	}
	
	override string GetLightMemoryPoint()
	{
		return "light_trigger";
	}
	
	override void OnTriggerAttached(EntityAI entity)
	{
		super.OnTriggerAttached(entity);
		UpdateTriggerVisuals(MOE_ComponentTriggerBase.Cast(entity));
	}
	
	override void SetLedState(bool state)
	{				
		int selectionIdx = GetHiddenSelectionIndex(SELECTION_TRIGGER_LED);
		if(selectionIdx == -1)
		{
			return;
		}
		
		if(state)
		{
			SetObjectTexture(selectionIdx, GetLedColorTexture());
			SetObjectMaterial(selectionIdx, MATERIAL_LED_ON);
			return;
		}
		
		SetObjectTexture(selectionIdx, TEXTURE_LED_OFF);
		SetObjectMaterial(selectionIdx, MATERIAL_LED_OFF);	
	}
	
    override void UpdateAttachmentVisuals()
	{		
		super.UpdateAttachmentVisuals();
		if(!ValidateTrigger())
		{
			SetAnimationPhase(HIDE_TRIGGER, 1);
			SetAnimationPhase(HIDE_TRIGGER_WIRING, 1);
			return;
		}
		
		SetAnimationPhase(HIDE_TRIGGER, 0);
		SetAnimationPhase(HIDE_TRIGGER_WIRING, !IsArmed());
		SetAnimationPhaseNow(ROTATE_TRIGGER_ANTENNA, m_Trigger.IsReceiver());	
	}
	
	override vector GetPlacementPositionOffset()
	{
		return "0 0 -0.016";
	}
	
	//Yaw, Pitch, Roll to apply before placement rotation
	override vector GetPlacementRotationOffset()
	{
		return "180 0 90";
	}
}