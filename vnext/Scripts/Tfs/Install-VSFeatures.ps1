Invoke-WebRequest -Method Get `
	-Uri 'https://download.visualstudio.microsoft.com/download/pr/c4fef23e-cc45-4836-9544-70e213134bc8/1ee5717e9a1e05015756dff77eb27d554a79a6db91f2716d836df368381af9a1/vs_Enterprise.exe' `
	-OutFile ${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe

Get-ChildItem ${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe

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

Write-Host "VC versions after installation:"
Get-ChildItem "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\"
