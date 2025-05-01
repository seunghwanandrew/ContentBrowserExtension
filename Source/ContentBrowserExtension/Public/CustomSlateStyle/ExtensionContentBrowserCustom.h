#pragma once

#include "Styling/SlateStyle.h"

class CONTENTBROWSEREXTENSION_API FExtensionContentBrowserCustom
{
public:
	// Getter
	static FName GetCustomIconStyleSetName() { return CustomIconStyleSetName; }
	// End - Getter

	static void InitializeCustomIconStyle();
	static void TerminateCustomIconStyle();

private:
	static FName CustomIconStyleSetName;
	static TSharedPtr<FSlateStyleSet> CreatedCustomStyleSet;

	static TSharedRef<FSlateStyleSet> CreateCustomStyleSet();
};