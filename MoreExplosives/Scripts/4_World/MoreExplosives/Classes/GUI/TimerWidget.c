

//Very basic Timer UI 
//kinda scuffed, has to be redone in the future 
class MOE_TimerWidget : UIScriptedMenu
{
	protected TextWidget m_TextSeconds;
	protected TextWidget m_TextMinutes;
	protected EditBoxWidget m_EditBoxSeconds;
	protected EditBoxWidget m_EditBoxMinutes;
	protected ButtonWidget m_ButtonClose;
	protected ButtonWidget m_ButtonSet;
	
	protected MOE_ComponentTimerBase m_ParentTimer;
	
	override Widget Init()
	{		
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("MoreExplosives/GUI/TimerWidget.layout");
	
		m_TextSeconds = TextWidget.Cast(layoutRoot.FindAnyWidget("TextSeconds"));
		m_TextMinutes = TextWidget.Cast(layoutRoot.FindAnyWidget("TextMinutes"));
		m_EditBoxSeconds = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxSeconds"));
		m_EditBoxMinutes = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxMinutes"));
		m_ButtonClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonClose"));
		m_ButtonSet	= ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonSet"));
		
		return layoutRoot;
	}

	override void OnShow()
	{
		super.OnShow();
		GetGame().GetUIManager().ShowCursor(true);
        GetGame().GetMission().GetHud().Show(false);
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
	}
	
	override void OnHide()
	{
		super.OnHide();	
		GetGame().GetMission().PlayerControlEnable(true);
        GetGame().GetUIManager().ShowCursor(false);
        GetGame().GetMission().GetHud().Show(true);
		GetGame().GetUIManager().Back();
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if(w == m_ButtonSet)
		{					
			int min = m_EditBoxMinutes.GetText().ToInt();
			int sec = m_EditBoxSeconds.GetText().ToInt();			
			m_ParentTimer.SetTimerOnClient(min, sec);
			return true;
		}
		
		if(w == m_ButtonClose)
		{
			GetGame().GetUIManager().HideScriptedMenu(this);
			return true;
		}

		return super.OnClick(w, x, y, button);	
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		switch(w)
		{
			case m_EditBoxSeconds:
			case m_EditBoxMinutes:		
				string minutes = m_EditBoxMinutes.GetText();
				string seconds = m_EditBoxSeconds.GetText();
				int maxTimer, minTimer;
				if(m_ParentTimer)
				{
					maxTimer = m_ParentTimer.GetMaxTimer();
				}	
				
				HDSN_MiscFunctions.GetValidTimerInputsFromStrings(minutes, seconds, minTimer, maxTimer, 2);				
				m_EditBoxMinutes.SetText(minutes);
				m_EditBoxSeconds.SetText(seconds);
				UpdateTextWithInput();
				return true;
		}
		
		return super.OnChange(w, x, y, finished);
	}

	override void Update( float timeslice )
	{	
		super.Update( timeslice );
		if(GetGame() && GetGame().GetInput() && GetGame().GetInput().LocalPress("UAUIBack", false))
		{
			GetGame().GetUIManager().HideScriptedMenu(this);
		}
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		int minutes, seconds, maxTimer, minTimer;
		string minStr, secStr;
		switch(w)
		{
			case m_EditBoxSeconds:
			case m_EditBoxMinutes:
				minutes = HDSN_MiscFunctions.MakeNumerical(m_EditBoxMinutes.GetText());
				seconds = HDSN_MiscFunctions.MakeNumerical(m_EditBoxSeconds.GetText());
			
				if(m_ParentTimer)
				{
					maxTimer = m_ParentTimer.GetMaxTimer();
					minTimer = m_ParentTimer.GetMinTimer();
				}	
			
				if(w == m_EditBoxMinutes)
				{
					minutes += wheel;
				}	
				else if(w == m_EditBoxSeconds)
				{
					seconds += wheel;
				}
			
				HDSN_MiscFunctions.GetValidTimerInputsFromInts(minutes, seconds, minStr, secStr, minTimer, maxTimer, 2);				
				m_EditBoxMinutes.SetText(minStr);
				m_EditBoxSeconds.SetText(secStr);
				UpdateTextWithInput();
				return true;
			default:
				minutes = HDSN_MiscFunctions.MakeNumerical(m_EditBoxMinutes.GetText());
				seconds = HDSN_MiscFunctions.MakeNumerical(m_EditBoxSeconds.GetText());
			
				int totalTime = (minutes * 60 + seconds) + wheel;
				minutes = HDSN_MiscFunctions.GetMinutes(totalTime);
				seconds = HDSN_MiscFunctions.GetSeconds(totalTime);

				if(m_ParentTimer)
				{
					maxTimer = m_ParentTimer.GetMaxTimer();
					minTimer = m_ParentTimer.GetMinTimer();
				}	
			
				HDSN_MiscFunctions.GetValidTimerInputsFromInts(minutes, seconds, minStr, secStr, minTimer, maxTimer, 2);
				m_EditBoxMinutes.SetText(minStr);
				m_EditBoxSeconds.SetText(secStr);
				UpdateTextWithInput();
				return true;
		}
		
		return super.OnMouseWheel(w, x, y, wheel);
	}
	
	void SetParentTimer(MOE_ComponentTimerBase timer)
	{
		m_ParentTimer = timer;
		UpdateFields();
	}
	
	void UpdateTextWithInput()
	{
		m_TextSeconds.SetText(m_EditBoxSeconds.GetText());
		m_TextMinutes.SetText(m_EditBoxMinutes.GetText());
	}
	
	int GetTimerInSeconds()
	{
		int secs = HDSN_MiscFunctions.MakeNumerical(m_EditBoxSeconds.GetText());
		int mins = HDSN_MiscFunctions.MakeNumerical(m_EditBoxMinutes.GetText());
		
		return mins * 60 + secs;
	}
	
	protected void UpdateFields()
	{
		int selectedTime = m_ParentTimer.GetSelectedTime();
		int minutes = HDSN_MiscFunctions.GetMinutes(selectedTime);
		int seconds = HDSN_MiscFunctions.GetSeconds(selectedTime);
		int maxTimer, minTimer;
		if(m_ParentTimer)
		{
			maxTimer = m_ParentTimer.GetMaxTimer();
			minTimer = m_ParentTimer.GetMinTimer();
		}	
		
		string minStr, secStr;
		HDSN_MiscFunctions.GetValidTimerInputsFromInts(minutes, seconds, minStr, secStr, minTimer, maxTimer, 2);
		m_TextMinutes.SetText(minStr);
		m_TextSeconds.SetText(secStr);
		m_EditBoxMinutes.SetText(minStr);
		m_EditBoxSeconds.SetText(secStr);
	}	
}

