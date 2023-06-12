enum MOE_ERPCTypes
{
	CONFIG 			= 711220,	
	SYNC 			= 711221,	
	EVENT_ARMED 	= 711222,	
	EVENT_EVALUATE 	= 711223	
}

enum MOE_ELogTypes
{
	NONE,
	DEBUG,
	CONFIG,
	ERROR,
	RAID,
	EXPLOSIVE
}

enum MOE_EDetonatorModes
{
    SAFE,
    ALL,
    SINGLE
}

enum MOE_EExplosiveStates
{
	DISARMED,
	ARMED,
	ACTIVE
}

enum MOE_EExplosiveStateFlags 
{
	INITIALIZED = 1,
	ARMED = 2,
	MOUNTED = 4	
}

enum MOE_EExplosiveMountingModes 
{
	EVERYWHERE, 
	BASEBUILDINGONLY,
	SELECTEDONLY,
	ANYTARGET
}

enum MOE_EDestructionSystemTypes
{
	NONE,
	DAYZ,	 
	MOE,	
	BC		
}

enum MOE_EDamageSystemTypes
{
	NONE,
	DAYZ,
	MOE,
	BC
}


typedef Param2<int, int> MOE_NetworkID;
typedef Param4<int, int, int, int> MOE_PersistentID;

typedef Param3<bool, int, int> MOE_SignalSourceSyncData;

//added/removed, item
typedef Param2<bool, MOE_StateSyncedItemBase> MOE_StateSyncedObjectHistoryEntry;

//PartName, RequiredDestroyedParts
typedef map<string, ref array<string>> MOE_RequiredDestroyedParts;