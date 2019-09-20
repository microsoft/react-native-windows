Invoke-WebRequest -Method Get `
	-Uri 'https://download.visualstudio.microsoft.com/download/pr/c4fef23e-cc45-4836-9544-70e213134bc8/1ee5717e9a1e05015756dff77eb27d554a79a6db91f2716d836df368381af9a1/vs_Enterprise.exe' `
	-OutFile ${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe

Invoke-WebRequest -Method Get `
	-Uri 'https://download.microsoft.com/download/8/3/4/834E83F6-C377-4DCE-A757-69A418B6C6DF/Collect.exe' `
	-OutFile ${env:System_DefaultWorkingDirectory}\Collect.exe

Write-Host "Current VC versions:"
Get-ChildItem "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\"

Start-Process `
	-FilePath "${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe" `
	-ArgumentList `
		"--installPath", 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise' , `
		"--wait", `
		'--quiet', `
		'--norestart', `
		'--add', 'Microsoft.VisualStudio.Component.VC.v141.x86.x64' `
	-Wait `
	-PassThru `
	-OutVariable returnCode

	Write-Host "return code: [$returnCode]"

# Should generate ${env:Temp}\vslogs.zip
Start-Process `
	-FilePath "${env:System_DefaultWorkingDirectory}\Collect.exe" `
	-Wait `
	-PassThru

mkdir -Force ${env:Build_ArtifactStagingDirectory}\vslogs
Copy-Item ${env:TEMP}\vslogs.zip ${env:Build_ArtifactStagingDirectory}\vslogs\

Write-Host "VC versions after installation:"
Get-ChildItem "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\"
