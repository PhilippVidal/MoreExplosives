modded class ItemActionsWidget
{
    override void BuildCursor()
    {
        super.BuildCursor();

		MOE_DetonatorBase detonator;
        if(CastTo(detonator, m_EntityInHands))
        {
            SetDetonatorValues(detonator, "ia_item_subdesc", "ia_item_subdesc_up", "ia_item_subdesc_down", "ia_item", "ia_item_quantity_pb", "ia_item_quantity_text");
        }
    }

	protected void SetDetonatorValues(MOE_DetonatorBase detonator, string subDescWidget, string upWidget, string downWidget, string itemWidget, string quantityPBWidget, string quantityTextWidget)
	{		
		Widget itemSubDescWidget;		
		itemSubDescWidget = m_Root.FindAnyWidget(subDescWidget);
		
		TextWidget txtUpWidget;
		Class.CastTo(txtUpWidget, itemSubDescWidget.FindAnyWidget(upWidget));
		
		TextWidget txtDownWidget;
		Class.CastTo(txtDownWidget, itemSubDescWidget.FindAnyWidget(downWidget));
		
		
		Widget widget;
		widget = m_Root.FindAnyWidget(itemWidget);
		
		ProgressBarWidget progressBar;
		Class.CastTo(progressBar, widget.FindAnyWidget(quantityPBWidget));
		
		TextWidget txtQuantityWidget;
		Class.CastTo(txtQuantityWidget, widget.FindAnyWidget(quantityTextWidget));

		
		string detonatorMode = typename.EnumToString(MOE_EDetonatorModes, detonator.GetSelectedDetonatorMode());
		int connectedExplosiveCount = detonator.GetConnectionCount();
		int selectedExplosive = 0;
		
		if(connectedExplosiveCount > 0)
		{
			selectedExplosive = detonator.GetSelectedReceiverIndex() + 1;
		}
		
		txtUpWidget.SetText(string.Format("Linked: %1/%2", connectedExplosiveCount.ToString(), detonator.GetMaxConnectedReceivers()));
		txtDownWidget.SetText("Selected: " + selectedExplosive.ToString());
		txtQuantityWidget.SetText(detonatorMode);
		
		if(detonatorMode != "SINGLE")	
		{		
			txtDownWidget.SetText("");
		}
			
		txtQuantityWidget.Show(true);
		txtQuantityWidget.GetParent().Show(true);
		progressBar.Show(false);
		itemSubDescWidget.Show(true);
	}
}