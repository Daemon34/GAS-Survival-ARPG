// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/SARPGBaseAnimInstance.h"
#include "SARPGCharacterAnimInstance.generated.h"

class ASARPGBaseCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class SARPG_API USARPGCharacterAnimInstance : public USARPGBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	ASARPGBaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;
};
