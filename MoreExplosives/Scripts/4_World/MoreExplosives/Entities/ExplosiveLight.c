class MOE_TriggerLight : PointLightBase 
{
	protected bool m_IsOn;
	protected float m_ConfigRadius = 10;
	protected float m_ConfigBrightness = 10;
	protected vector m_ColorToDisplay = "1 1 1";
	
	protected ref Timer m_TurnOffTimer;
	protected MOE_ExplosiveBase m_ParentExplosive;
	
	void InitLight(notnull MOE_ExplosiveBase explosive, notnull MOE_ComponentTriggerBase trigger)
	{
		m_ParentExplosive = explosive;
		SetVisibleDuringDaylight(false);
		SetFlareVisible(false);
		FadeIn(0);
		SetFadeOutTime(0);

		SetConfigRadius(trigger.GetLightRadius());
		SetConfigBrightness(trigger.GetLightBrightness());
		SetColorToDisplay(trigger.GetLightColor());
	
		SetRadiusTo(0);
		SetBrightnessTo(0);
		
		SetCastShadow(false);
	}
	
	void SetConfigRadius(float radius)
	{
		m_ConfigRadius = radius;
	}
	
	void SetConfigBrightness(float brightness)
	{
		m_ConfigBrightness = brightness;
	}
	
	
	void TurnOn()
	{
		if(m_TurnOffTimer)
		{
			if(m_TurnOffTimer.IsRunning())
			{
				m_TurnOffTimer.Stop();
			}
		}
		
		SetRadiusTo(m_ConfigRadius);
		SetBrightnessTo(m_ConfigBrightness);
		m_IsOn = true;
		OnLightTurnedOn();
	}
	
	void TurnOff()
	{
		if(m_TurnOffTimer)
		{
			if(m_TurnOffTimer.IsRunning())
			{
				m_TurnOffTimer.Stop();
			}
		}
		
		SetRadiusTo(0);
		SetBrightnessTo(0);
		m_IsOn = false;
		OnLightTurnedOff();
	}
	
	void SetState(bool state)
	{
		if(state)
		{
			TurnOn();
			return;
		}
		
		TurnOff();
	}
	
	void Toggle()
	{
		SetState(!IsOn());
	}
	
	bool IsOn()
	{
		return m_IsOn;
	}
	
	void SetColorToDisplay(vector color)
	{
		m_ColorToDisplay = color;
		UpdateDisplayedColor();
	}
	
	vector GetColorToDisplay()
	{
		return m_ColorToDisplay;
	}
	
	void UpdateDisplayedColor()
	{
		float r = m_ColorToDisplay[0];
		float g = m_ColorToDisplay[1];
		float b = m_ColorToDisplay[2];
		SetDiffuseColor(r, g, b);
		SetAmbientColor(r, g, b);
	}
	
	void TurnOnForSeconds(float seconds)
	{
		TurnOn();
		if(!m_TurnOffTimer)
		{
			m_TurnOffTimer = new Timer;
		}
		
		m_TurnOffTimer.Run(seconds, this, "TurnOff");
	}
	
	void OnLightTurnedOn()
	{
		if(m_ParentExplosive)
		{
			m_ParentExplosive.OnTriggerLightTurnedOn();
		}
	}
	
	void OnLightTurnedOff()
	{
		if(m_ParentExplosive)
		{
			m_ParentExplosive.OnTriggerLightTurnedOff();
		}
	}
}