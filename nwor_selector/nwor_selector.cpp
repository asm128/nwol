// These sources are best viewed in Visual Studio 2017 with a screen of at least 1920x1080 screen and the zoom set to 81 %.
// Pablo Ariel Zorrilla Cepeda (asm128) Copyright (c) 2010-2017 - Distributed under the MIT License.
#include "nwor_selector.h"

#include "nwol_input.h"
#include "nwol_ascii_display.h"
#include "ascii_color.h"
#include "gui.h"
#include "nwol_storage.h"
#include "nwol_sync.h"

#include "nwol_runtime_impl.h"
#include "gstring.h"

#include "parse_number.h"

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(SApplication, "Application Selector", 0, 1);

int32_t											cleanup											(::SApplication& instanceApp)																									{ 
	for(uint32_t iModule = 0, moduleCount = instanceApp.ApplicationModulesHandle.size(); iModule < moduleCount; ++iModule)
		error_if(::nwol::moduleUnload(instanceApp.ApplicationModulesHandle[iModule]), "Failed to unload module. Maybe the module wasn't actually loaded?");
	error_if(::nwol::asciiDisplayDestroy(), "This could happen if the display wasn't successfully created in the first place.");
	error_if(::nwol::asciiTargetDestroy(instanceApp.ASCIITarget), "");
	return 0; 
}

int32_t											renderSelectorApp								(::SApplication& instanceApp)																								{
	::nwol::SASCIITarget								& target										= instanceApp.ASCIITarget;
	nwol_necall(::nwol::asciiTargetClear(target, ' ', ::nwol::ASCII_COLOR_INDEX_WHITE)	, "Unknown error!");
	nwol_necall(::nwol::renderGUIASCII(target, instanceApp.GUI)							, "Unknown error!");
	nwol_necall(::nwol::asciiDisplayPresent(target)										, "Unknown error!");
	return 0; 
}

static	const char								errorFormat1[]									= "Dynamically loaded function is null, maybe due to a buffer overrun which erased the pointers: %s.";

int32_t											renderSelection									(const ::SApplication & instanceApp)																							{
	int32_t												errVal											= 0;
 	const ::nwol::SApplicationModule					& moduleInterface								= instanceApp.ApplicationModulesHandle[instanceApp.ApplicationModuleSelected];
	error_if(errored(errVal = moduleInterface.Render()), "Failed to call module function.");

	::nwol::RUNTIME_CALLBACK_ID							callbackPointersErased							= moduleInterface.TestForNullPointerFunctions();
	if(callbackPointersErased) { 
		::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
		errVal											= -1;
	}
	return errVal;
}

int32_t											render											(::SApplication& instanceApp)																									{
	int32_t												retVal											= 0;
	// sync here. This is necessary for when the client application requests shutdown but may still be rendering a frame, so we have to wait the render call to finish before deleting the object.
	if( 1 == sync_increment(instanceApp.RenderSemaphore) ) {
		switch(instanceApp.SelectorState) {
		case SELECTOR_STATE_START				:
		case SELECTOR_STATE_MENU				: retVal = ::renderSelectorApp(instanceApp);												break;
		case SELECTOR_STATE_LOADING_SELECTION	: break;	// careful because of unique ASCII screen instance. We may be closing ours or the client module creating its own.
		case SELECTOR_STATE_RUNNING_SELECTION	: error_if(errored(::renderSelection(instanceApp)), "Failed to render client application");	break;
		default:
			error_printf("Unrecognized state: %u", (uint32_t)instanceApp.SelectorState);
		}
	}
	sync_decrement(instanceApp.RenderSemaphore);	// desync here
	return retVal;
}

