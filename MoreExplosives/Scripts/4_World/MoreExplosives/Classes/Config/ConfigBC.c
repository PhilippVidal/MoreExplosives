class MOE_ConfigBC : MOE_ConfigBase
{
	protected MOE_JsonData_Internal m_JsonData;

	const protected string m_ConfigPath = "$profile:/MoreExplosives/Jsons/";
	const protected string m_GeneralSettingsJsonName = "generalsettings.json";
	
	void MOE_ConfigBC()
	{
		InitData();
	}
	
	override void LoadGeneralSettings()
	{
		m_GeneralSettings = new MOE_GeneralSettings();	
		JsonFileLoader<MOE_GeneralSettings>.JsonLoadFile(m_ConfigPath + m_GeneralSettingsJsonName, m_GeneralSettings);		
	}
	
	protected void InitData()
	{
		m_JsonData = new MOE_JsonData_Internal(new MOE_JsonData_External());
	}	
	
	
	override void RequestDataFromServer()
	{
		if(GetGame().IsClient())
		{
			GetGame().RPC(null, MOE_ERPCTypes.CONFIG, null, true, null);
		}					
	}
	
	override void SentDataToClient(notnull PlayerIdentity client)
	{
		if(GetGame().IsClient())
		{
			return;
		}
		
		GetGame().RPC(null, MOE_ERPCTypes.CONFIG, m_ConfigData.GetClientData(), true, client);
	}
	
	override bool ReceiveDataFromServer(ParamsReadContext ctx)
	{
		m_ConfigData.ReadServerData(ctx);	
	}
	
}
