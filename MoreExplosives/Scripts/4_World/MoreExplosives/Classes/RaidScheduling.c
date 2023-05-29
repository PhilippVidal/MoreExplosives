class MOE_RaidSchedulingBase 
{
	bool IsInRaidSchedule()
	{
		return true;
	}
}

class MOE_RaidScheduling : MOE_RaidSchedulingBase
{
	//! Tomohiko Sakamoto Algorithm
	//0 = Sunday 
	//1 = Monday 
	//...
	//6 = Saturday
	int GetWeekdayFromDate(int year, int month, int day)
	{
		int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
		
		if (month < 3)
		{
			year -= 1;
		}
		
		return (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7;	
	}
	
	void AdjustTimeWithOffset(int utcOffset, out int year, out int month, out int day, out int hour)
	{
		if(utcOffset == 0)
		{
			return;
		}
		
		utcOffset = Math.Clamp(utcOffset, -24, 24);
		
		//Handle offset for hour/day 
		hour += utcOffset;
		int daysInMonth = GetDaysInMonth(year, month);
		if(hour >= 24)
		{
			day++;
			hour = hour % 24;
		}
		else if(hour < 0)
		{
			day--;
			hour = (24 + hour) % 24;
		}
		
		//Handle end of month/year 
		if(day > daysInMonth)
		{
			month++;
			
			if(month > 12)
			{
				year++;
				month = 1;
			}
			
			day = 1;
		}
		else if(day < 1)
		{
			month--;
			
			if(month < 1)
			{
				year--;
				month = 12;
			}
			
			day = GetDaysInMonth(year, month);
		}	
	}
	
	int GetDaysInMonth(int year, int month)
	{
		switch(month)
		{
			case 4:
			case 6:
			case 9:
			case 11:
				return 30;
			case 2:
				if(!(year % 4)) return 29;
				return 28;
		}
		
		return 31;
	}
	
	override bool IsInRaidSchedule()
	{
		int year, month, day;
		GetYearMonthDayUTC(year, month, day);

		int hour, minute, second;
		GetHourMinuteSecondUTC(hour, minute, second);
		
		string raidSchedulingCfgPath = CFG_MOE + "RaidScheduling ";
		int timeZoneOffset = GetGame().ConfigGetInt(raidSchedulingCfgPath + "timeZoneOffset"); 
		AdjustTimeWithOffset(timeZoneOffset, year, month, day, hour);
		
		int weekday = GetWeekdayFromDate(year, month, day);
		string weekdayName; 
		switch(Math.Clamp(weekday, 0, 6))
		{
			case 0: 
				weekdayName = "Sunday";
			break;
			case 1: 
				weekdayName = "Monday";
			break;
			case 2: 
				weekdayName = "Tuesday";
			break;
			case 3: 
				weekdayName = "Wednesday";
			break;
			case 4: 
				weekdayName = "Thursday";
			break;
			case 5: 
				weekdayName = "Friday";
			break;
			case 6: 
				weekdayName = "Saturday";	
			break;		
		}
		
		array<float> timeSlots = new array<float>();
		GetGame().ConfigGetFloatArray(raidSchedulingCfgPath + weekdayName + " timeSlots", timeSlots);
		
		if(!timeSlots || !timeSlots.Count())
		{
			return false;
		}
		
		if(timeSlots.Count() % 2)
		{
			timeSlots.Remove(timeSlots.Count() - 1);
		}

		float currentTime = hour + ((minute * 60.0 + second) / 3600);
		for(int i = 0; i < timeSlots.Count(); i += 2)
		{
			if(currentTime > timeSlots[i] && currentTime < timeSlots[i + 1])
			{
				return true;
			}
		}
		
		return false;
	}
}