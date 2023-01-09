//Modified Timer class that is used by Timer Components to update their remaining time 
//Setting the remaining time here prevents unnecessary effort when timer is not running at all 
class MOE_Timer : Timer 
{
	protected MOE_ComponentTimerBase m_ParentTimerComponent;
	
	void SetRemainingTime_MOE(float time) 
	{
		m_time = m_duration - time;
	}
	
	override void Run(float duration, Managed obj, string fn_name, Param params = NULL, bool loop = false)
	{
		CastTo(m_ParentTimerComponent, obj);
		super.Run(duration, obj, fn_name, params, loop);
	}
	
	override void OnUpdate()
	{
		super.OnUpdate();	
				
		if(m_ParentTimerComponent) 
		{
			m_ParentTimerComponent.SetRemainingTime(GetRemaining(), true);
		}		
	}
	
	override protected void OnTimer()
	{		
		if(m_ParentTimerComponent) 
		{
			m_ParentTimerComponent.SetRemainingTime(0, true);
		}
					
		if(m_function == "") 
		{
			return;
		}
	
		super.OnTimer();
	}
}
