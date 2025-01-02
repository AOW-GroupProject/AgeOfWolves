#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"

#include "EnvQueryContext_Target.generated.h"

/**
 *	@UEnvQueryContext_Target
 * 
 *	Environment Query Context 로  Target을 설정해줍니다.
 */
UCLASS()
class AGEOFWOLVES_API UEnvQueryContext_Target : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const;
};
