param(
  [String]$ExeName,
  [String]$DumpFilePath
)

if (!$ExeName -and !$DumpFilePath) {
  Write-Output "Either specify an exe name or a dump file path"
}
$isElevated = [bool](([System.Security.Principal.WindowsIdentity]::GetCurrent()).groups -match "S-1-5-32-544")

$localDumpsPath = "HKLM:\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\$ExeName.exe"

$crashDumpsFolder = "$env:TEMP\CrashDumps"

if (!(Test-Path $localDumpsPath)) {
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





$cdb = 'C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe'

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

if ($ExeName) {
  $r = Read-Host Please reproduce your crash scenario. Press Enter when done
  $DumpFilePath = gci $crashDumpsFolder\*.dmp | Sort-Object -Property LastWriteTime -Descending | Select-Object -First 1
}

Write-Host Analyzing crash dump $DumpFilePath
& $cdb -z $DumpFilePath -c ".lines; !analyze -v; .ecxr; kP; q" > analyze.txt

Write-Host "Written analysis to analyze.txt - Please paste its contents in the bug inside of a <details>...</details> tag."
Write-Host "You may also upload the file $DumpFilePath to your OneDrive or other cloud provider and share a link in the bug description."

start analyze.txt

function FileIssue {
  # We can't populate the !analyze output because the URL ends up being too long and GitHub rejects it. Following up internally but at least we can prepopulate the <details> tags etc.
  $title = "[Crash] ENTER TITLE HERE"
  $body = "ENTER YOUR ISSUE DESCRIPTION HERE

<details>

``````
  PASTE YOUR ANALYZE.TXT HERE
``````
</details>
"

  $bodyUrl = [uri]::EscapeUriString($body)
  start "https://github.com/microsoft/react-native-windows/issues/new?title=$title&body=$bodyUrl"
}

FileIssue