int32_t											loadValidModules								(const char* modulesPath, ::nwol::SRuntimeValues* runtimeValues, ::nwol::array_pod<::nwol::SApplicationModule>& loadedModules)	{
	::nwol::array_obj<::nwol::glabel>					fileNames;
	nwol_necall(::nwol::listFiles(modulesPath, fileNames), "Cannot load modules from path: %s.", modulesPath);

	static constexpr const char							fileExtensionToLookFor	[]						= "." DYNAMIC_LIBRARY_EXTENSION;
	static constexpr const int32_t						fileExtensionLength								= ::nwol::size(fileExtensionToLookFor)-1;
	::nwol::array_obj<::nwol::glabel>					possibleModuleNames;
	for(uint32_t iFile = 0, fileCount = fileNames.size(); iFile < fileCount; ++iFile) {
		const ::nwol::glabel								& moduleName									= fileNames[iFile];
		info_printf("File found: %s.", moduleName.begin());
		if(moduleName.size() > fileExtensionLength) {
			const char											* nameText										= &moduleName[moduleName.size()-4];
			char												fileExtension	[fileExtensionLength+1]			= {};
			for(uint32_t iChar = 0, charCount = fileExtensionLength; iChar < charCount; ++iChar)
				fileExtension[iChar]							= (char)::tolower(nameText[iChar]);

			if(0 == ::strcmp(fileExtension, fileExtensionToLookFor)) {
				info_printf("DLL found: %s.", moduleName.begin());
				nwol_necall(possibleModuleNames.push_back(moduleName), "Failed to push module name. Out of memory?");
			}
		}
	}
	for(uint32_t iFile = 0, fileCount = possibleModuleNames.size(); iFile < fileCount; ++iFile) {
		const ::nwol::glabel								& moduleName									= possibleModuleNames[iFile];
		::nwol::SApplicationModule							loadedModule									= {};
		loadedModule.RuntimeValues						= runtimeValues;

		int32_t												errorLoad										= ::nwol::applicationModuleLoad(*runtimeValues, loadedModule, moduleName.begin());
		continue_warn_if(errored(errorLoad), "Module is not a valid NWOR interface: %s.", moduleName.begin());
		nwol_necall(loadedModules.push_back(loadedModule), "Failed to push module to output list. Out of memory?");
		info_printf("Valid module found: %s.", moduleName.begin());
	}
	return 0;
}

int32_t											listDLLFiles									(const char* modulesPath, ::nwol::array_obj<::nwol::glabel>& possibleModuleNames)												{
	::nwol::array_obj<::nwol::glabel>					fileNames;
	nwol_necall(::nwol::listFiles(modulesPath, fileNames), "Failed to list module files from path: '%s'.", modulesPath);
	for(uint32_t iFile=0, fileCount = fileNames.size(); iFile < fileCount; ++iFile) {
		const ::nwol::glabel								& moduleName							= fileNames[iFile];
		info_printf("File found: %s.", moduleName.begin());
		if(moduleName.size() > 4) {
			const char											* nameText								= &moduleName[moduleName.size()-4];
			char												fileExtension	[5]						= {};
			for(uint32_t iChar = 0, charCount = ::nwol::size(fileExtension)-1; iChar<charCount; ++iChar)
				fileExtension[iChar]							= (char)::tolower(nameText[iChar]);

			if(0 == ::strcmp(fileExtension, "." DYNAMIC_LIBRARY_EXTENSION))
				nwol_necall(possibleModuleNames.push_back(moduleName), "Failed to push module name to output list. Out of memory?");
		}
	}
	return 0;
}

int32_t											moduleUnloads									(::nwol::array_pod<::nwol::SApplicationModule> & loadedModules)																	{
	for(uint32_t iModule=0, moduleCount = loadedModules.size(); iModule < moduleCount; ++iModule)
		error_if(errored(::nwol::moduleUnload(loadedModules[iModule])), "Unknown reason.");
	loadedModules.clear();
	return 0;
}

