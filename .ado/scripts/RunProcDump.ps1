# RunProcDump.ps1 [Args]
# Ex: .\RunProcDump.ps1 @('-ma', '-e', '-x', 'C:\WER\UserDumps', 'RNTesterApp_8wekyb3d8bbwe!App')
# Ex: .\RunProcDump.ps1 @('-mm', '-i', 'C:\WER\UserDumps') -Verbose
# 
# This script downloads and runs the ProcDump tool with the given arguments.

param(
    [Parameter(Mandatory=$true)]
    [String[]]$ProcDumpArgs,
    [string]$ProcDumpInstallPath = "$env:TEMP\ProcDump",
    [string]$ProcDumpExe = "procdump64.exe",
    [string]$ProcDumpUrl = "https://download.sysinternals.com/files/Procdump.zip"
)

function Get-ProcDump64Path {
    Write-Verbose "Looking for $ProcDumpExe in path...";
    try {
        [string]$procDumpPath = (Get-Command $ProcDumpExe -ErrorAction Stop).Path;
        Write-Verbose "$ProcDumpExe already in path at $procDumpPath";
        return $procDumpPath;
    } catch {
        Write-Debug $_
    }

    [string]$procDumpZipPath = "$env:TEMP\Procdump.zip";

    Write-Verbose "$ProcDumpExe not in path, downloading zip to $procDumpZipPath...";
    Invoke-WebRequest -UseBasicParsing $ProcDumpUrl -OutFile $procDumpZipPath;

    [string]$procDumpOutPath = "$ProcDumpInstallPath";
    Write-Verbose "Unzipping archive...";
    Expand-Archive $procDumpZipPath -DestinationPath $procDumpOutPath -Force;

    [string]$procDumpPath = Join-Path $procDumpOutPath $ProcDumpExe;
    if (Test-Path $procDumpPath) {
        Write-Verbose "$ProcDumpExe installed to $procDumpPath";
        return $procDumpPath;
    }
    
    throw "Unable to find and/or install $ProcDumpExe";
}

try {
    Write-Host "Looking for $ProcDumpExe...";
    [string]$procDumpPath = Get-ProcDump64Path;

    Write-Host "Running ProcDump...";
    & $procDumpPath -accepteula $ProcDumpArgs;

    exit 0;
} catch {
    Write-Debug $_
    exit 1;
}
