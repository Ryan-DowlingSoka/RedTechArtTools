// Copyright Epic Games, Inc. All Rights Reserved.


#include "K2Nodes/K2Node_CastRedBPEnumValueToEnum.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "EditorCategoryUtils.h"
#include "K2Node_CastByteToEnum.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "KismetCompilerMisc.h"
#include "RedBPEnum.h"
#include "Kismet/KismetNodeHelperLibrary.h"
#include "Styling/AppStyle.h"

const FName UK2Node_CastRedBPEnumValueToEnum::RedBPEnumName(TEXT("Red BP Enum"));
const FName UK2Node_CastRedBPEnumValueToEnum::ByteInputPinName(TEXT("Byte"));

UK2Node_CastRedBPEnumValueToEnum::UK2Node_CastRedBPEnumValueToEnum(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSafe = false;
}

void UK2Node_CastRedBPEnumValueToEnum::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
	if (!Enum)
	{
		MessageLog.Error(*NSLOCTEXT("K2Node", "CastByteToNullEnumError", "Undefined Enum in @@").ToString(), this);
	}
}

void UK2Node_CastRedBPEnumValueToEnum::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FRedBPEnum::StaticStruct(), RedBPEnumName);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Byte, Enum, UEdGraphSchema_K2::PN_ReturnValue);
}

FText UK2Node_CastRedBPEnumValueToEnum::GetTooltipText() const
{
	if (Enum == nullptr)
	{
		return NSLOCTEXT("K2Node", "CastByteToEnum_NullTooltip", "Red BP Enum to Enum (bad enum)");
	}

	if(CachedTooltip.IsOutOfDate(this))
	{
		// FText::Format() is slow, so we cache this to save on performance
		CachedTooltip.SetCachedText(FText::Format(
			NSLOCTEXT("K2Node", "CastByteToEnum_Tooltip", "Red BP Enum to Enum {0}"),
			FText::FromName(Enum->GetFName())
		), this);
	}
	return CachedTooltip;
}

FText UK2Node_CastRedBPEnumValueToEnum::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetTooltipText();
}

FSlateIcon UK2Node_CastRedBPEnumValueToEnum::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "GraphEditor.Enum_16x");
	return Icon;
}

FText UK2Node_CastRedBPEnumValueToEnum::GetCompactNodeTitle() const
{
	return NSLOCTEXT("K2Node", "CastSymbol", "\x2022");
}

FName UK2Node_CastRedBPEnumValueToEnum::GetFunctionName() const
{
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(URedBPEnumBlueprintLibrary, GetValidValue);
	return FunctionName;
}

void UK2Node_CastRedBPEnumValueToEnum::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	if (bSafe && Enum)
	{
		const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

		// FUNCTION NODE
		const FName FunctionName = GetFunctionName();
		const UFunction* Function = URedBPEnumBlueprintLibrary::StaticClass()->FindFunctionByName(FunctionName);
		check(NULL != Function);
		UK2Node_CallFunction* CallValidation = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph); 
		CallValidation->SetFromFunction(Function);
		CallValidation->AllocateDefaultPins();
		check(CallValidation->IsNodePure());

		// FUNCTION ENUM PIN
		UEdGraphPin* FunctionEnumPin = CallValidation->FindPinChecked(TEXT("Enum"));
		Schema->TrySetDefaultObject(*FunctionEnumPin, Enum);
		check(FunctionEnumPin->DefaultObject == Enum);

		// FUNCTION INPUT BYTE PIN
		UEdGraphPin* OrgInputPin = FindPinChecked(RedBPEnumName);
		UEdGraphPin* FunctionIndexPin = CallValidation->FindPinChecked(TEXT("EnumeratorValue"));
		check(EGPD_Input == FunctionIndexPin->Direction && UEdGraphSchema_K2::PC_Struct == FunctionIndexPin->PinType.PinCategory);
		CompilerContext.MovePinLinksToIntermediate(*OrgInputPin, *FunctionIndexPin);

		// // UNSAFE CAST NODE
		UK2Node_CastByteToEnum* UnsafeCast = CompilerContext.SpawnIntermediateNode<UK2Node_CastByteToEnum>(this, SourceGraph); 
		UnsafeCast->Enum = Enum;
		UnsafeCast->bSafe = false;
		UnsafeCast->AllocateDefaultPins();
		//
		// // UNSAFE CAST INPUT
		UEdGraphPin* CastInputPin = UnsafeCast->FindPinChecked(ByteInputPinName);
		UEdGraphPin* FunctionReturnPin = CallValidation->GetReturnValuePin();
		check(NULL != FunctionReturnPin);
		Schema->TryCreateConnection(CastInputPin, FunctionReturnPin);

		//OUTPUT PIN
		UEdGraphPin* OrgReturnPin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
		UEdGraphPin* NewReturnPin = UnsafeCast->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
		CompilerContext.MovePinLinksToIntermediate(*OrgReturnPin, *NewReturnPin);

		BreakAllNodeLinks();
	}
}

