# The 
$packageIdentity = "59b7e31b-a8fe-4fc7-85d6-a0a2475ad32d"
$taskName = "SampleHeadlessTask"

$packages = plmdebug /query
$packageFullName = ""

if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to launch PLMDebug. This is needed to query background tasks and trigger execution on demand."
    Write-Error "PLMdebug is either not present on this machine or it is not on the path."
    Write-Error "See this page for information to download plmdebug https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/plmdebug"
    exit -1
}


foreach ($line in $packages) {
    if ($line -and ($line.Contains($packageIdentity))) {
        $packageFullName = $line.Substring($line.IndexOf(':') + 2)
        break
    }
}

if ($packageFullName -eq "") {
    Write-Error "Failed to find package. Is your defult xbox set? xbconnect <ip> Is the app sideloaded?"
    exit -1
}

Write-Host -ForegroundColor Green "Found package full name $packageFullName"

$tasks = plmdebug /enumeratebgtasks $packageFullName
$taskGuid = ""

foreach ($line in $tasks) {
    if ($line -and ($line.Contains($taskName))) {
        $taskGuid = $line.Substring($line.IndexOf(':') + 2)
        break
    }
}

if ($taskGuid -eq "") {
    Write-Error "Failed to find task Guid. Is the task Registered? Need to run the app and make sure you have called register on the BackgroundTaskBuilder object."
    exit -1
}

Write-Host -ForegroundColor Green "Found Task Guid $taskGuid"

plmdebug /activatebgtask $taskGuid

if ($LASTEXITCODE -eq 0) {
    Write-Host -ForegroundColor Green "Success! $taskName has been activated."
}
else {
    Write-Error "Failed to execute background task"
}