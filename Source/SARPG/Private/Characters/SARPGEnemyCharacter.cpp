// Clement Lecoeur All Rights Reserved


#include "Characters/SARPGEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/SARPGWidgetBase.h"
#include "Components/BoxComponent.h"
#include "SARPGFunctionLibrary.h"
#include "GameModes/SARPGBaseGameMode.h"

#include "SARPGDebugHelper.h"

ASARPGEnemyCharacter::ASARPGEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.0f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");
	EnemyHealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthBarWidgetComponent");
	EnemyHealthBarWidgetComponent->SetupAttachment(GetMesh());

	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox->SetupAttachment(GetMesh());
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

}

UPawnCombatComponent* ASARPGEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* ASARPGEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* ASARPGEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void ASARPGEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (USARPGWidgetBase* HealthWidget = Cast<USARPGWidgetBase>(EnemyHealthBarWidgetComponent->GetUserWidgetObject())) {
		HealthWidget->InitEnemyCreatedWidget(this);

	};
}

void ASARPGEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitEnemyStartUpData();
}

#if WITH_EDITOR
void ASARPGEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionBoxAttachBoneName))
	{
		LeftHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}
}
#endif // WITH_EDITOR

void ASARPGEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor)) {
		if (USARPGFunctionLibrary::IsTargetPawnHostile(this, HitPawn)) {
			EnemyCombatComponent->OnHitTargetActor(HitPawn);
		}
	}
}

void ASARPGEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull()) {
		return;
	}

	int32 AbilityApplyLevel = 1;
	if (ASARPGBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<ASARPGBaseGameMode>()) {
		switch (BaseGameMode->GetCurrentGameDifficulty()) {
		case ESARPGGameDifficulty::Easy:
			AbilityApplyLevel = 1;
			break;
		case ESARPGGameDifficulty::Normal:
			AbilityApplyLevel = 2;
			break;
		case ESARPGGameDifficulty::Hard:
			AbilityApplyLevel = 3;
			break;
		case ESARPGGameDifficulty::Extreme:
			AbilityApplyLevel = 4;
			break;
		default:
			break;
		}
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([this, AbilityApplyLevel]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get()) {

					LoadedData->GiveToAbilitySystemComponent(SARPGAbilitySystemComponent, AbilityApplyLevel);
				}
			}
		)
	);
}
