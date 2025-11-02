// Clement Lecoeur All Rights Reserved


#include "Controllers/SARPGAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "SARPGDebugHelper.h"

ASARPGAIController::ASARPGAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	AISenseConfig_Sight->SightRadius = 5000.0f;
	AISenseConfig_Sight->LoseSightRadius = 0.0f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.0f;

	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type ASARPGAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId()) {
		return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Friendly;
}

void ASARPGAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UCrowdFollowingComponent* CrowdComponent = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent())) {
		if(bEnableDetourCrowdAvoidance){
			CrowdComponent->SetCrowdSimulationState(ECrowdSimulationState::Enabled);
		} else {
			CrowdComponent->SetCrowdSimulationState(ECrowdSimulationState::Disabled);
		}
		
		switch (DetourCrowdAvoidanceQuality) {
			case 1:
				CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);
				break;
			case 2:
				CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);
				break;
			case 3:
				CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
				break;
			case 4:
				CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
				break;
			default:
				break;
		}

		CrowdComponent->SetAvoidanceGroup(1);
		CrowdComponent->SetGroupsToAvoid(1);
		CrowdComponent->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
}

void ASARPGAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent()) {
		if (!BlackboardComponent->GetValueAsObject(FName("TargetActor"))) {
			if (Stimulus.WasSuccessfullySensed() && Actor) {
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
	}
}
