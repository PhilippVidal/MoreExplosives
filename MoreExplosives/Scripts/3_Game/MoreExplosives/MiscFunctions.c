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
}
#endif