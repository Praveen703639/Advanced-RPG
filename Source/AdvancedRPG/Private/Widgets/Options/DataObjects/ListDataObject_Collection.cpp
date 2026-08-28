// pravin's  patashala all Rights Reserved 


#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"


void UListDataObject_Collection::AddChildListData(UListDataObject_Base* InChildListData)
{
    // Notify child list data to init itself
    InChildListData->InitDataObject();

    // set the child list data's parent to this
    InChildListData->SetParaentData(this);

    ChildListDataArray.Add(InChildListData);
}

TArray<UListDataObject_Base*> UListDataObject_Collection::GetChildListData() const
{
    return ChildListDataArray;
}

bool UListDataObject_Collection::bHasAnyChildListData() const
{
    return !ChildListDataArray.IsEmpty();
}
