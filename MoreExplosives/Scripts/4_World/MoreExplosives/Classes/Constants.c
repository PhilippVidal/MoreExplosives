class MOE_Constants
{	
	const int MENU_TIMER_UI_ID = 77777722;
	
	const float TIME_CONNECT_RECEIVER = 3;
	const float TIME_DISCONNECT_RECEIVER = 2;
	const float TIME_CONVERT_ITEM = 7;
	

	const float DISTANCE_MANIPULATE_EXPLOSIVE = 2;
	const float DISTANCE_MOUNT_EXPLOSIVE = 1.5;
	
	const float TIMER_SPAM_DELAY = 1;
	const float TIMER_MIN_TICKRATE = 0.1;	//in seconds
	
	
	const float ANGLE_PLACEMENT_ADJUSTMENT_THRESHOLD = 45;
	
	const PhxInteractionLayers PlacementCollisionLayers = PhxInteractionLayers.BUILDING | PhxInteractionLayers.ITEM_LARGE | PhxInteractionLayers.FENCE | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.FIREGEOM;
}