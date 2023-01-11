//Base class for all timers that can interact with explosives
//Triggers the assigned explosive when timer runs out 
class MOE_ComponentTimerBase : MOE_ComponentTriggerBase 
{		
	//Main Timer for both server & client 
	protected ref MOE_Timer m_Timer;	
	//Timer for ticking on client -> e.g., determines when to play audio
	protected ref Timer m_TickTimer;	
	//Timer to prevent spamming of RPCs to the server 
	protected ref Timer m_SpamTimer;	
	
	protected bool m_IsRunning;
	protected bool m_ClientCanSendTimer;
	
	protected int m_CurrentStage;
	protected int m_SelectedTime;
	
	protected float m_CurrentTickrate;
	protected float m_RemainingTime, m_PrevRemainingTime;

	protected EffectSound m_BeepingSound;
	 
	
	void ~MOE_ComponentTimerBase()
	{
		DeleteTimers();
		DeleteBeepingSound();
	}
	
	override void Init_MOE()
	{	
		m_SelectedTime = GetMinTimer();
		RegisterNetSyncVariableBool("m_IsRunning");
		RegisterNetSyncVariableInt("m_SelectedTime");
		RegisterNetSyncVariableFloat("m_RemainingTime");	
		m_ClientCanSendTimer = true;
	}
	
	float GetCurrentTickrate()
	{
		return m_CurrentTickrate;
	}
	
	void StartTimer(PlayerBase player = null)
	{		
		
#ifdef SERVER				
			SetInteractingPlayerOnExplosive(player);
			StartTimerOnServer();
#else
			StartTimerOnClient();
#endif
		
		OnTimerStarted();	
	}
	
	protected void StartTimerOnClient()
	{
		if(!m_Timer) { m_Timer = new MOE_Timer(); }
		if(!m_TickTimer) { m_TickTimer = new Timer(); }
		
		//start main timer
		m_Timer.Run(m_RemainingTime, this, "");
		
		//start tick loop for audio	& light	
		m_CurrentStage = GetCurrentStage();
		m_CurrentTickrate = Math.Clamp( (GetInitialTickrate() / Math.Pow(GetTickSpeedIncreaseFactor(), m_CurrentStage)), MOE_Constants.TIMER_MIN_TICKRATE, float.MAX );
		
		m_TickTimer.Run(m_CurrentTickrate, this, "UpdateTickTimer", null, true);	
	}
	
	protected void StartTimerOnServer()
	{
		if(!m_Timer) m_Timer = new MOE_Timer();
		if(!m_TickTimer) m_TickTimer = new Timer();
		
		ResetTimer();
		m_CurrentTickrate = 1.0;
		
		m_Timer.Run(m_RemainingTime, this, "StopTimer");
		m_TickTimer.Run(m_CurrentTickrate, this, "UpdateTickTimer", null, true);
		SetIsRunning(true);	
	}
	
	void StopTimer(PlayerBase player = null) 
	{	
		if(m_Timer) { m_Timer.Stop(); }		
		if(m_TickTimer) { m_TickTimer.Stop(); }	
		
#ifdef SERVER
			SetInteractingPlayerOnExplosive(player);
			SetIsRunning(false);				
#endif
		
		OnTimerStopped();
		
		if(m_RemainingTime <= 0.0)
		{
			OnTimerEnded();
		}
	}
	
	void ResetTimer()
	{
		if(m_Timer && m_Timer.IsRunning())
		{
			StopTimer();
		}
		
		m_RemainingTime = m_SelectedTime;
		SetSynchDirty();
	}
	
	protected void UpdateTickTimer()
	{
#ifdef SERVER 
		NoiseSystem noiseSystem = GetGame().GetNoiseSystem();
		if(noiseSystem)
		{
			noiseSystem.AddNoiseTarget(GetPosition(), m_CurrentTickrate, GetNoiseParams());
		}
#else 
		int newStage = GetCurrentStage();		
		if(m_CurrentStage != newStage)
		{
			OnStageChanged(m_CurrentStage, newStage);
			m_CurrentStage = newStage;
		}
		
		OnTimerTick();
#endif			
	}
	
	int GetCurrentStage()
	{	
		float progress = GetProgress01();
		int count = GetTimerStages().Count();
		for(int stage = 0; stage <= count; stage++)
		{
			if(progress < GetStageValue(stage))
			{
				return stage;
			}
		}
		
		return count + 1;
	}
	
