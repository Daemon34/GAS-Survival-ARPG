// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/SARPGBaseAnimInstance.h"
#include "SARPGHeroLinkedAnimLayer.generated.h"

class USARPGHeroAnimInstance;
/**
 * 
 */
UCLASS()
class SARPG_API USARPGHeroLinkedAnimLayer : public USARPGBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	USARPGHeroAnimInstance* GetHeroAnimInstance() const;

};
