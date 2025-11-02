// Clement Lecoeur All Rights Reserved


#include "Characters/SARPGHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/SARPGInputComponent.h"
#include "SARPGGameplayTags.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameModes/SARPGBaseGameMode.h"

#include "SARPGDebugHelper.h"

ASARPGHeroCharacter::ASARPGHeroCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 55.0f, 65.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent")); 
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
}

UPawnCombatComponent* ASARPGHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* ASARPGHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* ASARPGHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

void ASARPGHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull()) {
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous()) {
			int32 AbilityApplyLevel = 1;
			if (ASARPGBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<ASARPGBaseGameMode>()) {
				switch (BaseGameMode->GetCurrentGameDifficulty()) {
					case ESARPGGameDifficulty::Easy:
						AbilityApplyLevel = 4;
						break;
					case ESARPGGameDifficulty::Normal:
						AbilityApplyLevel = 3;
						break;
					case ESARPGGameDifficulty::Hard:
						AbilityApplyLevel = 2;
						break;
					case ESARPGGameDifficulty::Extreme:
						AbilityApplyLevel = 1;
						break;
					default:
						break;
				}
			}

			LoadedData->GiveToAbilitySystemComponent(SARPGAbilitySystemComponent, AbilityApplyLevel);
		}
	}
}

void ASARPGHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Forgot to assign a valid data asset as input config"))
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	USARPGInputComponent* SARPGInputComponent = CastChecked<USARPGInputComponent>(PlayerInputComponent);

	SARPGInputComponent->BindNativeInputAction(InputConfigDataAsset, SARPGGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	SARPGInputComponent->BindNativeInputAction(InputConfigDataAsset, SARPGGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

	SARPGInputComponent->BindNativeInputAction(InputConfigDataAsset, SARPGGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::Input_SwitchTargetTriggered);
	SARPGInputComponent->BindNativeInputAction(InputConfigDataAsset, SARPGGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::Input_SwitchTargetCompleted);

	SARPGInputComponent->BindNativeInputAction(InputConfigDataAsset, SARPGGameplayTags::InputTag_PickUp_Stones, ETriggerEvent::Started, this, &ThisClass::Input_PickUpStonesStarted);

	SARPGInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void ASARPGHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASARPGHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (MovementVector.Y != 0.0f) {
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	if (MovementVector.X != 0.0f) {
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASARPGHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.0f) {
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.0f) {
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASARPGHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
}

void ASARPGHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0.0f ? SARPGGameplayTags::Player_Event_SwitchTarget_Right : SARPGGameplayTags::Player_Event_SwitchTarget_Left,
		FGameplayEventData()
	);
}

void ASARPGHeroCharacter::Input_PickUpStonesStarted(const FInputActionValue& InputActionValue)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SARPGGameplayTags::Player_Event_ConsumeStones,
		FGameplayEventData()
	);
}

void ASARPGHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	SARPGAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void ASARPGHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	SARPGAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
