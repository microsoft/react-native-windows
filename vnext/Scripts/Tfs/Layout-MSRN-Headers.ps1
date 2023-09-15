# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

param(
	[string] $SourceRoot = ($PSScriptRoot | Split-Path | Split-Path | Split-Path),
	[string] $TargetRoot = "$SourceRoot\vnext\target",
	[System.IO.DirectoryInfo] $ReactWindowsRoot = "$SourceRoot\vnext",
	[System.IO.DirectoryInfo] $ReactNativeRoot = "$SourceRoot\node_modules\react-native",
	[System.IO.DirectoryInfo] $ReactCommonOverrideRoot = "$ReactWindowsRoot\ReactCommon\TEMP_UntilReactCommonUpdate",
	[string[]] $Extensions = ('h', 'hpp')
)

Write-Host "Source root: [$SourceRoot]"
Write-Host "Destination root: [$TargetRoot]"
Write-Host "React Native root: [$ReactNativeRoot]"
Write-Host "ReactCommon Override root: [$ReactCommonOverrideRoot]"

md -Force $TargetRoot

$patterns = $Extensions| ForEach-Object {"*.$_"}

# include headers
Copy-Item -Force -Recurse -Path $ReactWindowsRoot\include -Destination $TargetRoot\inc

# Microsoft.ReactNative.CXX project
Copy-Item -Force -Recurse -Path $ReactWindowsRoot\Microsoft.ReactNative.Cxx -Destination $TargetRoot\

# Copy native module spec files
Copy-Item -Force -Recurse -Path $ReactWindowsRoot\codegen -Destination $TargetRoot\inc

# Overwrite temporary ReactCommon files (since this script can runs on a different machine than where ReactCommon was built)
Copy-Item -Force -Recurse -Path $ReactCommonOverrideRoot\* -Destination $ReactNativeRoot\ReactCommon\

# Microsoft.ReactNative.CXX project JSI files
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\jsi\jsi\decorator.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\jsi\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\jsi\jsi\instrumentation.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\jsi\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\jsi\jsi\jsi.cpp -Destination $TargetRoot\Microsoft.ReactNative.Cxx\jsi\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\jsi\jsi\jsi.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\jsi\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\jsi\jsi\jsi-inl.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\jsi\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\jsi\jsi\threadsafe.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\jsi\

# Microsoft.ReactNative.CXX project TurboModule files
New-Item -ItemType Directory -Path $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon -Force
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\callinvoker\ReactCommon\CallInvoker.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\react\bridging\CallbackWrapper.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\react\bridging\LongLivedObject.cpp -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\react\bridging\LongLivedObject.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\react\nativemodule\core\ReactCommon\TurboModule.cpp -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\react\nativemodule\core\ReactCommon\TurboModule.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\react\nativemodule\core\ReactCommon\TurboModuleUtils.cpp -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\
Copy-Item -Force -Path $ReactNativeRoot\ReactCommon\react\nativemodule\core\ReactCommon\TurboModuleUtils.h -Destination $TargetRoot\Microsoft.ReactNative.Cxx\ReactCommon\

# NUSPEC
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\*.nuspec -Destination $TargetRoot

#Copy StripAdditionalPlatformsFromNuspec.ps1 for use by publish task
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\StripAdditionalPlatformsFromNuspec.ps1 -Destination $TargetRoot

# Microsoft.ReactNative.VersionCheck.targets
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\Microsoft.ReactNative.VersionCheck.targets -Destination $TargetRoot

# Microsoft.ReactNative.targets
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\Microsoft.ReactNative.targets -Destination $TargetRoot

# Microsoft.ReactNative.Cxx.targets
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\Microsoft.ReactNative.Cxx.targets -Destination $TargetRoot

# Microsoft.ReactNative.Managed.targets
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\Microsoft.ReactNative.Managed.targets -Destination $TargetRoot

# Microsoft.ReactNative.Managed.CodeGen.targets
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\Microsoft.ReactNative.Managed.CodeGen.targets -Destination $TargetRoot

