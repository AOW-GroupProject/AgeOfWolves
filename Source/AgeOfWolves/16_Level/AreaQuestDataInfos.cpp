#include "16_Level/AreaQuestDataInfos.h"
#include "Logging/StructuredLog.h"

#include "09_Item/Item.h"

DEFINE_LOG_CATEGORY(LogAreaQuestDataInfos)

//@Default Setting
#pragma region Default Setting
UAreaQuestDataInfos::UAreaQuestDataInfos(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

//@ 단일 프로퍼티 변경 시 검증 수행
void UAreaQuestDataInfos::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FAreaQuestDataValidationResult ValidationResult = ValidateAreaQuestDataAuto();

	if (!ValidationResult.bIsValid)
	{
		ShowValidationError(ValidationResult);

		if (!TryAutoFixValidationError(ValidationResult))
		{
			UE_LOGFMT(LogAreaQuestDataInfos, Warning,
				"AreaQuestData 무결성 검사 실패 - 수동 수정 필요: {0}",
				*ValidationResult.ErrorMessage);
		}
	}
	else
	{
		UE_LOGFMT(LogAreaQuestDataInfos, Log, "AreaQuestData 무결성 검사 통과");
	}

}

//@ 배열이나 구조체 내부 프로퍼티 변경 시에도 검증 수행
void UAreaQuestDataInfos::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	FAreaQuestDataValidationResult ValidationResult = ValidateAreaQuestDataAuto();
	
	if (!ValidationResult.bIsValid)
	{
		ShowValidationError(ValidationResult);
		TryAutoFixValidationError(ValidationResult);
	}
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateAreaQuestData() const
{
	// 1. Area Tag 유효성 검사 및 중복 검사
	FAreaQuestDataValidationResult Result = ValidateAreaTagUniqueness();
	if (!Result.bIsValid) return Result;
	
	// 2. 퀘스트 Tag 유효성 검사 및 중복 검사
	Result = ValidateQuestTagUniqueness();
	if (!Result.bIsValid) return Result;

	// 3. 퀘스트 Name 글자수 초과 검사
	Result = ValidateQuestNameLength();
	if (!Result.bIsValid) return Result;
	
	// 4. 퀘스트 Name 영문외 문자 사용 검사
	Result = ValidateQuestNameEngOnly();
	if (!Result.bIsValid) return Result;

	// 5. 퀘스트 Name 개행 사용 검사
	Result = ValidateQuestNameNewLine();
	if (!Result.bIsValid) return Result;
	
	// 6. 퀘스트 타입이 MAX인지 검사
	Result = ValidateInvalidQuestType();
	if (!Result.bIsValid) return Result;

	// 7. 보상 아이템 누락 여부 검사 
	Result = ValidateRewardItemsIfHasReward();
	if (!Result.bIsValid) return Result;
	
	if (!Result.bIsValid) return Result;
	// 8. 중복된 보상 아이템이 있는지 검사
	Result = ValidateRewardItemUniqueness();
	if (!Result.bIsValid) return Result;

	// 9. 보상 아이템의 개수 초과 검사
	Result = ValidateRewardItemQuantityExceeded();
	if (!Result.bIsValid) return Result;

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateAreaQuestDataAuto() const
{
	// 1. Area Tag 유효성 검사 및 중복 검사
	FAreaQuestDataValidationResult Result = ValidateAreaTagUniqueness();
	if (!Result.bIsValid) return Result;
	
	// 2. 퀘스트 Tag 유효성 검사 및 중복 검사
	Result = ValidateQuestTagUniqueness();
	if (!Result.bIsValid) return Result;

	// 3. 퀘스트 Name 글자수 초과 검사
	Result = ValidateQuestNameLength();
	if (!Result.bIsValid) return Result;
	
	// 4. 퀘스트 Name 영문외 문자 사용 검사
	Result = ValidateQuestNameEngOnly();
	if (!Result.bIsValid) return Result;

	// 5. 퀘스트 Name 개행 사용 검사
	Result = ValidateQuestNameNewLine();
	if (!Result.bIsValid) return Result;
	
	// 6. 퀘스트 타입이 MAX인지 검사
	Result = ValidateInvalidQuestType();
	if (!Result.bIsValid) return Result;
	
	// 7. 중복된 RewardItem이 있는지 검사
	Result = ValidateRewardItemUniqueness();
	if (!Result.bIsValid) return Result;

	// 8. 보상 아이템의 개수 초과 검사
	Result = ValidateRewardItemQuantityExceeded();
	if (!Result.bIsValid) return Result;

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateAreaTagUniqueness() const
{
	TMap<FGameplayTag, int32> UsedAreaTags;
	
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		if (AreaQuestDataInfo.AreaTag.IsValid())
		{
			if (int32* ExistingIndex = UsedAreaTags.Find(AreaQuestDataInfo.AreaTag))
			{
				return FAreaQuestDataValidationResult(
					EAreaQuestDataValidationError::DuplicateQuestTag,
					FString::Printf(TEXT("중복된 AreaTag 발견: %s (Area 인덱스 %d와 %d)"),
						*AreaQuestDataInfo.AreaTag.ToString(),
						*ExistingIndex, AreaIndex),
						AreaIndex, -1
				);
			}
			UsedAreaTags.Add(AreaQuestDataInfo.AreaTag, AreaIndex);
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateQuestTagUniqueness() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];
		TMap<FGameplayTag, int32> UsedQuestTags;
		
		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& EliminationQuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];
	
			if (EliminationQuestInfo.QuestTag.IsValid())
			{
				if (int32* ExistingIndex = UsedQuestTags.Find(EliminationQuestInfo.QuestTag))
				{
					return FAreaQuestDataValidationResult(
						EAreaQuestDataValidationError::DuplicateQuestTag,
						FString::Printf(TEXT("중복된 QuestTag 발견: %s (Area : %s, Quest 인덱스 %d와 %d)"),
							*EliminationQuestInfo.QuestTag.ToString(),
							*AreaQuestDataInfo.AreaTag.ToString(), *ExistingIndex, QuestIndex),
							AreaIndex, QuestIndex
					);
				}
				UsedQuestTags.Add(EliminationQuestInfo.QuestTag, QuestIndex);
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateQuestNameLength() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& EliminationQuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];

			if (EliminationQuestInfo.QuestName.Len() > 20)
			{
				return FAreaQuestDataValidationResult(
					EAreaQuestDataValidationError::RewardItemQuantityExceeded,
					FString::Printf(TEXT("QuestName 글자수 20자 초과: %s (Area: %s, Quest 인덱스 %d)"),
						*EliminationQuestInfo.QuestName,
						*AreaQuestDataInfo.AreaTag.ToString(), QuestIndex),
					AreaIndex, QuestIndex
				);
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateQuestNameEngOnly() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& EliminationQuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];
			const FString& QuestName = EliminationQuestInfo.QuestName;

			for (int32 CharIndex = 0; CharIndex < QuestName.Len(); ++CharIndex)
			{
				TCHAR CurrentChar = QuestName[CharIndex];

				if (!((CurrentChar >= 'A' && CurrentChar <= 'Z') ||
					  (CurrentChar >= 'a' && CurrentChar <= 'z') ||
					  (CurrentChar >= '0' && CurrentChar <= '9')))
				{
					return FAreaQuestDataValidationResult(
						EAreaQuestDataValidationError::StringNotEngOnly,
						FString::Printf(TEXT("QuestName에 영어/숫자가 아닌 문자가 포함됨: %s (Area: %s, Quest 인덱스: %d)"),
							*QuestName, *AreaQuestDataInfo.AreaTag.ToString(), QuestIndex),
						AreaIndex, QuestIndex
					);
				}
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateQuestNameNewLine() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& EliminationQuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];

			if (EliminationQuestInfo.QuestName.Contains(TEXT("\n")) || 
				EliminationQuestInfo.QuestName.Contains(TEXT("\r")) ||
				EliminationQuestInfo.QuestName.Contains(TEXT("\r\n")))
			{
				return FAreaQuestDataValidationResult(
					EAreaQuestDataValidationError::StringUseNewLine,
					FString::Printf(TEXT("QuestName에 개행 문자가 포함됨: %s (Area: %s, Quest 인덱스 %d)"),
						*EliminationQuestInfo.QuestName,
						*AreaQuestDataInfo.AreaTag.ToString(),
						QuestIndex),
					AreaIndex, QuestIndex
				);
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateInvalidQuestType() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& EliminationQuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];

			if (EliminationQuestInfo.QuestType == EQuestType::MAX)
			{
				return FAreaQuestDataValidationResult(
					EAreaQuestDataValidationError::InvalidQuestType,
					FString::Printf(TEXT("QuestType MAX 사용 불가능 (Area: %s, Quest 인덱스 %d)"),
						*AreaQuestDataInfo.AreaTag.ToString(), QuestIndex),
					AreaIndex, QuestIndex
				);
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateRewardItemsIfHasReward() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& QuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];

			if (QuestInfo.bHasReward && QuestInfo.RewardItems.Num() == 0)
			{
				return FAreaQuestDataValidationResult(
					EAreaQuestDataValidationError::MissingRewardItems,
					FString::Printf(TEXT("bHasReward가 True인데 보상 아이템이 비어 있음 (Area: %s, Quest 인덱스 %d)"),
						*AreaQuestDataInfo.AreaTag.ToString(), QuestIndex),
					AreaIndex, QuestIndex
				);
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateRewardItemUniqueness() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& QuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];

			TMap<TSubclassOf<AItem>, int32> UsedItemClasses;

			for (int32 RewardIndex = 0; RewardIndex < QuestInfo.RewardItems.Num(); ++RewardIndex)
			{
				const FQuestRewardItem& RewardItem = QuestInfo.RewardItems[RewardIndex];

				if (!RewardItem.ItemClass)
				{
					continue;
				}

				if (int32* ExistingIndex = UsedItemClasses.Find(RewardItem.ItemClass))
				{
					return FAreaQuestDataValidationResult(
						EAreaQuestDataValidationError::DuplicateRewardItem,
						FString::Printf(TEXT("중복된 RewardItem.ItemClass 발견: %s (Area: %s, Quest 인덱스 %d, 보상 인덱스 %d와 %d)"),
							*RewardItem.ItemClass->GetName(),
							*AreaQuestDataInfo.AreaTag.ToString(),
							QuestIndex,
							*ExistingIndex,
							RewardIndex),
						AreaIndex, QuestIndex
					);
				}

				UsedItemClasses.Add(RewardItem.ItemClass, RewardIndex);
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

FAreaQuestDataValidationResult UAreaQuestDataInfos::ValidateRewardItemQuantityExceeded() const
{
	for (int32 AreaIndex = 0; AreaIndex < AreaQuestSets.Num(); ++AreaIndex)
	{
		const FAreaQuestDataSet& AreaQuestDataInfo = AreaQuestSets[AreaIndex];

		for (int32 QuestIndex = 0; QuestIndex < AreaQuestDataInfo.EliminationQuests.Num(); ++QuestIndex)
		{
			const FEliminationQuestDataInfo& QuestInfo = AreaQuestDataInfo.EliminationQuests[QuestIndex];

			for (const FQuestRewardItem& RewardItem : QuestInfo.RewardItems)
			{
				if (RewardItem.Quantity > 50)
				{
					return FAreaQuestDataValidationResult(
						EAreaQuestDataValidationError::StringLengthExceeded,
						FString::Printf(TEXT("보상 아이템 %s 개수 50개 초과 (Area: %s, Quest 인덱스 %d)"),
							*RewardItem.ItemClass->GetName(),
							*AreaQuestDataInfo.AreaTag.ToString(),
							QuestIndex),
						AreaIndex, QuestIndex
					);
				}
			}
		}
	}

	return FAreaQuestDataValidationResult();
}

void UAreaQuestDataInfos::ShowValidationError(const FAreaQuestDataValidationResult& ValidationResult) const
{
	// Log 출력
	UE_LOGFMT(LogAreaQuestDataInfos, Error,
	"AreaQuestData 검증 실패 | 오류 타입: {0} | 메시지: {1} | Area 인덱스: {2} | Quest 인덱스: {3}",
	*UEnum::GetValueAsString(ValidationResult.ErrorType),
	*ValidationResult.ErrorMessage,
	ValidationResult.ProblemAreaIndex,
	ValidationResult.ProblemQuestIndex);

	// === 개발자에게 즉각적인 피드백 제공 ===
	ensureMsgf(false, TEXT("AreaQuestData 검증 실패: %s"), *ValidationResult.ErrorMessage);

	// === 화면에 시각적 경고 표시 ===
	if (GEngine)
	{
		FString ScreenMessage = FString::Printf(
			TEXT("AreaQuestData 검증 실패: %s"),
			*ValidationResult.ErrorMessage
		);

		// 빨간색으로 7초간 화면에 표시
		GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Red, ScreenMessage);
	}
}

bool UAreaQuestDataInfos::TryAutoFixValidationError(const FAreaQuestDataValidationResult& ValidationResult)
{
	switch (ValidationResult.ErrorType)
	{
	case EAreaQuestDataValidationError::InvalidQuestType:
		{
			AreaQuestSets[ValidationResult.ProblemAreaIndex].EliminationQuests[ValidationResult.ProblemQuestIndex].QuestType = EQuestType::None;
			UE_LOGFMT(LogAreaQuestDataInfos, Log, "QuestType MAX 설정으로 인해 QuestType이 None이 되었습니다.");
			return true;
		}
	case EAreaQuestDataValidationError::MissingRewardItems:
		{
			AreaQuestSets[ValidationResult.ProblemAreaIndex].EliminationQuests[ValidationResult.ProblemQuestIndex].bHasReward = false;
			UE_LOGFMT(LogAreaQuestDataInfos, Log, "RewardItem 누락에 의한 bHasReward가 false가 되었습니다.");
			return true;	
		}
	default:
		{
			UE_LOGFMT(LogAreaQuestDataInfos, Log,
				"자동 수정 불가: {0} 오류는 수동 수정이 필요합니다. Area 인덱스 {1} | Quest 인덱스 {2} 을 검토하세요.",
				*UEnum::GetValueAsString(ValidationResult.ErrorType),
				ValidationResult.ProblemAreaIndex,
				ValidationResult.ProblemQuestIndex);
			return false;
		}
	}
}

void UAreaQuestDataInfos::ValidateAreaQuestDataManually()
{
	FAreaQuestDataValidationResult ValidationResult = ValidateAreaQuestData();

	if (ValidationResult.bIsValid)
	{
		UE_LOGFMT(LogAreaQuestDataInfos, Log, "✅ AreaQuest Data 무결성 검사 성공: 모든 설정이 유효합니다");

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
				TEXT("✅ AreaQuest Data 검증 성공! 모든 Area의 QuestData 설정이 올바릅니다."));
		}
	}
	else
	{
		ShowValidationError(ValidationResult);
	}
}
#pragma endregion
