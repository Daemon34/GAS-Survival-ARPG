// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "EnemyUIComponent.generated.h"

class USARPGWidgetBase;
/**
 * 
 */
UCLASS()
class SARPG_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawnWidget(USARPGWidgetBase* InWidgetToRegister);

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyDrawnWidgetIfAny();
	
private:
	TArray<USARPGWidgetBase*> EnemyDrawnWidgets;
};
