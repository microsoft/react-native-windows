param(
  [String]$ExeName,
  [String]$DumpFilePath,
  [String]$Repo = "microsoft/react-native-windows"
)

$logfile = 'analyze.log'

if (!$ExeName -and !$DumpFilePath) {
  Write-Output "Either specify an exe name or a dump file path"
}
$isElevated = [bool](([System.Security.Principal.WindowsIdentity]::GetCurrent()).groups -match "S-1-5-32-544")

$localDumpsPath = "HKLM:\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\$ExeName.exe"

$crashDumpsFolder = "$env:TEMP\CrashDumps"

if ($ExeName -and !(Test-Path $localDumpsPath)) {
  if (!$isElevated) {
    Write-Output "This script must run elevated to set up crash dump collection. Exiting.";
    exit 1;
  }

  Write-Host Configuring $ExeName to collect heap dumps on crash at $crashDumpsFolder

  if (!(Test-Path $crashDumpsFolder)) {
    mkdir $crashDumpsFolder
  }

  New-Item $localDumpsPath -Force | Out-Null
  New-ItemProperty $localDumpsPath -Name DumpFolder -PropertyType String -Value $crashDumpsFolder | Out-Null
  New-ItemProperty $localDumpsPath -Name DumpType -PropertyType DWord -Value 2 | Out-Null
}


$cdb = "${env:ProgramFiles(x86)}\Windows Kits\10\Debuggers\x64\cdb.exe"

if (!(Test-Path $cdb)) {
  if (!$isElevated) {
    Write-Output "This script must run elevated to install the required debuggers. Exiting.";
    exit 1;
  }

  Write-Host "Downloading Windows SDK"
  # download Windows SDK 
  $installerUrl = 'https://go.microsoft.com/fwlink/?linkid=2173743'
  $winsdksetup = "$env:TEMP\winsdksetup.exe"

  $ProgressPreference = 'Ignore';
  Invoke-WebRequest -UseBasicParsing $installerUrl -OutFile $winsdksetup


  Write-Host "Installing debuggers"
  # run the installer to get the debugger
  $wss=Start-Process $winsdksetup -ArgumentList @('/features', 'OptionId.WindowsDesktopDebuggers', '/q') -PassThru
  $wss.WaitForExit()
}

if (!(Test-Path $cdb)) {
  Write-Output "Failed to find cdb.exe in the Windows SDK Debuggers folder. Try to manually install the debuggers from the Windows SDK setup. Exiting."
  exit 2;
}

if ($ExeName) {
  $r = Read-Host Please reproduce your crash scenario. Press Enter when done
  $DumpFilePath = gci $crashDumpsFolder\*.dmp | Sort-Object -Property LastWriteTime -Descending | Select-Object -First 1
}

Write-Host Analyzing crash dump $DumpFilePath
& $cdb -z "$DumpFilePath" -c ".lines; !analyze -v; .ecxr; kP; q" > $logfile

if ($LASTEXITCODE -ne 0) {
  Write-Output "Failed to analyze the crash dump. Exiting"
  if (Test-Path $logfile) {
    gc $logfile
  }
  exit 3
}

if (!(Test-Path $logfile)) {
  Write-Output "Couldn't find analysis log file $logfile. Exiting"
  exit 4
}

Get-Content $logfile | & "${env:SystemRoot}\System32\clip.exe"

Write-Host "
Written analysis to $PWD\$logfile

The contents have been copied to the clipboard.
If you wish to file a bug, please paste its contents in inside of a <details>...</details> tag.
You may also upload the file $DumpFilePath and associated .pdb symbol files to your OneDrive or other cloud provider and share a link in the bug description.

"

Write-Warning "Note: the contents of $logfile, crash dump, and symbol files, might contain personally identifiable information. Please carefully review these contents before sharing them."

start notepad.exe $logfile

function FileIssue {
  # We can't populate the !analyze output because the URL ends up being too long and GitHub rejects it. Following up internally but at least we can prepopulate the <details> tags etc.
  $title = [uri]::EscapeUriString("[Crash] ENTER TITLE HERE")
  $body = [uri]::EscapeUriString("ENTER YOUR ISSUE DESCRIPTION HERE

<details>

``````
  PASTE YOUR ANALYSIS LOG FILE HERE
``````
</details>
")

  start "https://github.com/$Repo/issues/new?title=$title&body=$body&labels=bug"
}

FileIssue