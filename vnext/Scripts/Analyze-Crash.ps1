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

# First, do a quick check to see if this is a /GS crash (STATUS_STACK_BUFFER_OVERRUN, 0xc0000409)
$gsCheckLogfile = "$env:TEMP\gs_check.log"
& $cdb -z "$DumpFilePath" -c ".lastevent; q" > $gsCheckLogfile 2>&1
$isGSCrash = $false
if (Test-Path $gsCheckLogfile) {
  $content = Get-Content $gsCheckLogfile -Raw
  if ($content -match "c0000409|STATUS_STACK_BUFFER_OVERRUN") {
    $isGSCrash = $true
    Write-Host -ForegroundColor Yellow "`n*** DETECTED /GS STACK BUFFER OVERRUN CRASH (0xc0000409) ***"
    Write-Host -ForegroundColor Yellow "This is a security-related crash that requires immediate attention.`n"
  }
  Remove-Item $gsCheckLogfile -ErrorAction SilentlyContinue
}

# Run analysis with !gs extension for /GS crashes
if ($isGSCrash) {
  Write-Host "Running detailed /GS crash analysis..."
  & $cdb -z "$DumpFilePath" -c ".lines; !analyze -v; !gs; .ecxr; kP; q" > $logfile
} else {
  & $cdb -z "$DumpFilePath" -c ".lines; !analyze -v; .ecxr; kP; q" > $logfile
}

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

# Check for /GS crash indicators in the analysis
$analysisContent = Get-Content $logfile -Raw
$gsClassification = "Unknown"

if ($analysisContent -match "GS_POSITIVE") {
  $gsClassification = "GS_POSITIVE (High-confidence stack buffer overrun)"
  Write-Host -ForegroundColor Red "`n*** HIGH-CONFIDENCE /GS CRASH DETECTED ***"
  Write-Host -ForegroundColor Red "Problem Class: GS_POSITIVE"
  Write-Host -ForegroundColor Red "This is a confirmed stack buffer overrun that MUST be fixed immediately."
  Write-Host -ForegroundColor Red "Per SDL policy, this must be analyzed and fixed within 60 days.`n"
} elseif ($analysisContent -match "GS_SUSPECT") {
  $gsClassification = "GS_SUSPECT (Suspected stack buffer overrun)"
  Write-Host -ForegroundColor Yellow "`n*** SUSPECTED /GS CRASH DETECTED ***"
  Write-Host -ForegroundColor Yellow "Problem Class: GS_SUSPECT"
  Write-Host -ForegroundColor Yellow "This is a suspected stack buffer overrun that should be investigated.`n"
} elseif ($analysisContent -match "GS_FALSE_POSITIVE") {
  $gsClassification = "GS_FALSE_POSITIVE (Can be safely ignored)"
  Write-Host -ForegroundColor Green "`n/GS False Positive Detected"
  Write-Host -ForegroundColor Green "Problem Class: GS_FALSE_POSITIVE - This can be safely ignored.`n"
} elseif ($isGSCrash) {
  Write-Host -ForegroundColor Yellow "`n/GS crash detected but classification not determined."
  Write-Host -ForegroundColor Yellow "Manual review of the crash dump is recommended.`n"
}

Get-Content $logfile | & "${env:SystemRoot}\System32\clip.exe"

Write-Host "
Written analysis to $PWD\$logfile
/GS Classification: $gsClassification

The contents have been copied to the clipboard.
If you wish to file a bug, please paste its contents in inside of a <details>...</details> tag.
You may also upload the file $DumpFilePath and associated .pdb symbol files to your OneDrive or other cloud provider and share a link in the bug description.

"

if ($gsClassification -eq "GS_POSITIVE (High-confidence stack buffer overrun)") {
  Write-Host -ForegroundColor Red @"

===============================================================================
SDL POLICY REQUIREMENT: HIGH-CONFIDENCE /GS CRASH
===============================================================================
Per Microsoft SDL policy, this crash MUST be:
1. Analyzed and triaged immediately
2. Fixed within 60 days of being reported to Watson
3. Reported as a security vulnerability if exploitable

Action Items:
- Review the !gs analysis output in $logfile
- Identify the buffer overrun location and root cause
- File a security bug if this is exploitable
- Implement and test a fix

For more information, see: https://aka.ms/watson
===============================================================================

"@
}


Write-Warning "Note: the contents of $logfile, crash dump, and symbol files, might contain personally identifiable information. Please carefully review these contents before sharing them."

start notepad.exe $logfile

function FileIssue {
  # We can't populate the !analyze output because the URL ends up being too long and GitHub rejects it. Following up internally but at least we can prepopulate the <details> tags etc.
  
  $issueTitle = "[Crash] ENTER TITLE HERE"
  $labels = "bug"
  
  if ($gsClassification -eq "GS_POSITIVE (High-confidence stack buffer overrun)") {
    $issueTitle = "[Security] /GS High-Confidence Stack Buffer Overrun"
    $labels = "bug,security"
  } elseif ($gsClassification -eq "GS_SUSPECT (Suspected stack buffer overrun)") {
    $issueTitle = "[Security] /GS Suspected Stack Buffer Overrun"
    $labels = "bug,security"
  }
  
  $title = [uri]::EscapeUriString($issueTitle)
  $body = [uri]::EscapeUriString("ENTER YOUR ISSUE DESCRIPTION HERE

/GS Classification: $gsClassification

<details>

``````
  PASTE YOUR ANALYSIS LOG FILE HERE
``````
</details>
")

  start "https://github.com/$Repo/issues/new?title=$title&body=$body&labels=$labels"
}

FileIssue