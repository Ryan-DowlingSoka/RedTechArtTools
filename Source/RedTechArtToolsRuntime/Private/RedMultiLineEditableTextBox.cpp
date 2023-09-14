// MIT License
//
// Copyright (c) 2022 Ryan DowlingSoka
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "RedMultiLineEditableTextBox.h"
#include "Launch/Resources/Version.h"

#include "Widgets/Input/SMultiLineEditableTextBox.h"

URedMultiLineEditableTextBox::URedMultiLineEditableTextBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , bShiftEnterForNewLine(true)
{
};

TSharedRef<SWidget> URedMultiLineEditableTextBox::RebuildWidget()
{
	// Copied from UMultiLineEditableTextBox may get out of date.
	// Ideally this should use Super::RebuildWidget and modify it there,
	// but i'm not sure that is possible. >_>

	MyEditableTextBlock = SNew(SMultiLineEditableTextBox)
	.AllowContextMenu(AllowContextMenu)
	.Style(&WidgetStyle)
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION <= 1 && ENGINE_PATCH_VERSION >= 1
	.TextStyle(&TextStyle);
#endif
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 1
	.IsReadOnly(GetIsReadOnly())
#else
	.IsReadOnly(bIsReadOnly)
#endif
		//		.MinDesiredWidth(MinimumDesiredWidth)
		//		.Padding(Padding)
		//		.IsCaretMovedWhenGainFocus(IsCaretMovedWhenGainFocus)
		//		.SelectAllTextWhenFocused(SelectAllTextWhenFocused)
		//		.RevertTextOnEscape(RevertTextOnEscape)
		//		.ClearKeyboardFocusOnCommit(ClearKeyboardFocusOnCommit)
		//		.SelectAllTextOnCommit(SelectAllTextOnCommit)
		.VirtualKeyboardOptions(VirtualKeyboardOptions)
		.VirtualKeyboardDismissAction(VirtualKeyboardDismissAction)
		.OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnTextChanged))
		.OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, HandleOnTextCommitted))
		.ModiferKeyForNewLine(bShiftEnterForNewLine ? EModifierKey::Shift : EModifierKey::None);

	return MyEditableTextBlock.ToSharedRef();
}
