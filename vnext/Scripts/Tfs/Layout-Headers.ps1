# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

param(
	[string] $TargetRoot = "$SourceRoot\vnext\target",
	[string] $SourceRoot = ($PSScriptRoot | Split-Path | Split-Path | Split-Path),
	[System.IO.DirectoryInfo] $ReactWindowsRoot = "$SourceRoot\vnext",
	[System.IO.DirectoryInfo] $ReactNativeRoot = "$SourceRoot\node_modules\react-native",
	[string] $FollyVersion = '2019.09.30.00',
	[System.IO.DirectoryInfo] $FollyRoot = "$SourceRoot\node_modules\.folly\folly-${FollyVersion}",
	[string[]] $Extensions = ('h', 'hpp', 'def')
)

Write-Host "Source root: [$SourceRoot]"
Write-Host "Destination root: [$TargetRoot]"

$patterns = $Extensions| ForEach-Object {"*.$_"}

# ReactCommon headers
Get-ChildItem -Path $ReactNativeRoot\ReactCommon -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactNativeRoot\ReactCommon\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\$(Split-Path $_) -Force) `
	-Force
}

# Folly headers
Get-ChildItem -Path $FollyRoot -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $FollyRoot\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\folly\$(Split-Path $_) -Force) `
	-Force
}

# stubs headers
Get-ChildItem -Path $ReactWindowsRoot\stubs -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\stubs\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\stubs\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Core headers
Get-ChildItem -Path $ReactWindowsRoot\ReactWindowsCore -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\ReactWindowsCore\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\ReactWindowsCore\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Desktop headers
Get-ChildItem -Path $ReactWindowsRoot\Desktop -Name -Recurse -Include '*.h','*.hpp' | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\Desktop\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\ReactWin32\$(Split-Path $_) -Force) `
	-Force
}
# React.Windows.Desktop DEFs
Get-ChildItem -Path $ReactWindowsRoot\Desktop.DLL -Recurse -Include '*.def' | ForEach-Object { Copy-Item `
	-Path        $_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\ -Force) `
	-Force
}

# React.Windows.ReactUWP headers
Get-ChildItem -Path $ReactWindowsRoot\ReactUWP -Name -Recurse -Include '*.h','*.hpp' | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\ReactUWP\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\ReactUWP\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Test headers
Get-ChildItem -Path $ReactWindowsRoot\Test -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\Test\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\Test\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Test DLL DEF files
Get-ChildItem -Path $ReactWindowsRoot\Desktop.Test.DLL -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\Desktop.Test.DLL\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\$(Split-Path $_) -Force) `
	-Force
}

# include headers
Copy-Item -Force -Recurse -Path $ReactWindowsRoot\include -Destination $TargetRoot\inc

# NUSPEC
Copy-Item -Force -Path $ReactWindowsRoot\Scripts\React*.nuspec -Destination $TargetRoot
