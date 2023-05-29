modded class PlayerBase 
{	
	override void PlacingStartServer(ItemBase item)
	{
		MOE_ExplosiveBase explosive;
		if (CastTo(explosive, item))
		{
			m_HologramServer = new MOE_Hologram(this, GetLocalProjectionPosition(), item);
			GetHologramServer().SetProjectionPosition(GetLocalProjectionPosition());
			GetHologramServer().SetProjectionOrientation(GetLocalProjectionOrientation());
			GetHologramServer().GetProjectionEntity().OnPlacementStarted(this);
			//GetHologramServer().CheckPowerSource();
			//GetHologramServer().RefreshVisual();
		}
		
		super.PlacingStartServer(item);
	}
	
	override void PlacingStartLocal(ItemBase item)
	{
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
		{
			return;
		}
		
		
		MOE_ExplosiveBase explosive;
		if(CastTo(explosive, item))
		{
			vector pos = GetLocalProjectionPosition();
			m_HologramLocal = new MOE_Hologram(this, pos, item);
			GetHologramLocal().GetProjectionEntity().OnPlacementStarted(this);
			return;
		}
		
		super.PlacingStartLocal(item);		
	}
}