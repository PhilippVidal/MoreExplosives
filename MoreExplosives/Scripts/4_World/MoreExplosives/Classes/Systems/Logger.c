static void Log_MOE(string message, int type = 0)
{
	MOE_Logger.GetInstance().Log(message, type);
}

static void Log_MOE(array<string> messages, int type = 0)
{
	MOE_Logger.GetInstance().Log(messages, type);
}

class MOE_Logger 
{
	protected static ref MOE_Logger m_Instance;
	
	const string LOG_SUB_PATH = "Logs/";
	protected string m_LogName;
	protected string m_FullLogPath;
	protected FileHandle m_LogFileHandle;
	
	void MOE_Logger()
	{
		m_LogName = HDSN_MiscFunctions.GetCurrentDateAndTimeAsPath() + ".txt";		
		SetupLogDirectories();
		
		m_FullLogPath = string.Format("%1/%2", MoreExplosives.MOD_BASE_PATH + LOG_SUB_PATH, GetLogName());
	}
	
	void Write(string message)
	{
		FPrint(m_LogFileHandle, message);
	}
	
	void WriteLine(string message)
	{
		FPrintln(m_LogFileHandle, message);	
	}
	
	bool GetLogFileHandle(out FileHandle logFileHandle)
	{
		SetupLogDirectories();
		m_LogFileHandle = OpenFile(m_FullLogPath, FileMode.APPEND);
		return m_LogFileHandle != 0;
	}
	
	void Log(string message, int type = -1)
	{
		if(!GetLogFileHandle(m_LogFileHandle))
		{
			return;
		}
		
		string prefix;
		
		if(type > -1)
		{
			prefix = string.Format(
						"[%1] %2 ", 
						HDSN_MiscFunctions.GetCurrentDateAndTime(), 
						GetLogPrefix(type));
		} 
		
		WriteLine(prefix + message);
		
		CloseFile(m_LogFileHandle);
	}
	
	
	void Log(array<string> messages, int type = -1)
	{		
		if(!messages || !messages.Count() || !GetLogFileHandle(m_LogFileHandle))
		{
			return;
		}
		
		string prefix;		
		if(type > -1)
		{
			prefix = string.Format(
						"[%1] %2 ", 
						HDSN_MiscFunctions.GetCurrentDateAndTime(), 
						GetLogPrefix(type));
		} 
		
		messages[0] = prefix + messages[0];
		
		foreach(string message : messages)
		{
			Write(message);
		}
	
		CloseFile(m_LogFileHandle);
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