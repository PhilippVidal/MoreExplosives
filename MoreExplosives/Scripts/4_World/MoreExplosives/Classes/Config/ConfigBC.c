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
	
	MOE_JsonData_Internal GetJsonData()
	{
		return m_JsonData;
	}
	
	MOE_CategoryData_Internal GetCategory(int id)
	{
		if(!m_JsonData || !m_JsonData.Categories)
		{
			return null;
		}

		return m_JsonData.Categories.Get(id);
	}

	MOE_DestroyableObjectData_Internal GetDestroyableObject(string type)
	{
		if(!m_JsonData || !m_JsonData.DestroyableObjects)
		{
			return null;
		}

		return m_JsonData.DestroyableObjects.Get(type);
	}

	bool GetDestroyableObject(string type, out MOE_DestroyableObjectData_Internal destroyableObject)
	{
		destroyableObject = GetDestroyableObject(type);
		return destroyableObject != null;
	}

	int GetExplosiveID(string type)
	{
		if(!m_JsonData || !m_JsonData.ExplosiveIDs)
		{
			return 0;
		}

		return m_JsonData.ExplosiveIDs.Get(type);
	}

	bool IsExplosiveTypeAccepted(string explosiveType, MOE_DestroyableObjectData_Internal destroyableObject, array<int> builtPartIDs)
	{
		MOE_JsonData_Internal config; //TODO

		int explosiveID = config.GetExplosiveID(explosiveType);
		if(explosiveID < 1)
		{
			return false;
		}

		foreach(int partID : builtPartIDs)
		{
			

			int categoryID = destroyableObject.PartCategories.Get(partID);
			if(categoryID < 1)
			{
				continue;
			}

			MOE_CategoryData_Internal category = config.GetCategory(categoryID);
			if(!category)
			{
				continue;
			}

			if(category.IncompatibleExplosiveTypes.Contains(explosiveID))
			{
				return false;
			}
		}

		return true;
	}
}
