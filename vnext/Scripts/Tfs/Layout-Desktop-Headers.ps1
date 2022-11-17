# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

param(
	[string] $SourceRoot = ($PSScriptRoot | Split-Path | Split-Path | Split-Path),
	[string] $TargetRoot = "$SourceRoot\vnext\target",
	[System.IO.DirectoryInfo] $ReactWindowsRoot = "$SourceRoot\vnext",
	[System.IO.DirectoryInfo] $ReactNativeRoot = "$SourceRoot\node_modules\react-native",	
	[string[]] $Extensions = ('h', 'hpp', 'def')
)

[xml]$props = gc $PSScriptRoot\..\..\Directory.Build.props
[string] $FollyVersion = $props.Project.PropertyGroup.FollyVersion;
$FollyVersion = $FollyVersion.Trim() # The extracted FollyVersion contains a space at the end that isn't actually present, issue #6216
$FollyRoot = "$SourceRoot\node_modules\.folly\folly-${FollyVersion}";
$FollyOverrideRoot = "$ReactWindowsRoot\Folly\TEMP_UntilFollyUpdate";

[string] $FmtVersion = $props.Project.PropertyGroup.FmtVersion;
$FmtVersion = $FmtVersion.Trim() # The extracted FmtVersion contains a space at the end that isn't actually present, issue #6216
$FmtRoot = "$SourceRoot\node_modules\.fmt\fmt-${FmtVersion}";

# Download Folly if running on a machine which hasn't run native build logic to acquire it
if (!(Test-Path $FollyRoot)) {
	Write-Host "Downloading Folly $FollyVersion"
	$FollyZip = "$SourceRoot\node_modules\.folly\folly-${FollyVersion}.zip"
	$FollyDest = "$SourceRoot\node_modules\.folly"

	New-Item $FollyRoot -ItemType Directory
	Invoke-RestMethod -Uri "https://github.com/facebook/folly/archive/v$FollyVersion.zip" -OutFile $FollyZip
	Expand-Archive -LiteralPath $FollyZip -DestinationPath $FollyRoot
}

# Download Fmt if running on a machine which hasn't run native build logic to acquire it
if (!(Test-Path $FmtRoot)) {
	Write-Host "Downloading Fmt $FmtVersion"
	$FmtZip = "$SourceRoot\node_modules\.fmt\fmt-${FmtVersion}.zip"
	$FmtDest = "$SourceRoot\node_modules\.fmt"

	New-Item $FmtRoot -ItemType Directory
	Invoke-RestMethod -Uri "https://github.com/fmtlib/fmt/archive/refs/tags/$FmtVersion.zip" -OutFile $FmtZip
	Expand-Archive -LiteralPath $FmtZip -DestinationPath $FmtDest
}

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

# Folly overrides
Get-ChildItem -Path $FollyOverrideRoot -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $FollyOverrideRoot\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\folly\folly\$(Split-Path $_) -Force) `
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

# Natvis files
Copy-Item -Force -Path $ReactWindowsRoot\Folly\Folly.natvis -Destination (New-Item -ItemType Directory $TargetRoot\natvis -Force)
