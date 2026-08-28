// pravin's  patashala all Rights Reserved 

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Collection.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDRPG_API UListDataObject_Collection : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void AddChildListData(UListDataObject_Base* InChildListData);

	virtual  TArray<UListDataObject_Base*> GetChildListData() const;
	virtual bool bHasAnyChildListData() const;

private:

	UPROPERTY(Transient)
	TArray< UListDataObject_Base*> ChildListDataArray;



	
};
