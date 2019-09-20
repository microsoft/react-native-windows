Invoke-WebRequest -Method Get `
	-Uri 'https://download.visualstudio.microsoft.com/download/pr/c4fef23e-cc45-4836-9544-70e213134bc8/1ee5717e9a1e05015756dff77eb27d554a79a6db91f2716d836df368381af9a1/vs_Enterprise.exe' `
	-OutFile ${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe

Get-ChildItem ${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe

& "${env:System_DefaultWorkingDirectory}\vs_Enterprise.exe" modify --installPath 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise' --wait --quiet --add Microsoft.VisualStudio.Component.VC.v141.x86.x64