	float GetStageValue(int stage)
	{
		array<float> stages = GetTimerStages();
		if(stages.Count() < stage)
		{
			return 1.0;
		}
		
		if(stage < 0)
		{
			return 0.0;
		}
		
		return stages[stage];
	}
	
	
	float GetRemainingTime()
	{
		return m_RemainingTime; 
	}
	
	void SetRemainingTime(float time, bool fromTimer = false)
	{
		if(!fromTimer)
		{
			m_RemainingTime = Math.Clamp(time, 0.0, m_SelectedTime);	
			SetSynchDirty();
			return;
		}
			
		m_RemainingTime = time;	
		
#ifndef SERVER
		if(m_RemainingTime != m_PrevRemainingTime)
		{
			int newSecs = GetSeconds(m_RemainingTime);
			int lastSecs = GetSeconds(m_PrevRemainingTime);
			
			if(newSecs != lastSecs)
			{
				OnTimerSecondPassed(newSecs);				
			}
			
			int newMins = GetMinutes(m_RemainingTime);
			int lastMins = GetMinutes(m_PrevRemainingTime);
			
			if(newMins != lastMins)
			{
				OnTimerMinutePassed(newMins);
			}
		}
		
		m_PrevRemainingTime = m_RemainingTime;		
#endif		
	}
	
	void SetTimerOnClient(int minutes, int seconds)
	{
		SetTimerOnClient(minutes * 60 + seconds);
	}
	
	void SetTimerOnClient(int seconds)
	{
		if(!CanSendTimerUpdates())
		{
			return;
		}
		
		if(!m_SpamTimer)
		{
			m_SpamTimer = new Timer();
		}
		
		if(m_SpamTimer.IsRunning())
		{
			return;
		}
			
		SendSetTimerToServer(seconds);	
		BlockTimerUpdatesFromClient();
		m_SpamTimer.Run(MOE_Constants.TIMER_SPAM_DELAY, this, "UnblockTimerUpdatesFromClient");
	}
	
	
	void SendSetTimerToServer(int seconds)
	{	
		RPCSingleParam(MOE_ERPCTypes.SYNC, new Param1<int>(seconds), true);
	}
	
	protected void BlockTimerUpdatesFromClient()
	{
		m_ClientCanSendTimer = false;
	}
	
	protected void UnblockTimerUpdatesFromClient()
	{
		m_ClientCanSendTimer = true;
	}
	
	int GetSelectedMinutes()
	{
		return GetMinutes(m_SelectedTime);
	}
	
	int GetSelectedSeconds()
	{
		return GetSeconds(m_SelectedTime);
	}
	
	int GetRemainingMinutes()
	{
		return GetMinutes(m_RemainingTime);
	}
	
	int GetRemainingSeconds()
	{
		return GetSeconds(m_RemainingTime);
	}
	
	void SetSelectedTime(float timeInSeconds)
	{
		m_SelectedTime =  Math.Clamp(timeInSeconds, GetMinTimer(), GetMaxTimer());	
		SetSynchDirty();	
	}
		
	int GetSelectedTime()
	{
		return m_SelectedTime;
	}
	
	vector GetStageColor()
	{
		//Get color index
		array<vector> lightColors = GetLightColors();
		if(!lightColors || lightColors.Count() == 0)
		{
			return "0 0 1";
		}
	
		array<float> stages = GetTimerStages();		
		int stageCount = stages.Count() + 1;
		int colorCount = lightColors.Count();
		int difference = stageCount - colorCount;
		
		int idx = Math.Clamp(GetCurrentStage() - difference, 0, colorCount - 1);
		if(idx < 0)
		{
			return "0 1 1";
		}
		
		return lightColors[idx];
	}
	
	protected void SetIsRunning(bool value)
	{
		m_IsRunning = value;
		SetSynchDirty();
	}
	
	protected void DeleteBeepingSound()
	{	
		if(m_BeepingSound)
		{		
			SEffectManager.DestroyEffect(m_BeepingSound);
		}
	}
	
	protected void DeleteTimers()
	{
		if(m_Timer)
		{
			m_Timer.Stop();
			m_Timer = null;
		}
		
		if(m_TickTimer)
		{
			m_TickTimer.Stop();
			m_TickTimer = null;
		}
	}
	
	override void OpenUI()
	{
		//MOE_TimerWidget.GetInstance().Show();
		//MOE_TimerWidget.GetInstance().SetParentTimer(this);
		MOE_TimerWidget timerWidget = MOE_TimerWidget.Cast(GetGame().GetUIManager().EnterScriptedMenu(MOE_Constants.MENU_TIMER_UI_ID, null));
		if(timerWidget)
		{
			timerWidget.SetParentTimer(this);
		}
	}
	
