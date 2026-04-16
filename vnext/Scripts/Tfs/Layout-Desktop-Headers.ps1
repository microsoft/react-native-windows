# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

param(
	[string] $SourceRoot = ($PSScriptRoot | Split-Path | Split-Path | Split-Path),
	[string] $TargetRoot = "$SourceRoot\vnext\target",
	[System.IO.DirectoryInfo] $ReactWindowsRoot = "$SourceRoot\vnext",
	[System.IO.DirectoryInfo] $ReactNativeRoot = "$SourceRoot\node_modules\react-native",
	[string[]] $Extensions = ('h', 'hpp', 'def')
)

# Folly and fmt source are committed in vnext/external/ — no download needed
$FollyRoot = "$ReactWindowsRoot\external\folly";
$FmtRoot = "$ReactWindowsRoot\external\fmt";

Write-Host "Source root: [$SourceRoot]"
Write-Host "Destination root: [$TargetRoot]"
Write-Host "React Native root: [$ReactNativeRoot]"

md -Force $TargetRoot

$patterns = $Extensions| ForEach-Object {"*.$_"}

# ReactCommon headers
Get-ChildItem -Path $ReactNativeRoot\ReactCommon -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactNativeRoot\ReactCommon\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\$(Split-Path $_) -Force) `
	-Force
}

# Yoga headers
Get-ChildItem -Path $ReactNativeRoot\ReactCommon\yoga\yoga -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactNativeRoot\ReactCommon\yoga\yoga\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\Yoga\$(Split-Path $_) -Force) `
	-Force
}

# Folly headers
Get-ChildItem -Path $FollyRoot -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $FollyRoot\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\folly\$(Split-Path $_) -Force) `
	-Force
}

# Fmt headers
Get-ChildItem -Path $FmtRoot\include -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $FmtRoot\include\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\$(Split-Path $_) -Force) `
	-Force
}

# stubs headers
Get-ChildItem -Path $ReactWindowsRoot\stubs -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\stubs\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\stubs\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Core headers
Get-ChildItem -Path $ReactWindowsRoot\Shared -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $ReactWindowsRoot\Shared\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\Shared\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Desktop headers
Get-ChildItem -Path $ReactWindowsRoot\Desktop -Name -Recurse -Include '*.h','*.hpp','*.winmd' | ForEach-Object { Copy-Item `
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

# include headers
Copy-Item -Force -Recurse -Path $ReactWindowsRoot\include -Destination $TargetRoot\inc

# Natvis files
Copy-Item -Force -Path $ReactWindowsRoot\external\folly\Folly.natvis -Destination (New-Item -ItemType Directory $TargetRoot\natvis -Force)
