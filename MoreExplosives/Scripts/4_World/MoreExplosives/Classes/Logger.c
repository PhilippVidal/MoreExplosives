static void Log_MOE(string message, int logType = 0)
{
	if(logType == MOE_ELogTypes.ERROR || logType == MOE_ELogTypes.DEBUG)
	{
		Print(GetMOE().GetLogPrefix(logType) + "	" + message);
		return;
	}
	
	GetMOE().GetLogger().Log(message, logType);
}

class MOE_Logger 
{
	protected string m_LogPath; 
	protected string m_LogName;
	protected string m_FullLogPath;
	
	void MOE_Logger(string path)
	{
		m_LogPath = path;
		m_LogName = HDSN_MiscFunctions.GetCurrentDateAndTimeAsPath() + ".txt";
		
		SetupLogDirectories();
		
		m_FullLogPath = string.Format("%1/%2", GetLogBasePath(), GetName());
	}
	
	void Log(string message)
	{
		SetupLogDirectories();
		
		FileHandle fileHandle = OpenFile(m_LogPath, FileMode.APPEND);		
		if(fileHandle != 0)
		{
			FPrintln(fileHandle, message);		
			CloseFile(fileHandle);
		}
	}
	
	void Log(string message, int logType)
	{
		Log(string.Format("[%1] %2 %3", HDSN_MiscFunctions.GetCurrentDateAndTime(), GetLogPrefix(logType), message));
	}
	
	string GetPath()
	{
		return m_LogPath;
	}
	
	string GetName()
	{
		return m_LogName;
	}
	
	string GetFullLogPath()
	{
		return m_FullLogPath;
	}
	
	void SetupLogDirectories()
	{	
		string modBasePath = GetModBasePath(); 
		string logBasePath = GetLogBasePath();
		if(!FileExist(logBasePath))
		{
			if(!FileExist(MoreExplosives.MOD_BASE_PATH))
			{
				MakeDirectory(MoreExplosives.MOD_BASE_PATH);
			}
			
			MakeDirectory(logBasePath);
		}
	}
	
	string GetModBasePath()
	{
		return MoreExplosives.MOD_BASE_PATH;
	}
	
	string GetLogBasePath()
	{
		return MoreExplosives.MOD_BASE_PATH + m_LogPath;
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
}