	override void Deactivate(EntityAI usedTool = null)
	{
#ifdef SERVER
			StopTimer();
#endif
		super.Deactivate(usedTool);
	}
	
	override vector GetLightColor()
	{
		return GetStageColor();
	}
	
	//========================================
	//============= "Events" =================
	//========================================
	
	void OnTimerStarted()
	{			
#ifndef SERVER		
		EntityAI soundParent = this;
		vector soundPositionLocal;		
		if(ValidateConnectedExplosive())
		{
			soundParent = m_ConnectedExplosive;
			soundPositionLocal = m_ConnectedExplosive.GetTriggerPosition_Local();
			m_ConnectedExplosive.UpdateTimerVisuals(this);		
			if(HasLight())
			{
				MOE_TriggerLight light = m_ConnectedExplosive.CreateTriggerLight(this);
				if(light)
				{		
					light.TurnOnForSeconds(m_CurrentTickrate * GetLightOnDurationWeight());
				}
			}				
		}
			
		string beepingSoundSet = GetBeepingSoundSet();
		if(beepingSoundSet != "")
		{
			m_BeepingSound = SEffectManager.CreateSound(beepingSoundSet, soundParent.GetPosition(), 0, 0, false);
			m_BeepingSound.SetParent(soundParent);
			m_BeepingSound.SetLocalPosition(soundPositionLocal);
			m_BeepingSound.SoundPlay();
		}		
#endif				
	}

	void OnTimerStopped()
	{		
#ifndef SERVER				
		if(ValidateConnectedExplosive())
		{
			m_ConnectedExplosive.DestroyTriggerLight();	
		}
					
		DeleteBeepingSound();
#endif		
	}
	
	void OnTimerEnded()
	{		
#ifndef SERVER	
			if(ValidateConnectedExplosive())
			{
				m_ConnectedExplosive.UpdateTimerVisuals(this, 0);
			}
#endif	
		TriggerExplosive();
	}
	
	void OnTimerTick()
	{
#ifndef SERVER		
			if(HasLight() && ValidateConnectedExplosive())
			{				
				MOE_TriggerLight light = m_ConnectedExplosive.GetTriggerLight();
				if(light)
				{
					light.SetColorToDisplay(GetLightColor());
					light.TurnOnForSeconds(m_CurrentTickrate * GetLightOnDurationWeight());			
				}
			}
			
			if(m_BeepingSound)
			{			
				m_BeepingSound.SoundStop();
				m_BeepingSound.SoundPlay();
			}	
#endif	
	}
	
	float GetLightOnDurationWeight()
	{
		return Math.Lerp(0.5, 0.85, GetProgress01());
	}

	void OnStageChanged(int prevStage, int newStage)
	{
		//Timer is at end 
		if(newStage > GetTimerStages().Count())
		{
			m_TickTimer.Stop();
		}
				
		m_CurrentTickrate = GetInitialTickrate() / Math.Pow(GetTickSpeedIncreaseFactor(), newStage);
		
		//Clamp to prevent audio issues 
		m_CurrentTickrate = Math.Clamp(m_CurrentTickrate, MOE_Constants.TIMER_MIN_TICKRATE, float.MAX);
		m_TickTimer.Run(m_CurrentTickrate, this, "UpdateTickTimer", null, true);
	}
	
	void OnTimerSecondPassed(int newSeconds)
	{
#ifndef SERVER	
		if(ValidateConnectedExplosive())
		{
			m_ConnectedExplosive.UpdateTriggerVisuals(this);
		}		
#endif
	}
	
	void OnTimerMinutePassed(int newMinutes)
	{
		
	}
	
	//========================================
	//========== Misc. Functions =============
	//========================================
	
	int GetMinutes(int timeInSeconds)
	{
		return Math.Floor(timeInSeconds / 60);
	}
	
	int GetSeconds(int timeInSeconds)
	{
		return timeInSeconds % 60;
	}
	
	//0 -> full time left, 1 -> timer finished
	float GetProgress01()
	{
		return 1 - (m_RemainingTime / m_SelectedTime);
	}
	