int32_t											refreshModules									(::SApplication& instanceApp)																									{
	info_printf("Refreshing modules...");
#if defined(__WINDOWS__)
	const char											modulesPath	[]									= ".\\modules";
#else
	const char											modulesPath	[]									= "./modules";
#endif
	::nwol::array_obj<::nwol::glabel>					possibleModuleNames;
	error_if(errored(::listDLLFiles(modulesPath, possibleModuleNames)), "Failed to load modules from folder: %s", modulesPath);
	PLATFORM_CRT_CHECK_MEMORY();
	::nwol::array_pod<::nwol::SApplicationModule>			& loadedModules									= instanceApp.ApplicationModulesHandle;
	error_if(errored(::moduleUnloads(loadedModules)), "Why would this happen?");
	uint32_t											maxModuleNameLength								= 0;
	uint32_t											maxModuleTitleLength							= 0;
	PLATFORM_CRT_CHECK_MEMORY();
	for(uint32_t iFile=0, fileCount = possibleModuleNames.size(); iFile < fileCount; ++iFile) {
		const ::nwol::glabel								& moduleName									= possibleModuleNames[iFile];
		info_printf("DLL found: %s.", moduleName.begin());
		::nwol::SApplicationModule							loadedModule									= {};
		loadedModule.RuntimeValues						= instanceApp.RuntimeValues;
		continue_warn_if(errored(::nwol::applicationModuleLoad(*instanceApp.RuntimeValues, loadedModule, moduleName.begin())), "DLL is not a valid module: %s." , moduleName.begin());
		const uint32_t										titleLen										= loadedModule.ModuleTitle ? (uint32_t)::strlen(loadedModule.ModuleTitle) : 0;
		maxModuleNameLength								= (maxModuleNameLength	> moduleName.size())	? maxModuleNameLength	: moduleName.size();
		maxModuleTitleLength							= (maxModuleTitleLength > titleLen)				? maxModuleTitleLength	: titleLen;
		int32_t												moduleIndex										= loadedModules.push_back(loadedModule);
		if(-1 == moduleIndex) {
			error_printf("Failed to push loaded module. Out of memory?");
			error_if(errored(::nwol::moduleUnload(loadedModule)), "Is this even possible? We shouldn't get here if the module isn't unloadable! Failed to unlaod module: %s", moduleName.c_str());
			return -1;
		}
		info_printf("Valid module found: %s." , moduleName.begin());
	}
	PLATFORM_CRT_CHECK_MEMORY();

	const ::nwol::glabel								newControlLabel;
	::nwol::SGUIControl									newControl;
	newControl.AreaASCII							= {1, 1, (int32_t)newControlLabel.size(), 1};
	newControl.Text									= newControlLabel;	
	newControl.AlignArea							= ::nwol::SCREEN_TOP_LEFT;

	++maxModuleNameLength	; // Leave a space after the text
	++maxModuleTitleLength	; // Leave a space after the text
	
	static constexpr const char							itemTextPreFormat	[]							= "%%-%u.%us v%%3.3s: %%-%u.%us";
	char												itemTextFormat		[1024]						= {};
	char												versionString		[16]						= {};
	::sprintf_s(itemTextFormat, itemTextPreFormat, maxModuleNameLength, maxModuleNameLength, maxModuleTitleLength, maxModuleTitleLength);

	PLATFORM_CRT_CHECK_MEMORY();
	::nwol::gacstring									itemText										= maxModuleNameLength + maxModuleTitleLength + 56;
	PLATFORM_CRT_CHECK_MEMORY();
	newControl.AreaASCII.Size.x						= maxModuleNameLength + maxModuleTitleLength + 8;
	::nwol::SGUI										& guiSystem										= instanceApp.GUI;
	guiSystem.Controls.Resize(1);
	PLATFORM_CRT_CHECK_MEMORY();
	for(uint32_t iModuleItem = 0, moduleItemCount = loadedModules.size(); iModuleItem < moduleItemCount; ++iModuleItem) {
		const ::nwol::SApplicationModule					& _module										= loadedModules[iModuleItem];
		::sprintf_s(versionString, "%u.%u", _module.VersionMajor(), _module.VersionMinor());
		::sprintf_s(itemText.begin(), itemText.size(), itemTextFormat, _module.FilenameOriginal.c_str(), versionString, _module.ModuleTitle);
		newControl.Text									= ::nwol::glabel(itemText.begin(), ~0U);
		error_if(errored(::nwol::createControl(guiSystem, newControl)), "Failed to create control for module #%u ('s').", newControl.Text.begin());
		++newControl.AreaASCII.Offset.y;
	}
	info_printf("Finished refreshing modules.");
	return 0;
}

int32_t											setup											(::SApplication& instanceApp)																									{ 
	instanceApp.GUI.Controls.Clear();
	//static constexpr const char							inputNumber[]							= "123456ABCDEF";
	//int													charsProcessed							= -1;
	//uint64_t											resultValue								= ~0ULL;
	//::nwol::gacstring									resultString							;
	//static constexpr	const char						symbolList []							= "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	//::gods::parseArbitraryBaseInteger(16, symbolList, inputNumber, 0, (int32_t)strlen(inputNumber), &charsProcessed, &resultValue, resultString);

	::nwol::SGUI										& guiSystem										= instanceApp.GUI;
	nwol_necall(::nwol::asciiDisplayCreate(guiSystem.TargetSizeASCII), "Cannot create our humble display.");
	nwol_necall(::nwol::asciiTargetCreate(instanceApp.ASCIITarget, guiSystem.TargetSizeASCII), "Failed to create render target. Out of memory?");
	char												windowTitle[512]							= {};
	char												selectorModuleTitle[240]					= {};
	uint8_t												selectorModuleTitleLen						= (uint8_t)::nwol::size(selectorModuleTitle);
	uint32_t											selectorModuleVersion						= 0;
	windowTitle[239]								= 0;
	nwol_necall(::nwol_moduleVersion(&selectorModuleVersion), "??");
	nwol_necall(::nwol_moduleTitle(selectorModuleTitle, &selectorModuleTitleLen), "If this fails then something weird is going on.");
	::sprintf_s(windowTitle, "%s v%u.%u", selectorModuleTitle, selectorModuleVersion & 0xFFU, (selectorModuleVersion & 0xFF00U) >> 8);
	nwol_necall(::nwol::asciiDisplayTitleSet(windowTitle), "Unknown error.");

	static	const ::nwol::glabel						newControlLabel									= "Exit";
	::nwol::SGUIControl									newControl;
	newControl.AreaASCII							= {1, 1, (int32_t)newControlLabel.size(), 1};
	newControl.Text									= newControlLabel;

	// Create exit button
	nwol_necall(::nwol::createControl(guiSystem, newControl)	, "%s.", "Failed to create control");
	nwol_necall(::refreshModules(instanceApp)					, "%s.", "Failed to refresh modules");

	instanceApp.SelectorState						= ::SELECTOR_STATE_MENU;
	return 0; 
}

