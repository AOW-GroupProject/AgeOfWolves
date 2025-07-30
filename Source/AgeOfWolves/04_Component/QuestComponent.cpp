// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/QuestComponent.h"

#include "03_Player/PlayerStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

#include "14_Subsystem/AreaManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogQuest)
// UE_LOGFMT(LogQuest, Log, "");

//@Defualt Setting
#pragma region Default Setting 
UQuestComponent::UQuestComponent(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UQuestComponent::OnRegister()
{
	Super::OnRegister();
}

void UQuestComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UQuestComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Area Manager 캐싱
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		CachedAreaManager = GameInstance->GetSubsystem<UAreaManagerSubsystem>();
		if (!CachedAreaManager)
		{
			UE_LOGFMT(LogQuest, Error, "AreaManagerSubsystem 캐싱 실패");
		}
		else
		{
			UE_LOGFMT(LogQuest, Log, "AreaManagerSubsystem 캐싱 성공");
		}
	}
}

void UQuestComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
}

void UQuestComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromArea();
	
	Super::EndPlay(EndPlayReason);
}

void UQuestComponent::InternalBindToArea()
{
	//@Area
	if (!AreaRef.Get())
	{
		UE_LOGFMT(LogQuest, Warning, "유효하지 않은 Area와 바인딩 시도");
		return;
	}

	//@외부 바인딩...
	AreaRef.Get()->AreaAIStateChanged.AddUFunction(this, "OnAreaAIStateChanged");
	
	UE_LOGFMT(LogQuest, Log, "Area {0}와 바인딩 완료", *AreaRef.Get()->GetName());
}

void UQuestComponent::ExternalBindToPlayerState()
{
	//@World
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOGFMT(LogQuest, Error, "{0}: World is null", __FUNCTION__);
		return;
	}
	
	//@PC
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOGFMT(LogQuest, Error, "{0}: PlayerController is null", __FUNCTION__);
		return;
	}

	APlayerStateBase* PS = PC->GetPlayerState<APlayerStateBase>();
	if (!PS)
	{
		UE_LOGFMT(LogQuest, Error, "{0}: Player State가 유효하지 않습니다.", __FUNCDNAME__);
		return;
	}

	PS->NotifyPlayerDeathEvent.AddUFunction(this, "OnPlayerDeath");
}

void UQuestComponent::UnbindFromArea()
{
	//@내부 바인딩 해제...
	AreaRef.Get()->AreaAIStateChanged.RemoveAll(this);

	UE_LOGFMT(LogQuest, Log, "Area {0}와 바인딩 해제 완료", *AreaRef.Get()->GetName());
}

void UQuestComponent::UnbindFromPlayerState()
{
	//@World
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOGFMT(LogQuest, Error, "{0}: World is null", __FUNCTION__);
		return;
	}
	
	//@PC
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOGFMT(LogQuest, Error, "{0}: PlayerController is null", __FUNCTION__);
		return;
	}

	APlayerStateBase* PS = PC->GetPlayerState<APlayerStateBase>();
	if (!PS)
	{
		UE_LOGFMT(LogQuest, Error, "{0}: Player State가 유효하지 않습니다.", __FUNCDNAME__);
		return;
	}
	
	//@외부 바인딩 해제...
	PS->NotifyPlayerDeathEvent.RemoveAll(this);

	UE_LOGFMT(LogQuest, Log, "PlayerState와 바인딩 해제 완료");
}

void UQuestComponent::UpdateQuestElapsedTime(float DeltaTime)
{
	for (FEliminationQuestDataInfo& EliminationQuest : AreaQuest.EliminationQuests)
	{
		if (EliminationQuest.bHasTimeLimit)
		{
			EliminationQuest.ElapsedTime += DeltaTime;

			if (EliminationQuest.ElapsedTime >= EliminationQuest.TimeLimit)
			{
				EliminationQuest.QuestFail();
			}
		}
	}
}

void UQuestComponent::OnAreaAIStateChanged(AActor* AIActor, const FGameplayTag& StateTag,
                                           AArea* SourceArea, const FGuid& AreaID)
{
	//@ Dead 상태 태그 확인
	if (!StateTag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead"))) return;
	
	if (!AIActor)
	{
		UE_LOGFMT(LogQuest, Warning, "처치 대상 AI가 유효하지 않습니다.");
		return;
	}

	if (!SourceArea)
	{
		UE_LOGFMT(LogQuest, Warning, "처치 대상 AI의 SourceArea가 유효하지 않습니다.");
		return;
	}

	for (FEliminationQuestDataInfo& EliminationQuest : AreaQuest.EliminationQuests)
	{
		//@ 퀘스트 내용 진행
		if (EliminationQuest.ProcessEvent(FGameplayTag::EmptyTag, AIActor, SourceArea, AIActor->GetActorLocation()))
		{
			//@ 퀘스트 완료 확인
			if (EliminationQuest.CheckCompletion(SourceArea))
			{
				EliminationQuest.QuestSuccess();
            
				// 완료시 AreaQuest를 GameMode에 전달
				AAgeOfWolvesGameMode* AOWGameMode = Cast<AAgeOfWolvesGameMode>(SourceArea->GetWorld()->GetAuthGameMode());
				if (AOWGameMode)
				{
					AOWGameMode->HandleAreaQuestCompletion(EliminationQuest);  
				}
			}
		}
	}
}

void UQuestComponent::OnPlayerDeath(APlayerStateBase* DeadPlayerState)
{
	for (FEliminationQuestDataInfo& EliminationQuest : AreaQuest.EliminationQuests)
	{
		//@ 플레이어 죽음관련 퀘스트 처리
		EliminationQuest.QuestFail();
	}
}

void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOGFMT(LogQuest, Warning, "Owner가 유효하지 않습니다.");
		return;
	}

	AreaRef = Cast<AArea>(Owner);
	if (!AreaRef.IsValid())
	{
		UE_LOGFMT(LogQuest, Warning, "Owner Area가 유효하지 않습니다.");
		return;
	}
	
	if (CachedAreaManager)
	{
		AreaQuest = CachedAreaManager->GetAreaQuestDataSet(AreaRef->GetAreaNameTag());
	}
	
	//@가져온 데이터 유효성 검사 및 상세 로그
	if (!AreaQuest.AreaTag.IsValid())
	{
		UE_LOGFMT(LogQuest, Warning, "AreaTag({0})에 해당하는 퀘스트 데이터를 찾을 수 없습니다.", 
			*AreaRef->GetAreaNameTag().ToString());
		return;
	}

	InternalBindToArea();
	ExternalBindToPlayerState();
	
	UE_LOGFMT(LogQuest, Log, "=== AreaQuest 데이터 로드 완료 ===");
}

void UQuestComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//@ Quest 경과시간 업데이트
	UpdateQuestElapsedTime(DeltaTime);
}
#pragma endregion

//@Property/Info...etc
#pragma region Quest
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
