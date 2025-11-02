// Clement Lecoeur All Rights Reserved


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/SARPGWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawnWidget(USARPGWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnWidgetIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty()) {
		return;
	}

	for (USARPGWidgetBase* DrawnWidget : EnemyDrawnWidgets) {
		if (DrawnWidget) {
			DrawnWidget->RemoveFromParent();
		}
	}

	EnemyDrawnWidgets.Empty();
}
