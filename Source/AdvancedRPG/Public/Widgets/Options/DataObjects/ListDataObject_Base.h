// pravin's patashala all Rights Reserved 
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ListDataObject_Base.generated.h"

// ✅ FIX: 
#define LIST_DATA_ACCESSOR(DataType,PropertyName) \
FORCEINLINE DataType Get##PropertyName() const { return PropertyName; } \
FORCEINLINE void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName; }

/**
 * */
UCLASS()
class ADVANCEDRPG_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()

public:

	LIST_DATA_ACCESSOR(FName, DataID)
		LIST_DATA_ACCESSOR(FText, DataDisplayName)
		LIST_DATA_ACCESSOR(FText, DescriptionRichText)
		LIST_DATA_ACCESSOR(FText, DisabledRichText)
		LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
		LIST_DATA_ACCESSOR(UListDataObject_Base*, ParaentData)

		void InitDataObject();

		// Empty in the base class, but can be overridden in derived classes to provide custom behavior when the data is selected.

		virtual  TArray<UListDataObject_Base*> GetChildListData() const { return TArray<UListDataObject_Base*>(); }
	    virtual bool bHasAnyChildListData() const { return false; }

protected:
	// Empty in base class , the child classes must override it to handle the initialized needed accordingly
	virtual void OnDataObjectInitialized();

private:

	FName DataID;
	FText DataDisplayName;
	FText DescriptionRichText;
	FText DisabledRichText;
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	UPROPERTY(Transient)
	UListDataObject_Base* ParaentData;
};