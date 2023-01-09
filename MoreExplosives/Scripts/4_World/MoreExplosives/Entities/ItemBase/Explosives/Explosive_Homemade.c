class MOE_Explosive_Homemade : MOE_ExplosiveBase
{
	//Hide Animations
	protected const string HIDE_TIMER			= "HideTimer";
	protected const string HIDE_TIMER_WIRING 	= "HideWiringTimer";
	
	protected const string HIDE_RECEIVER 			= "HideReceiver";
	protected const string HIDE_RECEIVER_WIRING 	= "HideWiringReceiver"; 
	protected const string HIDE_RECEIVER_DISPLAY 	= "HideReceiverDisplay";

	//Selections, Materials and Textures
	protected const string SELECTION_TIMER_LED 			= "Timer_Led";
	protected const string SELECTION_RECEIVER_DISPLAY 	= "Receiver_Display";
	protected const string SELECTION_TIMER_COLON		= "Timer_Colon";
	protected const ref array<string> SELECTION_TIMER_SEC = {
		"Timer_Sec0",
		"Timer_Sec1"
	};
	
	protected const ref array<string> SELECTION_TIMER_MIN = {
		"Timer_Min0",
		"Timer_Min1"
	};

	//LED 
	protected const string MATERIAL_TIMER_LED_ON 	= "MoreExplosives\\Data\\Models\\Textures\\led_on.rvmat";
	protected const string TEXTURE_TIMER_LED_OFF 	= "MoreExplosives\\Data\\Models\\Textures\\circuitboard_homemade_led_co.paa";
	protected const string MATERIAL_TIMER_LED_OFF 	= "MoreExplosives\\Data\\Models\\Textures\\circuitboard_homemade_led_off.rvmat";
	
	protected const string MATERIAL_RECEIVER_DISPLAY_ON 	= "dz\weapons\attachments\optics\data\tritium.rvmat";
	protected const string TEXTURE_RECEIVER_DISPLAY_OFF 	= "MoreExplosives\\Data\\Models\\Textures\\broken_radio_co.paa";
	protected const string MATERIAL_RECEIVER_DISPLAY_OFF	= "MoreExplosives\\Data\\Models\\Textures\\broken_radio.rvmat";
	
	override void SetLedState(bool state)
	{			
		if(HasTimer())
		{
			SetLetStateOnTimer(state);
			return;
		}
		
		if(HasReceiver())
		{
			SetLetStateOnReceiver(state);
			return;
		}
	}
	
	void SetLetStateOnTimer(bool state)
	{
		int selectionIdx = GetHiddenSelectionIndex(SELECTION_TIMER_LED);	
		if(selectionIdx == -1)
		{
			return;
		}
		
		if(state)
		{
			SetObjectTexture(selectionIdx, GetLedColorTexture());
			SetObjectMaterial(selectionIdx, MATERIAL_TIMER_LED_ON);
			return;
		}
		
		SetObjectTexture(selectionIdx, TEXTURE_TIMER_LED_OFF);
		SetObjectMaterial(selectionIdx, MATERIAL_TIMER_LED_OFF);	
	}
	
	
	void SetLetStateOnReceiver(bool state)
	{
		int selectionIdx = GetHiddenSelectionIndex(SELECTION_RECEIVER_DISPLAY);	
		if(selectionIdx == -1)
		{
			return;
		}
		
		if(state)
		{
			SetObjectTexture(selectionIdx, GetLedColorTexture());
			SetObjectMaterial(selectionIdx, MATERIAL_RECEIVER_DISPLAY_ON);
			return;
		}
		
		SetObjectTexture(selectionIdx, TEXTURE_RECEIVER_DISPLAY_OFF);
		SetObjectMaterial(selectionIdx, MATERIAL_RECEIVER_DISPLAY_OFF);	
	}

	override string GetLightMemoryPoint()
	{
		if(HasTimer())
		{
			return "light_timer";
		}
		
		if(HasReceiver())
		{
			return "light_receiver";
		}
		
		return "";
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

		SetDigitsOnSelections( SELECTION_TIMER_SEC, HDSN_MiscFunctions.GetSeconds(remainingTime), 59 );
		SetDigitsOnSelections( SELECTION_TIMER_MIN, HDSN_MiscFunctions.GetMinutes(remainingTime) );
	}
	
	override void UpdateAttachmentVisuals()
	{		
		super.UpdateAttachmentVisuals();
		
		if(!ValidateTrigger())
		{
			SetAnimationPhase(HIDE_TIMER_WIRING, 1);
			SetAnimationPhase(HIDE_TIMER, 1);
			SetAnimationPhase(HIDE_RECEIVER_WIRING, 1);
			SetAnimationPhase(HIDE_RECEIVER, 1);			
			SetAnimationPhase(HIDE_RECEIVER_DISPLAY, 1);
			return;
		}
		
		if(m_Trigger.IsReceiver())
		{
			
			SetAnimationPhase(HIDE_RECEIVER, 0);
			SetAnimationPhase(HIDE_RECEIVER_WIRING, !IsArmed());
			SetAnimationPhase(HIDE_RECEIVER_DISPLAY, !IsArmed());
			return;
		}
		
		if(m_Trigger.IsTimer())
		{
			SetAnimationPhase(HIDE_TIMER, 0);
			SetAnimationPhase(HIDE_TIMER_WIRING, !IsArmed());
			return;
		}
	}
	
	override vector GetPlacementPositionOffset()
	{
		return "0 0 0";
	}
	
	//Yaw, Pitch, Roll to apply before placement rotation
	override vector GetPlacementRotationOffset()
	{
		return "0 90 0";
	}
}