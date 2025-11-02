// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/SARPGCharacterAnimInstance.h"
#include "SARPGHeroAnimInstance.generated.h"

class ASARPGHeroCharacter;
/**
 * 
 */
UCLASS()
class SARPG_API USARPGHeroAnimInstance : public USARPGCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	ASARPGHeroCharacter* OwningHeroCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxStateThreshold = 5.0f;

	float IdleElapsedTime;
};
