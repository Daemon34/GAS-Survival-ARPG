// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Items/PickUps/SARPGPickUpBase.h"
#include "SARPGStoneBase.generated.h"

class USARPGAbilitySystemComponent;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class SARPG_API ASARPGStoneBase : public ASARPGPickUpBase
{
	GENERATED_BODY()

public:
	void Consume(USARPGAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel);
	
protected:
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Stone Consumed"))
	void BP_OnStoneConsumed();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;
};
