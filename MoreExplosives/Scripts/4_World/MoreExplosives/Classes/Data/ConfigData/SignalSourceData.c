class MOE_SignalSourceDataBase : MOE_ConfigDataBase
{
	int MaxConnectedReceivers;
	float MaxRange;
	ref array<string> ConnectableReceivers; 
	float FailureChance;
	float TriggerOnRuinedChance;
}
