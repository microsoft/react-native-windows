# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

param(
	[string] $SourceRoot = ($PSScriptRoot | Split-Path | Split-Path | Split-Path),
	[string] $TargetRoot = "$SourceRoot\ReactWindows\target",
	[string[]] $Extensions = ('h', 'hpp')
)

Write-Host "Source root: [$SourceRoot]"
Write-Host "Destination root: [$TargetRoot]"

$patterns = $Extensions| ForEach-Object {"*.$_"}

# ReactCommon headers
Get-ChildItem -Path $SourceRoot\ReactCommon -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $SourceRoot\ReactCommon\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\$(Split-Path $_) -Force) `
	-Force
}

# Folly headers
Get-ChildItem -Path $SourceRoot\Folly -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $SourceRoot\Folly\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\$(Split-Path $_) -Force) `
	-Force
}

# stubs headers
Get-ChildItem -Path $SourceRoot\ReactWindows\stubs -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $SourceRoot\ReactWindows\stubs\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\stubs\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Core headers
Get-ChildItem -Path $SourceRoot\ReactWindows\ReactWindowsCore -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $SourceRoot\ReactWindows\ReactWindowsCore\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\Core\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.Desktop headers
Get-ChildItem -Path $SourceRoot\ReactWindows\Desktop -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $SourceRoot\ReactWindows\Desktop\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\Desktop\$(Split-Path $_) -Force) `
	-Force
}

# React.Windows.ReactUWP headers
Get-ChildItem -Path $SourceRoot\ReactWindows\ReactUWP -Name -Recurse -Include $patterns | ForEach-Object { Copy-Item `
	-Path        $SourceRoot\ReactWindows\ReactUWP\$_ `
	-Destination (New-Item -ItemType Directory $TargetRoot\inc\ReactUWP\$(Split-Path $_) -Force) `
	-Force
}

# include headers
Copy-Item -Force -Recurse -Path $SourceRoot\ReactWindows\include -Destination $TargetRoot\inc

# NUSPEC
Copy-Item -Force -Path $SourceRoot\ReactWindows\ReactWindows.nuspec -Destination $TargetRoot