#ifndef VK_ESCAPE
#	define VK_ESCAPE 0x1B
#endif

int32_t											updateSelectorApp								(::SApplication& instanceApp, bool exitRequested)																				{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput										& inputSystem									= instanceApp.Input	; nwol_necall(::nwol::pollInput(inputSystem)			, "%s", "Failed to update input states.");
	::nwol::SGUI										& guiSystem										= instanceApp.GUI	; nwol_necall(::nwol::updateGUI(guiSystem, inputSystem)	, "%s", "Failed to update gui states.");

	::nwol::array_pod<::nwol::CONTROL_FLAG>				& controlFlags									= guiSystem.Controls.ControlFlags;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			info_printf("Execute %u.", iControl);
			uint32_t											selectedModuleIndex								= UINT_MAX;
			switch(iControl) {
			case 0:		
				return ::nwol::APPLICATION_STATE_EXIT;
			default:
				selectedModuleIndex								= iControl-1;
				error_if(false == ::nwol::in_range(selectedModuleIndex, 0U, instanceApp.ApplicationModulesHandle.size()), "Invalid module index: %u.", selectedModuleIndex)
				else {
					instanceApp.ApplicationModuleSelected			= selectedModuleIndex;	// Module indices are 
					instanceApp.SelectorState						= ::SELECTOR_STATE_LOADING_SELECTION;
				}
			}
		}
	return 0;
}

static	const char								errorFormat2[]									= "Module function failed: %s.";

int32_t											loadSelection									(::SApplication& instanceApp)																									{
 	::nwol::SApplicationModule							& moduleInterface								= instanceApp.ApplicationModulesHandle[instanceApp.ApplicationModuleSelected];
	::nwol::error_t										retVal											= 0;
	nwol_necall(moduleInterface.Create(), errorFormat2, "moduleCreate()"); 
	
	::nwol::RUNTIME_CALLBACK_ID							callbackPointersErased							= moduleInterface.TestForNullPointerFunctions();
	if(callbackPointersErased) { 
		::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
		retVal											= -1;
	}
	else  {
		error_if(errored(::nwol::asciiDisplayDestroy()), "Display not initialized?");
		char												windowTitle[512]								= {};
		::sprintf_s(windowTitle, "%s v%u.%u", moduleInterface.ModuleTitle, moduleInterface.VersionMajor(), moduleInterface.VersionMinor());
#if defined(__WINDOWS__)
		SetWindowText(instanceApp.RuntimeValues->Screen.PlatformDetail.hWnd, windowTitle);
#else
#	error "Not implemented."
#endif
		int32_t												errSetup										= moduleInterface.Setup(); 
		if(0 > errSetup) { 
			error_printf(errorFormat2, "moduleSetup()"); 
			retVal											= -1;
			moduleInterface.Cleanup();
			nwol_necall(::nwol::asciiDisplayCreate(instanceApp.GUI.TargetSizeASCII.x, instanceApp.GUI.TargetSizeASCII.y), "Failed to create our humble display.");
		} 
		else {
			callbackPointersErased							= moduleInterface.TestForNullPointerFunctions();
			if(callbackPointersErased) { 
				::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
				retVal											= -1;
			}
			else {
				info_printf("Client application \"%s\" set up. Starting update loop.", moduleInterface.ModuleTitle ? moduleInterface.ModuleTitle : "Untitled");
				instanceApp.SelectorState						= SELECTOR_STATE_RUNNING_SELECTION;
			}
		}
	}
	return retVal;
}

