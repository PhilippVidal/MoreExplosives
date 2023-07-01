class MOE_RaidSchedulingBase 
{
	bool IsInSchedule()
	{
		return true;
	}
}

class MOE_RaidScheduling : MOE_RaidSchedulingBase
{
	override bool IsInSchedule()
	{
		int weekday;
		float time;
		GetAdjustedTime(weekday, time);
				
		return IsInTimeSlot(weekday, time);
	}

	void GetCurrentTimeUTC(out int year, out int month, out int day, out int hour, out int minute, out int second)
	{
		GetYearMonthDayUTC(year, month, day);
		GetHourMinuteSecondUTC(hour, minute, second);
	}
	
	void GetAdjustedTime(out int adjustedDay, out float adjustedTime)
	{
		int year, month, day, hour, minute, second;
		GetCurrentTimeUTC(year, month, day, hour, minute, second);
		
		AdjustTimeWithOffset(GetTimeZoneOffset(), year, month, day, hour);
		
		adjustedDay = GetWeekdayFromDate(year, month, day);
		adjustedTime = TimeToFloat(hour, minute, second);
	}
	
	bool FindNextTimeSlot(int currentDay, float currentTime, out int nextTimeSlotDay, out float nextTimeSlotStartTime)
	{
		currentDay = Math.Clamp(currentDay, 0, 6);
		nextTimeSlotDay = -1;
		
		//Check if there is a valid time slot today		
		array<float> timeSlots = new array<float>();
		if(GetTimeSlots(currentDay, timeSlots))
		{
			for(int i = 0; i < timeSlots.Count(); i += 2)
			{
				if(currentTime < timeSlots[i])
				{
					nextTimeSlotDay = currentDay;
					nextTimeSlotStartTime = timeSlots[i];
					return true;
				}
				
				if(i == 0) 
				{
					nextTimeSlotDay = currentDay;
					nextTimeSlotStartTime = timeSlots[i];
				}
			}
		}
		

		for(int weekday = (currentDay + 1) % 7; weekday != currentDay; weekday = ++weekday % 7)
		{
			if(!GetTimeSlots(weekday, timeSlots))
			{
				continue;
			}
			
			if(timeSlots && timeSlots.Count())
			{
				nextTimeSlotDay = weekday;
				nextTimeSlotStartTime = timeSlots[0];
				return true;
			}
		}
		
		return nextTimeSlotDay != -1;
	}
	
	
	bool GetTimeTillNextTimeSlot(out int days, out int hours, out int minutes)
	{
		int currentDay; 
		float currentTime; 
		GetAdjustedTime(currentDay, currentTime);
		
		int nextTimeSlotDay; 
		float nextTimeSlotStartTime;
		if(!FindNextTimeSlot(currentDay, currentTime, nextTimeSlotDay, nextTimeSlotStartTime))
		{
			return false;
		}
	
		int currentTimeStamp = (currentTime * 3600) + (86400 * currentDay);
		int nextTimeSlotTimeStamp = (nextTimeSlotStartTime * 3600) + (86400 * nextTimeSlotDay);
		
		int timeStampDifference = nextTimeSlotTimeStamp - currentTimeStamp + 604800 * (nextTimeSlotTimeStamp < currentTimeStamp);
		 
		days = timeStampDifference / (3600 * 24);
		
		int leftoverMinutes = timeStampDifference % (3600 * 24);
		hours = leftoverMinutes / 3600;
		
		leftoverMinutes = leftoverMinutes & 3600;
		
		minutes = leftoverMinutes / 60;
		
		return true;
	}
	
	int GetTimeZoneOffset()
	{
		return GetGame().ConfigGetInt(MoreExplosives.CFG_MOE + " " + "RaidScheduling timeZoneOffset"); 
	}
	
	//value % 2^n == value & (2^n - 1)
	bool GetTimeSlots(int weekday, out array<float> timeSlots)
	{ 
		GetGame().ConfigGetFloatArray(MoreExplosives.CFG_MOE + " RaidScheduling " + GetWeekdayName(Math.Clamp(weekday, 0, 6)) + " timeSlots", timeSlots);
		
		return timeSlots && timeSlots.Count() && !(timeSlots.Count() & 1);
	}

	float TimeToFloat(int hour, int minute, int second)
	{
		return hour + ((minute * 60.0 + second) / 3600);
	}

	bool IsInTimeSlot(int weekday, float time)
	{
		array<float> timeSlots = new array<float>();
		if(!GetTimeSlots(weekday, timeSlots))
		{
			return false;
		}

		for(int i = 0; i < timeSlots.Count(); i += 2)
		{
			if(time > timeSlots[i] && time < timeSlots[i + 1])
			{
				return true;
			}
		}

		return false;
	}

	//! Tomohiko Sakamoto Algorithm
	//0 = Sunday 
	//1 = Monday 
	//...
	//6 = Saturday
	int GetWeekdayFromDate(int year, int month, int day)
	{
		int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
		
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
	
	string GetWeekdayName(int i)
	{
		string weekdays[] = {
			"Sunday",
			"Monday",
			"Tuesday",
			"Wednesday",
			"Thursday",
			"Friday",
			"Saturday"
		};

		return weekdays[i];
	}
}