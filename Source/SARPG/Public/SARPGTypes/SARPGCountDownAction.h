// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SARPGTypes/SARPGEnumTypes.h"

class FSARPGCountDownAction : public FPendingLatentAction
{
public:
	FSARPGCountDownAction(float InTotalCountDownTime, float InUpdateInterval, float& InOutRemainingTime, ESARPGCountDownActionOutput& InCountDownOuput, const FLatentActionInfo& LatentInfo)
		: bNeedToCancel(false)
		, TotalCountDownTime(InTotalCountDownTime)
		, UpdateInterval(InUpdateInterval)
		, OutRemainingTime(InOutRemainingTime)
		, CountDownOutput(InCountDownOuput)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, ElapsedInterval(0.f)
		, ElapsedTimeSinceStart(0.f)
	{

	}

	virtual void UpdateOperation(FLatentResponse& Response) override;

	void CancelAction();

private:
	bool bNeedToCancel;
	float TotalCountDownTime;
	float UpdateInterval;
	float& OutRemainingTime;
	ESARPGCountDownActionOutput& CountDownOutput;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
};
