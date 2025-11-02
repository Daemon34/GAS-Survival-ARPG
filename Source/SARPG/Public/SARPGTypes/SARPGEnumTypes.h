#pragma once

UENUM()
enum class ESARPGConfirmType : uint8
{
	Yes,
	No
};

UENUM()
enum class ESARPGValidType : uint8
{
	Valid,
	Invalid
};

UENUM()
enum class ESARPGSuccessType : uint8
{
	Successfull,
	Failed
};

UENUM()
enum class ESARPGCountDownActionInput : uint8
{
	Start,
	Cancel
};

UENUM()
enum class ESARPGCountDownActionOutput : uint8
{
	Updated,
	Completed,
	Canceled
};

UENUM(BlueprintType)
enum class ESARPGGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	Extreme
};

UENUM(BlueprintType)
enum class ESARPGInputMode : uint8 {
	GameOnly,
	UIOnly
};