class FKCHandler_CastRedBPEnumValueToEnum : public FNodeHandlingFunctor
{
public:
	FKCHandler_CastRedBPEnumValueToEnum(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		FNodeHandlingFunctor::RegisterNets(Context, Node); //handle literals

		UEdGraphPin* InPin = Node->FindPinChecked(UK2Node_CastRedBPEnumValueToEnum::RedBPEnumName);
		UEdGraphPin* Net = FEdGraphUtilities::GetNetFromPin(InPin);
		if (Context.NetMap.Find(Net) == NULL)
		{
			FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Context.NetNameMap->MakeValidName(Net));
			Context.NetMap.Add(Net, Term);
		}

		FBPTerminal** ValueSource = Context.NetMap.Find(Net);
		check(ValueSource && *ValueSource);
		UEdGraphPin* OutPin = Node->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
		if (ensure(Context.NetMap.Find(OutPin) == nullptr))
		{
			// We need to copy here to avoid passing in a reference to an element inside the map. The array
			// that owns the map members could be reallocated, causing the reference to become stale.
			FBPTerminal* ValueSourceCopy = *ValueSource;
			Context.NetMap.Add(OutPin, ValueSourceCopy);
		}
	}
};

FNodeHandlingFunctor* UK2Node_CastRedBPEnumValueToEnum::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	if (!bSafe)
	{
		return new FKCHandler_CastRedBPEnumValueToEnum(CompilerContext);
	}
	return new FNodeHandlingFunctor(CompilerContext);;
}

bool UK2Node_CastRedBPEnumValueToEnum::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	const UEnum* SubCategoryObject = Cast<UEnum>(OtherPin->PinType.PinSubCategoryObject.Get());
	if (SubCategoryObject)
	{
		if (SubCategoryObject != Enum)
		{
			return true;
		}
	}
	return false;
}

void UK2Node_CastRedBPEnumValueToEnum::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static void SetNodeEnum(UEdGraphNode* NewNode, FFieldVariant /*EnumField*/, TWeakObjectPtr<UEnum> NonConstEnumPtr)
		{
			UK2Node_CastRedBPEnumValueToEnum* EnumNode = CastChecked<UK2Node_CastRedBPEnumValueToEnum>(NewNode);
			EnumNode->Enum  = NonConstEnumPtr.Get();
			EnumNode->bSafe = true;
		}
	};

	UClass* NodeClass = GetClass();
	ActionRegistrar.RegisterEnumActions( FBlueprintActionDatabaseRegistrar::FMakeEnumSpawnerDelegate::CreateLambda([NodeClass](const UEnum* InEnum)->UBlueprintNodeSpawner*
	{
		UBlueprintFieldNodeSpawner* NodeSpawner = UBlueprintFieldNodeSpawner::Create(NodeClass, const_cast<UEnum*>(InEnum));
		check(NodeSpawner != nullptr);
		TWeakObjectPtr<UEnum> NonConstEnumPtr = MakeWeakObjectPtr(const_cast<UEnum*>(InEnum));
		NodeSpawner->SetNodeFieldDelegate = UBlueprintFieldNodeSpawner::FSetNodeFieldDelegate::CreateStatic(GetMenuActions_Utils::SetNodeEnum, NonConstEnumPtr);

		return NodeSpawner;
	}) );
}

FText UK2Node_CastRedBPEnumValueToEnum::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Enum);
}

void UK2Node_CastRedBPEnumValueToEnum::ReloadEnum(class UEnum* InEnum)
{
	Enum = InEnum;
	CachedTooltip.MarkDirty();
}
