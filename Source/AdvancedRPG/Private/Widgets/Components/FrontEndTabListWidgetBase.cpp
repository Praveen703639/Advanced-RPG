// pravin's  patashala all Rights Reserved 


#include "Widgets/Components/FrontEndTabListWidgetBase.h"
#include "Editor/WidgetCompilerLog.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"

void UFrontEndTabListWidgetBase::RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName)
{
	RegisterTab(InTabID, TabButtonEntryWidgetClass, nullptr);

	if (UFrontendCommonButtonBase* FoundButton = Cast<UFrontendCommonButtonBase>(GetTabButtonBaseByID(InTabID)))
	{
		FoundButton->SetButtonText(InTabDisplayName);
	}
}

#if WITH_EDITOR	

void UFrontEndTabListWidgetBase::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(

			TEXT("The Variable TabButtonEntryWidgetClass has no valid entry specified dude!!  ") +
			GetClass()->GetName() +
			TEXT(" needs a valid entry widget class to function properly : ")

		));
	}
}
#endif