	//Returns given time as fancy string with given length (e.g., 032:02 -> filled up with 0s)
	string GetTimeAsFormattedString(int timeInSeconds, int minutesDigitCount = 2)
	{
		int minutes = Math.Floor((float)timeInSeconds / 60.0);
		int seconds = timeInSeconds % 60;
		
		string minutesStr = minutes.ToString();
		string secondsStr = seconds.ToString();
		
		int i;
		int difference = minutesDigitCount - minutesStr.Length();
		for(i = 0; i < difference; i++)
		{
			minutesStr = "0" + minutesStr;
		}
		
		difference = 2 - secondsStr.Length();
		for(i = 0; i < difference; i++)
		{
			secondsStr = "0" + secondsStr;
		}
		
		return minutesStr + ":" + secondsStr;
	}
		
	//========================================
	//========== Conditional Functions =======
	//========================================
		
	override bool IsTimer()
	{
		return true;
	}
	
	override bool IsActive()
	{
		return IsRunning();
	}
	
	override bool HasUI()
	{
		return true;
	}
	
	override bool CanOpenUI()
	{
		MOE_ConfigDataTimer configData = GetConfigData_Timer();
		if(configData && (configData.MinTimer == configData.MaxTimer))
		{
			return false;
		}
		
		return !IsRunning();
	}
	
	bool CanSendTimerUpdates()
	{
		return m_ClientCanSendTimer;
	}
	
	bool CanBeStarted()
	{
		return !IsRunning() && ValidateConnectedExplosive() && m_ConnectedExplosive.IsArmed() && m_ConnectedExplosive.CanExplode();
	}
	
	bool CanBeStopped()
	{
		if(ValidateConnectedExplosive())
		{
			return IsRunning() && !m_ConnectedExplosive.IsArmed();
		}
		
		return IsRunning();
	}
	
	bool IsRunning()
	{
		return m_IsRunning;
	}
	
	bool IsStopped()
	{
		return !m_IsRunning;
	}
	
	bool HasLight()
	{
		return GetLightColors().Count() != 0;
	}
	
	bool HasBeepingSound()
	{
		return GetBeepingSoundSet() != "";
	}
	
	//========================================
	//========== Modded Functions ============
	//========================================
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		if(rpc_type == MOE_ERPCTypes.SYNC)
		{
			Param1<int> clientTime;
			if(!ctx.Read(clientTime))
			{
				return;
			}
			
			SetSelectedTime(clientTime.param1);
		}
	}		
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if(m_IsRunning)
		{
			if(!m_Timer || !m_Timer.IsRunning())
			{
				StartTimer();
			}
			
			m_Timer.SetRemainingTime_MOE(m_RemainingTime);
		}
		else 
		{
			if(m_Timer)
			{			
				StopTimer();
			}
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(MOE_ActionOpenTriggerComponentUI);
		AddAction(MOE_ActionOpenTriggerComponentUIInHands);

	}
	
	override bool DescriptionOverride(out string output)
	{		
		output = ConfigGetString("descriptionShort");
		
		MOE_ConfigDataTimer timerData = GetConfigData_Timer();
		
		float minTimer = timerData.MinTimer;
		float maxTimer = timerData.MaxTimer;
		
		if(minTimer == maxTimer)
		{
			output += string.Format("\n%1:\t%2", "#STR_MOE_Timer_Time", minTimer);	
			return true;		
		}
		
		output += string.Format("\n%1:\t%2", "#STR_MOE_Timer_MinTime", timerData.MinTimer);
		output += string.Format("\n%1:\t%2", "#STR_MOE_Timer_MaxTime", timerData.MaxTimer);
		
		return true;
	}
	
	//========================================
	//======= Access Cached Config Data ======
	//========================================
	
	override void LoadConfigData()
	{
		m_ConfigData = GetMOE().GetTimerData(GetType());
	}
	
	MOE_ConfigDataTimer GetConfigData_Timer()
	{
		return MOE_ConfigDataTimer.Cast(GetConfigData());
	}
	
	float GetMaxTimer()
	{
		return GetConfigData_Timer().MaxTimer;
	}
	
	float GetMinTimer()
	{
		return GetConfigData_Timer().MinTimer;
	}
	
	float GetInitialTickrate()
	{
		return GetConfigData_Timer().InitialTickrate;
	}
	
	float GetTickSpeedIncreaseFactor()
	{
		return GetConfigData_Timer().TickSpeedIncreaseFactor;
	}
	
	array<float> GetTimerStages()
	{
		return GetConfigData_Timer().TimerStages;
	}
	
	NoiseParams GetNoiseParams()
	{
		return GetConfigData_Timer().NoiseParameters;
	}
	
	string GetBeepingSoundSet()
	{
		return  GetConfigData_Timer().BeepingSoundset;
	}
}