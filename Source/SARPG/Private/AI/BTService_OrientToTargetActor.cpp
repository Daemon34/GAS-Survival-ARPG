// Clement Lecoeur All Rights Reserved


#include "AI/BTService_OrientToTargetActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{
	NodeName = TEXT("Native Orient Rotation To Target Actor");

	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	RotationInterpSpeed = 5.0f; // Default interpolation speed for rotation
	Interval = 0.0f; // No interval, run every tick
	RandomDeviation = 0.0f; // No random deviation

	bNotifyTick = true;

	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset()) {
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Orient Rotation To %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp,NodeMemory, DeltaSeconds);

	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	APawn* AIOwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (AIOwningPawn && TargetActor) {
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(AIOwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
		const FRotator TargetRotation = FMath::RInterpTo(AIOwningPawn->GetActorRotation(), LookAtRotation, DeltaSeconds, RotationInterpSpeed);

		AIOwningPawn->SetActorRotation(TargetRotation);
	}
}