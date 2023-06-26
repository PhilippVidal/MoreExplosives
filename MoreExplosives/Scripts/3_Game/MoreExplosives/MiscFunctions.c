#ifndef HDSN_MISC
	#define HDSN_MISC

class HDSN_MiscFunctions
{	
	static int MakeNumerical(string input)
	{
		string numerical; 
		for(int i = 0; i < input.Length(); i++)
		{
			if(IsDigit(input[i]))
			{
				numerical += input[i];
			}
		}
		
		return numerical.ToInt();
	}
	
	static bool IsDigit(string input)
	{
		return input.Length() == 1 && (input.ToInt() > 0 || input == "0");
	}
		
	//IDX: 43210
	// IN: 12345
	//OUT: {5, 4, 3, 2, 1}
	static array<int> GetDigitsArray(int number)
	{
		string numberStr = number.ToString();
		array<int> digits = new array<int>();
		int count = numberStr.Length();
		for(int i = count - 1; i > -1; i--)
		{
			digits.Insert(numberStr.Get(i).ToInt());
		}
		
		return digits;
	}
	
	static int GetMinutes(int timeInSeconds)
	{
		return Math.Floor(timeInSeconds / 60);
	}
	
	static int GetSeconds(int timeInSeconds)
	{
		return timeInSeconds % 60;
	}
	
	static string GetCurrentTime()
	{
		int hour, minute, second;
		GetHourMinuteSecond(hour, minute, second);
		
		string h = hour.ToString();
		string m = minute.ToString();
		string s = second.ToString();
		
		if(hour < 10) h = "0" + h;
		
		if(minute < 10) m = "0" + m;
		
		if(second < 10) s = "0" + s;

		return h + "-" + m + "-" + s);
	}
	
	static string GetCurrentDate()
	{	
		int day, month, year;
		GetYearMonthDay(year, month, day);
		
		string d = day.ToString();
		string m = month.ToString();
		string y = year.ToString();
		
		if(day < 10) d = "0" + d;
		
		if(month < 10) m = "0" + m;
		
		if(year < 10) y = "0" + y;
		
		return d + "-" + m + "-" + y);
	}
	
	static string GetCurrentDateAndTime()
	{
		string date = GetCurrentDate();
		string time = GetCurrentTime();
		date.Replace("-", "/");
		time.Replace("-", ":");
		
		return date + "-" + time;
	}
	
	static string GetCurrentDateAndTimeAsPath()
	{		
		return GetCurrentDate() + "_" + GetCurrentTime();
	}
	
	static int SetBits(int value, int bits, bool bitValue)
	{
		return (value & ~bits) | (bitValue * bits);	
	}
	
	static bool IsBitSet(int value, int flagBit)
	{
		return (value & flagBit) != 0;
	}
	
	static bool IsBitMaskSet(int value, int mask)
	{
		return (value & mask) == mask;
	}
	
	static string IntToHexString(int input, bool removeLeadingZeros = true)
	{
		static const string hexChars[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
		
		string hexValue = "";
		for(int i = 0; i < 8; i++)
		{
			int index = (input >> (i * 4)) & 0xF;
			hexValue = hexChars[index] + hexValue;
		}
		
		if(!removeLeadingZeros)
		{
			return hexValue;
		}
		
		for(int j = 0; j < 8; j++)
		{
			if(hexValue[j] != "0")
			{
				return hexValue.Substring(j, 8 - j);
			}
		}		
			
		return "0";			
	}
	
	static void GetValidTimerInputsFromStrings(out string minutes, out string seconds, int min, int max, int maxMinutesDigits = 2)
	{
		//Make numbers only and clamp length of inputs
		minutes = ClampStringByLength(HDSN_MiscFunctions.MakeNumerical(minutes).ToString(), maxMinutesDigits);
		seconds = ClampStringByLength(HDSN_MiscFunctions.MakeNumerical(seconds).ToString(), 2);

		//Clamp numeric values so they are valid and actually make sense 
		int timeToSet = GetClampedTimerInput(minutes.ToInt(), seconds.ToInt(), min, max, maxMinutesDigits);
		
		minutes = HDSN_MiscFunctions.GetMinutes(timeToSet).ToString();
		seconds = HDSN_MiscFunctions.GetSeconds(timeToSet).ToString();
				
		minutes = FillStringWithZeros(minutes, maxMinutesDigits);	
		seconds = FillStringWithZeros(seconds, 2);			
	}
	
	static void GetValidTimerInputsFromInts(int minutes, int seconds, out string minutesStr, out string secondsStr, int min, int max, int maxMinutesDigits = 2)
	{
		//Make numbers only and clamp length of inputs
		minutesStr = ClampStringByLength(minutes.ToString(), maxMinutesDigits);
		secondsStr = ClampStringByLength(seconds.ToString(), 2);

		//Clamp numeric values so they are valid and actually make sense 
		int timeToSet = GetClampedTimerInput(minutesStr.ToInt(), secondsStr.ToInt(), min, max, maxMinutesDigits);
		
		minutesStr = HDSN_MiscFunctions.GetMinutes(timeToSet).ToString();
		secondsStr = HDSN_MiscFunctions.GetSeconds(timeToSet).ToString();
				
		minutesStr = FillStringWithZeros(minutesStr, maxMinutesDigits);	
		secondsStr = FillStringWithZeros(secondsStr, 2);			
	}
	
	static int GetClampedTimerInput(int minutes, int seconds, int min, int max, int maxMinutesDigits)
	{
		string maxedDigits;
		for(int i = 0; i < maxMinutesDigits; i++)
		{
			maxedDigits += "9";
		}
		
		//Clamp individual inputs
		int mins =  Math.Clamp(minutes, 0, maxedDigits.ToInt());
		int secs = Math.Clamp(seconds, 0, 59);
		
		//Clamp combined output
		return Math.Clamp(mins * 60 + secs, min, max);
	}
	
	static string ClampStringByLength(string str, int length)
	{
		int stringLength = str.Length();
		if(stringLength <= length)
		{
			return str;
		}
		
		return str.Substring(stringLength - length, length);
	}
	
	static string FillStringWithZeros(string str, int targetLength)
	{
		int difference = targetLength - str.Length();
		for(int i = 0; i < difference; i++)
		{
			str = "0" + str;
		}
		
		return str;
	}
	
	static int GetPivotFromComponent(Object object, int component)
	{
		return object.GetBonePivot(object.GetFireGeometryLevel(), component);
	}

	static bool ApproximatelyEqual(float a, float b, float epsilon)
	{
		return Math.AbsFloat(a - b) < epsilon;
	}
	
	static bool ApproximatelyEqual(vector a, vector b, float epsilon)
	{
		return ApproximatelyEqual(a[0], b[0], epsilon) && ApproximatelyEqual(a[1], b[1], epsilon) && ApproximatelyEqual(a[2], b[2], epsilon);
	}
}
#endif