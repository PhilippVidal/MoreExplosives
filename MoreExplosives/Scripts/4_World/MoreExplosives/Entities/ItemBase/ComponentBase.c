//Base class for all components of MOE_Explosives 
//Since such components can have complex behaviour (especially the triggers)
//they have the option to manage their interactions through UIs instead of actions 
class MOE_ComponentBase : ItemBase 
{
	MOE_ExplosiveBase m_ConnectedExplosive;
	
	void MOE_ComponentBase()
	{
		Init_MOE();
	}
	
	void ~MOE_ComponentBase()
	{
		Fini_MOE();
	}
	
	protected void Init_MOE() {}
	
	protected void Fini_MOE() {}
	
	void OpenUI() {}
	
	bool ValidateConnectedExplosive()
	{
		return m_ConnectedExplosive || CastTo(m_ConnectedExplosive, GetHierarchyParent());
	}
	
	MOE_ExplosiveBase GetConnectedExplosive()
	{
		return m_ConnectedExplosive;
	}
	
	bool HasUI()
	{
		return false;
	}
	
	bool CanOpenUI()
	{
		return true;
	}	
	
	bool IsTrigger()
	{
		return false;
	}
}