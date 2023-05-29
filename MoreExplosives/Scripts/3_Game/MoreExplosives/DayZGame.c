modded class DayZGame
{
	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{		
		if(rpc_type == MOE_ERPCTypes.CONFIG)
		{
#ifdef SERVER
			GetMOE().GetConfig().SentDataToClient(sender);
#else 
			GetMOE().GetConfig().ReceiveDataFromServer(ctx);
#endif	
			return;
		}

		super.OnRPC(sender, target, rpc_type, ctx);
	}
}