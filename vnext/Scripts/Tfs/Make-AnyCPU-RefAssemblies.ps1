# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

param(
	[string] $BuildRoot = "$SourceRoot\vnext\target",
	[string] $TargetRoot = "$SourceRoot\vnext\target"
)


# Fix ref Assemblies to be AnyCpu

# The reason for doing this here is that csc maintains the architecture of the ref assembly to match the implementation. It doesn't 
# automatically tag the ref as 'anycpu'. We use the ref assembly as the 'ref' in our nuget package. When the csc compiler
# compiles for other specific platforms like Arm, the compiler will fail if a ref assembly is marked as x64.
# The fix is to ensure that the 'ref' in the nuget package is marked as 'AnyCPU'.

# There unfortunately is no flag to csc, and no single tool to do this. CorFlags.exe does not have support to change this...
# This is a lot more straight-forward than trying to create an AnyCpu version of this assembly, update the solution, tests and add
# another platform to the CI matrix, not to mention the steps needed to ensure AnyCpu ran for the x64 build to test the nuget package
# in PR validation....

# Therefore this step will simply ildasm and ilasm the reference assembly and store it as which strips the bitness and generates anycpu msil.
# These reference assemblies are just regular assemblies but with empty bodies

ForEach ($refFolder in (Get-ChildItem -Path  $BuildRoot -Recurse -Include "ref"))
{ 
	ForEach ($refAsm in Get-ChildItem -Path $refFolder -Recurse  -Include "*.dll" ) 
	{
		$outputFolder=([System.IO.Path]::GetDirectoryName($refAsm)).Replace($BuildRoot, $TargetRoot) + ".anycpu";
		$outputAsm = [System.IO.Path]::Combine($outputFolder, [System.IO.Path]::GetFileName($refAsm));
		$outputIl = [System.IO.Path]::ChangeExtension($outputAsm, ".il");

		mkdir $outputFolder -ErrorAction SilentlyContinue
		& "${Env:ProgramFiles(x86)}\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.8 Tools\ildasm.exe" $refAsm /ALL "/OUT=$outputIl"
		Write-Host  $refAsm -> $outputIl
		& "$env:WINDIR\Microsoft.NET\Framework64\v4.0.30319\ilasm.exe" $outputIl /DLL /QUIET
		Write-Host  $outputIl -> $outputAsm
	} 
}
