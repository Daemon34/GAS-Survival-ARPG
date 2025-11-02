// Clement Lecoeur All Rights Reserved


#include "AbilitySystem/GEExecCal/GEExecCal_DamageTaken.h"
#include "AbilitySystem/SARPGAttributeSet.h"
#include "SARPGGameplayTags.h"

struct FSARPGDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken);

	FSARPGDamageCapture() {
		DEFINE_ATTRIBUTE_CAPTUREDEF(USARPGAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USARPGAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USARPGAttributeSet, DamageTaken, Target, false);
	}
};

static const FSARPGDamageCapture& GetSARPGDamageCapture() {
	static FSARPGDamageCapture SARPGDamageCapture;
	return SARPGDamageCapture;
}

UGEExecCal_DamageTaken::UGEExecCal_DamageTaken()
{
	/* Slow way of doing capture */
	/*FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
		USARPGAttributeSet::StaticClass(),
		GET_MEMBER_NAME_CHECKED(USARPGAttributeSet, AttackPower)
	);

	FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
		AttackPowerProperty,
		EGameplayEffectAttributeCaptureSource::Source,
		false
	);

	RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);*/

	RelevantAttributesToCapture.Add(GetSARPGDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetSARPGDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetSARPGDamageCapture().DamageTakenDef);
}

void UGEExecCal_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetSARPGDamageCapture().AttackPowerDef, EvaluateParameters, SourceAttackPower);

	float BaseDamage = 0.0f;
	int32 UsedLightAttackComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;
	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes) {
		if (TagMagnitude.Key.MatchesTagExact(SARPGGameplayTags::Shared_SetByCaller_BaseDamage)) {
			BaseDamage = TagMagnitude.Value;
		}

		if (TagMagnitude.Key.MatchesTagExact(SARPGGameplayTags::Player_SetByCaller_AttackType_Light)) {
			UsedLightAttackComboCount = TagMagnitude.Value;
		}

		if (TagMagnitude.Key.MatchesTagExact(SARPGGameplayTags::Player_SetByCaller_AttackType_Heavy)) {
			UsedHeavyAttackComboCount = TagMagnitude.Value;
		}
	}

	float TargetDefensePower = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetSARPGDamageCapture().DefensePowerDef, EvaluateParameters, TargetDefensePower);

	if (UsedLightAttackComboCount != 0) {
		const float DamageIncreasePercentLight = (UsedLightAttackComboCount - 1) * 0.05f + 1.0f; // 5% increase per light attack combo
		BaseDamage = BaseDamage * DamageIncreasePercentLight;
	}

	if (UsedHeavyAttackComboCount != 0) {
		const float DamageIncreasePercentHeavy = UsedHeavyAttackComboCount * 0.15f + 1.0f; // 15% increase per heavy attack combo
		BaseDamage = BaseDamage * DamageIncreasePercentHeavy;
	}

	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;

	if (FinalDamageDone > 0.0f) {
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetSARPGDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamageDone
			)
		);
	}
}
