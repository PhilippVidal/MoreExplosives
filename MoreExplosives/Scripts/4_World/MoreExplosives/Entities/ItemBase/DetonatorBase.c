class MOE_DetonatorBase : MOE_SignalSource
{
	int m_SelectedReceiverIndex;
	
	//Index of selected mode from config - server only! 
	int m_CurrentDetonatorModeIndex;	
	//Actual detonator mode that is selected
	int m_SelectedDetonatorMode;

	override void Init_MOE()
	{
		super.Init_MOE();
		RegisterNetSyncVariableInt("m_SelectedReceiverIndex");
        RegisterNetSyncVariableInt("m_SelectedDetonatorMode");
		SelectDetonatorMode(GetDetonatorModes()[0]);
	}
	
	override void LoadConfigData()
	{
		m_ConfigData = GetMOEConfig().GetDetonatorData(GetType());
	}
	
	
	
	override bool CanBeTriggered()
	{
		return super.CanBeTriggered() && !(m_SelectedDetonatorMode == MOE_EDetonatorModes.SAFE);
	}
	
	override void TriggerSignal(PlayerBase player = null)
	{
		switch(m_SelectedDetonatorMode)
        {
            case MOE_EDetonatorModes.SAFE:
                break;
            case MOE_EDetonatorModes.ALL:
                SendSignalToAll(player);
                break;
            case MOE_EDetonatorModes.SINGLE:
                SendSignal(m_SelectedReceiverIndex, player);
                break;
            default:
                break;       
        }
	}
	
	void PlayTriggerAnimation() {}
	
	int GetSelectedReceiverIndex()
	{
		return m_SelectedReceiverIndex;
	}
	
	//Index for mode array read from config 
	//modes from config act as "mask" for selectable modes
	int GetCurrentDetonatorModeIndex()
	{
		return m_CurrentDetonatorModeIndex;
	}
	
	//Actual selected detonator mode 
	//Specific detonator mode can be forced through 'SelectDetonatorMode',
	//regardless of whether mode is available based on config array or not 
	int GetSelectedDetonatorMode()
    {
		return m_SelectedDetonatorMode;
    }

    int SelectNextDetonatorMode()
    {
		array<int> modes = GetDetonatorModes();
		if(!modes || modes.Count() == 0)
		{
			return -1;
		}
		
		int mode = modes[++m_SelectedDetonatorMode % modes.Count()];
		SelectDetonatorMode(mode);

		return GetSelectedDetonatorMode();
    }

	//input: MOE_EDetonatorModes value
    void SelectDetonatorMode(int i)
    {
		if(i < 0 || i > EnumTools.GetEnumSize(MOE_EDetonatorModes) - 1)
		{
			return;
		}
		
		m_SelectedDetonatorMode = i;
		OnDetonatorModeSwitched(m_SelectedDetonatorMode);
		SetSynchDirty();
		
    }
	
	int SelectNextReceiver()
	{
		if(m_ConnectedReceivers.Count() == 0)
		{
			return 0;
		}
		
		m_SelectedReceiverIndex = ++m_SelectedReceiverIndex % m_ConnectedReceivers.Count();
		OnSelectedReceiverSwitched(m_SelectedReceiverIndex);
		SetSynchDirty();
		return m_SelectedReceiverIndex;
	}
	
	string GetItemConversionType(int typeIdentifier = 0)
	{
		return "";
	}
	
	bool CanBeConvertedToItem(int typeIdentifier = 0)
	{
		return false;
	}
	
	bool CanSwitchDetonatorModes()
	{
		return true;
	}
	
	bool CanSwitchSelectedReceiver()
	{
		if(!IsTurnedOn())
		{
			return false;
		}
		
			
		if(GetSelectedDetonatorMode() != MOE_EDetonatorModes.SINGLE)
		{
			return false;
		}
		
		array<int> availableModes = GetDetonatorModes();	
		if(GetSelectedDetonatorMode() != MOE_EDetonatorModes.SINGLE || !availableModes || availableModes.Count() == 0)
		{
			return false;
		}
		
		return true;
	}
		
	void OnSelectedReceiverSwitched(int idx)
	{
		
	}
	
	
	void OnDetonatorModeSwitched(int mode)
	{
		
	}
	
	override void RemoveReceiverConnection(int idx)
	{
		super.RemoveReceiverConnection(idx);
		if(idx <= m_SelectedReceiverIndex && m_SelectedReceiverIndex > 0)
		{
			m_SelectedReceiverIndex--;
			SetSynchDirty();
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(MOE_ActionTriggerSignal);	
		AddAction(MOE_ActionConnectReceiver);
		AddAction(MOE_ActionDisconnectReceiver);
		AddAction(MOE_ActionCycleSelectedReceiver);
		AddAction(MOE_ActionCycleDetonatorMode);
	}
	
	override bool DescriptionOverride(out string output)
	{		
		output = ConfigGetString("descriptionShort");
		
		MOE_DetonatorData detonatorData = GetConfigData_Detonator();
		output += string.Format("\n%1:\t%2", "#STR_MOE_Detonator_MaxConnectedReceivers", detonatorData.MaxConnectedReceivers);
		output += string.Format("\n%1:\t%2m", "#STR_MOE_Detonator_MaxRange", detonatorData.MaxRange);
		output += string.Format("\n%1:\n", "#STR_MOE_Detonator_ConnectableReceivers");
		
		array<string> connectableReceivers = detonatorData.ConnectableReceivers;
		
		int count = Math.Min(connectableReceivers.Count(), 5);
		for(int i = 0; i < count; i++)
		{
			output += string.Format("--- %1\n", MiscGameplayFunctions.GetItemDisplayName(connectableReceivers[i]));
		}
		
		if(connectableReceivers.Count() > 5)
		{
			output += "\n...\n";
		}
		
		return true;
	}
	
	//========================================
	//======= Access Cached Config Data ======
	//========================================
	
	MOE_DetonatorData GetConfigData_Detonator()
	{
		return MOE_DetonatorData.Cast(GetConfigData());
	}
	
	array<int> GetDetonatorModes()
	{
		return GetConfigData_Detonator().DetonatorModes;
	}
}