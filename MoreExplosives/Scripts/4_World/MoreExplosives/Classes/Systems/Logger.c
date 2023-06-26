static void Log_MOE(string message, int type = 0)
{
	MOE_Logger.GetInstance().Log(message, type);
}


class MOE_Logger 
{
	protected static ref MOE_Logger m_Instance;
	
	const string LOG_SUB_PATH = "Logs/";
	protected string m_LogName;
	protected string m_FullLogPath;
	
	void MOE_Logger()
	{
		m_LogName = HDSN_MiscFunctions.GetCurrentDateAndTimeAsPath() + ".txt";		
		SetupLogDirectories();
		
		m_FullLogPath = string.Format("%1/%2", MoreExplosives.MOD_BASE_PATH + LOG_SUB_PATH, GetLogName());
	}
	
	void Log(string message)
	{
		SetupLogDirectories();
		
		FileHandle fileHandle = OpenFile(m_FullLogPath, FileMode.APPEND);		
		if(fileHandle != 0)
		{
			FPrintln(fileHandle, message);		
			CloseFile(fileHandle);
		}
	}
	
	void Log(string message, int type)
	{
		Log(string.Format("[%1] %2 %3", HDSN_MiscFunctions.GetCurrentDateAndTime(), GetLogPrefix(type), message));
	}
		
	string GetLogName()
	{
		return m_LogName;
	}
	
	string GetFullLogPath()
	{
		return m_FullLogPath;
	}
	
	void SetupLogDirectories()
	{	
		string modBasePath = MoreExplosives.MOD_BASE_PATH; 
		string logBasePath = modBasePath + LOG_SUB_PATH;
		if(!FileExist(logBasePath))
		{
			if(!FileExist(modBasePath))
			{
				MakeDirectory(modBasePath);
			}
			
			MakeDirectory(logBasePath);
		}
	}
	
	string GetLogPrefix(int type)
	{
		switch(type)
		{
			case MOE_ELogTypes.RAID:
				return "[RAID]";
			case MOE_ELogTypes.EXPLOSIVE:
				return "[EXPLOSIVE]";
			case MOE_ELogTypes.DEBUG:
				return "[DEBUG]";
			case MOE_ELogTypes.CONFIG:
				return "[CONFIG]";
			case MOE_ELogTypes.ERROR:
				return "[ERROR]";
			default:
			break;
		}
		
		return "";
	}	
	
	static MOE_Logger GetInstance()
	{
		if(!m_Instance) 
		{			
			m_Instance = new MOE_Logger();
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
}