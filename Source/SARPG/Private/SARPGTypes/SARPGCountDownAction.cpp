// Clement Lecoeur All Rights Reserved


#include "SARPGTypes/SARPGCountDownAction.h"

void FSARPGCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel) {
		CountDownOutput = ESARPGCountDownActionOutput::Canceled;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}

	if(ElapsedTimeSinceStart >= TotalCountDownTime) {
		CountDownOutput = ESARPGCountDownActionOutput::Completed;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}

	if (ElapsedInterval < UpdateInterval) {
		ElapsedInterval += Response.ElapsedTime();
	}
	else {
		ElapsedTimeSinceStart += UpdateInterval > 0.0f ? UpdateInterval : Response.ElapsedTime();
		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;
		CountDownOutput = ESARPGCountDownActionOutput::Updated;
		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
		ElapsedInterval = 0.f;
	}
}

void FSARPGCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
