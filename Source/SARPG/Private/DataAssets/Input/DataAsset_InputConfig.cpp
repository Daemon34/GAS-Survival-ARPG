// Clement Lecoeur All Rights Reserved


#include "DataAssets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FSARPGInputActionConfig& InputActionConfig : NativeInputActions) {
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction) {
			return InputActionConfig.InputAction;
		}
	}

	return nullptr;
}