int32_t											update											(::SApplication & instanceApp, bool exitRequested)																				{
	::nwol::error_t										errResult										= 0;
	::nwol::APPLICATION_STATE							updateResult									= ::nwol::APPLICATION_STATE_INVALID;
	::nwol::RUNTIME_CALLBACK_ID							callbackPointersErased							= ::nwol::RUNTIME_CALLBACK_ID_NONE;
	::nwol::array_pod<::nwol::SApplicationModule>		& moduleHandles									= instanceApp.ApplicationModulesHandle;
	uint32_t											moduleSelectedIndex								= (uint32_t)instanceApp.ApplicationModuleSelected;
	::nwol::SApplicationModule							* moduleSelected								= iif(moduleSelectedIndex >= moduleHandles.size()) 0 : &moduleHandles[moduleSelectedIndex];
	const char											* moduleTitle									= iif(moduleSelected) moduleSelected->ModuleTitle : 0;

	switch(instanceApp.SelectorState) {
	case SELECTOR_STATE_MENU				: return ::updateSelectorApp(instanceApp, exitRequested);
	case SELECTOR_STATE_LOADING_SELECTION	: nwol_necall(::loadSelection(instanceApp), "Selected module \"%s\" failed to load.", moduleTitle);			break;
	case SELECTOR_STATE_RUNNING_SELECTION	:
		rve_if(::nwol::APPLICATION_STATE_INVALID, 0 == moduleSelected, "The module pointer is null. The only reason I can think of for this to happen is a write overrun but probably someone broke the code since I wrote this.");
		error_if(errored(updateResult = moduleSelected->Update(exitRequested)), "Module \"%s\" failed to update with error code %u.", moduleTitle, updateResult)
		else if(updateResult == ::nwol::APPLICATION_STATE_EXIT) {
			instanceApp.SelectorState						= SELECTOR_STATE_MENU;
			while(sync_compare_exchange(instanceApp.RenderSemaphore, 2, 0) == 1)
				continue;

			error_if(errored(moduleSelected->Cleanup()), "Failed to clean up module %i ('%s')", moduleTitle)
			else 
				info_printf("Module cleaned up successfully: '%s'", moduleTitle);

			callbackPointersErased							= moduleSelected->TestForNullPointerFunctions();
			if(callbackPointersErased) { 
				::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
				errResult										= ::nwol::APPLICATION_STATE_INVALID;
			}
			else 
				info_printf("Client application cleanup succeeded."); 

			::nwol::error_t										errDelete									= 0; 
			error_if(errored(errDelete = moduleSelected->Delete()), errorFormat2, errDelete, "moduleDelete()")
			else 
				info_printf("Module deleted successfully: '%s'", moduleTitle);

			callbackPointersErased							= moduleSelected->TestForNullPointerFunctions();
			if(callbackPointersErased) { 
				::nwol::printErasedModuleInterfacePointers(callbackPointersErased, errorFormat1);
				errResult										= ::nwol::APPLICATION_STATE_INVALID;
			}
			else {	// Reinitialize the selector window which was probably closed before starting the new app.
				nwol_necall(::nwol::asciiDisplayCreate(instanceApp.GUI.TargetSizeASCII.x, instanceApp.GUI.TargetSizeASCII.y), "Failed to create our humble display.");
				char												windowTitle[512]							= {};
				char												selectorModuleTitle[240]					= {};
				uint8_t												selectorModuleTitleLen						= (uint8_t)::nwol::size(selectorModuleTitle);
				uint32_t											selectorModuleVersion						= 0;
				windowTitle[239]								= 0;
				nwol_necall(::nwol_moduleVersion(&selectorModuleVersion), "??");
				nwol_necall(::nwol_moduleTitle(selectorModuleTitle, &selectorModuleTitleLen), "If this fails then something weird is going on.");
				::sprintf_s(windowTitle, "%s v%u.%u", selectorModuleTitle, selectorModuleVersion & 0xFFU, (selectorModuleVersion & 0xFF00U) >> 8);
				nwol_necall(::nwol::asciiDisplayTitleSet(windowTitle), "Unknown error.");
#if defined(__WINDOWS__)
				SetWindowText(instanceApp.RuntimeValues->Screen.PlatformDetail.hWnd, windowTitle);
#else
#	error "Not implemented."
#endif
				error_if(errored(::refreshModules(instanceApp)), "Error refreshing modules.");
				info_printf("Client application instance deleted successfully."); 
			}
			while(sync_compare_exchange(instanceApp.RenderSemaphore, 0, 2) == 3)
				continue;
		}
		break;
	}
	return errored(errResult) ? -1 : 0; 
}
