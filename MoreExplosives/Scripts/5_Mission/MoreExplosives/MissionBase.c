modded class MissionBase 
{   
    override UIScriptedMenu CreateScriptedMenu(int id) {

        UIScriptedMenu menu = super.CreateScriptedMenu(id);
        if(!menu) 
		{
            switch(id) 
			{
				case MOE_Constants.MENU_TIMER_UI_ID:
					menu = new MOE_TimerWidget;
				break;
            }
			
            if(menu) 
			{
                menu.SetID(id);
            }
        }
		
        return menu;
    }
}