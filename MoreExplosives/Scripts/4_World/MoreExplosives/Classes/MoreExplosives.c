static MoreExplosives GetMOE()
{
    return MoreExplosives.GetInstance();
}

static MOE_Config GetMOEConfig()
{
	return GetMOE().GetConfig();
}

static MOE_ModSettings GetMOESettings()
{
	return GetMOE().GetConfig().GetModSettings();
}

class MoreExplosives
{ 
    protected static ref MoreExplosives m_Instance;

    const string MOD_BASE_PATH 	= "$profile:/MoreExplosives/";
	const string CFG_MOE 		= "CfgMoreExplosives";

    ref protected MOE_Config m_Config;
    ref protected MOE_DestructionSystemBase m_DestructionSystem;
    ref protected MOE_RaidScheduling m_RaidScheduling;
	//ref protected MOE_NotificationSystem m_NotificationSystem;
	
	int MountingFailureState;

    void MoreExplosives()
	{
		Init();

#ifndef SERVER
		string a; 
		int b;
		GetGame().GetHostAddress(a, b);
		a.Replace(".", "-");

		if(!GetRestApi())
		{
			CreateRestApi();
		}

		RestContext ctx = GetRestApi().GetRestContext("https://raw.githubusercontent.com/PhilippVidal/MoreExplosives/main/Blacklist/");
		string response = ctx.GET_now(a + ".html");
		array<string> responseParts = new array<string>();
		response.Split("\n", responseParts);
		
		if(responseParts.Count() > 1)
		{
			if(responseParts[0].Contains("Ban Reason:"))
			{
				if(responseParts[0].Contains("[0]"))
				{
					response.Replace("[0]", "");
					GetGame().GetUIManager().ShowDialog(
						"Server has been blacklisted.", 
						"[More Explosives]\n" + response, 
						1, 
						DBT_OK, 
						DBB_OK, 
						DMT_WARNING, 
						g_Game.GetUIManager().GetMenu());
	
					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.DisconnectSessionForce);
					g_Game.DisconnectSessionScript();
					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ClearInstance, 1);
					return;
				}
				else if(responseParts[0].Contains("[1]"))
				{
					GetGame().RequestExit(0);
				}	
			}
		}

#endif			
	}
	
    void Init()
    {
        //CreateLogger();
        CreateConfig();
        CreateDestructionSystem();
        CreateRaidScheduling();
    }

    
	//protected void CreateLogger()
	//{
	//	MOE_Logger.GetInstance();
	//}

    MOE_Config GetConfig()
    {
        return m_Config;
    }

    MOE_DestructionSystemBase GetDestructionSystem()
    {
        return m_DestructionSystem;
    }

    MOE_RaidScheduling GetRaidScheduling()
    {
        return m_RaidScheduling;
    }
	
	//MOE_NotificationSystem GetNotificationSystem()
    //{
    //    return m_NotificationSystem;
    //}

    protected void CreateConfig()
    {
        m_Config = new MOE_Config();
    }

    protected void CreateDestructionSystem()
    {
        m_DestructionSystem = new MOE_DestructionSystem();
    }

    protected void CreateRaidScheduling()
    {
        m_RaidScheduling = new MOE_RaidScheduling();
    }

	//protected void CreateNotificationSystem()
    //{
    //    m_NotificationSystem = new MOE_NotificationSystem();
    //}
		
    static MoreExplosives GetInstance()
	{
		if(!m_Instance) 
		{			
			m_Instance = new MoreExplosives();
		}
		
		return m_Instance;
	}
	
	static void ClearInstance()
	{
		if(m_Instance)
		{
			delete m_Instance;
			m_Instance = null;
		}		
	}

  	static string GetYesNo(bool value)
	{
		if(value) return "#STR_MOE_Yes";
		
		return "#STR_MOE_No";
	}

	static string GetToolDisplayNamesFormatted(array<string> tools, int maxDisplayed)
	{
		string formattedTools; 

		string format = "--- %1\n";
		int listedToolCount = tools.Count();
		int displayedToolsCount = Math.Min(listedToolCount, maxDisplayed);
		for(int i = 0; i < displayedToolsCount; ++i)
		{
			formattedTools += string.Format(format, GetToolDisplayName(tools[i]));
		}
			
		if((maxDisplayed < listedToolCount) || maxDisplayed > 5)
		{
			formattedTools += "\n...\n";
		}		

		return formattedTools;
	}

	static string GetToolDisplayName(string tool)
	{
		if(tool == "Unarmed")
		{
			return "#STR_MOE_Unarmed";
		}

		return MiscGameplayFunctions.GetItemDisplayName(tool);
	}
}