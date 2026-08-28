// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "FrontEndTabListWidgetBase.generated.h"

class UFrontendCommonButtonBase;


/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ADVANCEDRPG_API UFrontEndTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:

	void RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName);

private:

	//~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface


	UPROPERTY(EditAnyWhere, BlueprintReadOnly ,Category = "Front End Tab List Setting", meta = (AllowPrivateAccess = "true",ClampMin = "1",ClampMax = "10"))
	int32 DebugEditorPreviewTabCount = 3;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Front End Tab List Setting", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFrontendCommonButtonBase> TabButtonEntryWidgetClass;

	
};
