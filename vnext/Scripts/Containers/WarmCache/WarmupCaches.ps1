param(
    [string]$phase = ''
)

Set-StrictMode -version 2.0

$rootDrive='c:\'
$cacheRoot=Join-Path -Path $rootDrive -ChildPath 'cache'
$agentRoot=Join-Path -Path $rootDrive -ChildPath 'agent'
$warmupRoot=Join-Path -Path $rootDrive -ChildPath 'warmup'

if ($phase -eq '' -or $phase -eq 'prep') {
    New-Item -ItemType directory -Path $warmupRoot -Force | Out-null

    $yarnCache=Join-Path -Path $cacheRoot -ChildPath 'yarn'
    Write-Host 'Set yarn cache : ' + yarnCache
    New-Item -ItemType directory -Path  $yarnCache -Force | Out-null
    [System.Environment]::SetEnvironmentVariable('YARN_CACHE_DIR', $yarnCache, [System.EnvironmentVariableTarget]::Machine)
    yarn config set cache-folder $yarnCache --global

    $midgardCache=Join-Path -Path $cacheRoot -ChildPath 'midgard'
    Write-Host 'Set midgard-yarn cache : ' $midgardCache
    New-Item -ItemType directory -Path  $midgardCache -Force | Out-null
    [System.Environment]::SetEnvironmentVariable('MIDGARD_YARN_CACHE_DIR',$midgardCache, [System.EnvironmentVariableTarget]::Machine)

    $npmCache=Join-Path -Path $cacheRoot -ChildPath 'npm'
    Write-Host 'Prep npm cache : ' $npmCache
    New-Item -ItemType directory -Path $npmCache -Force | Out-null
    [System.Environment]::SetEnvironmentVariable('NPM_CACHE_LOCATION','$npmCache', [System.EnvironmentVariableTarget]::Machine)
    npm config set cache $npmCache --global

    $nugetCache=Join-Path -Path $cacheRoot -ChildPath 'nuget'
    Write-Host 'Set Cache (nuget) : ' $nugetCache
    $nugetExe = Join-Path -Path $warmupRoot  -ChildPath '\nuget.exe'
    New-Item -ItemType directory -Path $nugetCache -Force | Out-null
    [System.Environment]::SetEnvironmentVariable('NUGET_PACKAGES', $nugetCache, [System.EnvironmentVariableTarget]::Machine)
    Invoke-WebRequest -Uri https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -UseBasicParsing -OutFile $nugetExe | Out-null
    & $nugetExe config -set globalPackagesFolder=$nugetCache

    Write-Host 'Clone repo'
    New-Item -ItemType directory -Path $warmupRoot -Force | Out-Null
    cd $warmupRoot
    & "C:\Program Files\Git\cmd\git.exe" clone https://github.com/microsoft/react-native-windows.git $warmupRoot\react-native-windows
}

if ($phase -eq '' -or $phase -eq 'yarn') {
    pushd $warmupRoot\react-native-windows

    Write-Host 'Warmup yarn'
    yarn install

    popd
}

if ($phase -eq '' -or $phase -eq 'nuget') {
    pushd $warmupRoot\react-native-windows

    # warning: running nuget restore requires yarn install to have run.....
    Write-Host 'Warmup nuget'
    & $warmupRoot\nuget.exe restore vnext\Microsoft.ReactNative.sln
    & $warmupRoot\nuget.exe restore vnext\ReactWindows-Desktop.sln

    popd
}

if ($phase -eq '' -or $phase -eq 'midgard') {
    pushd $warmupRoot\react-native-windows

    $midgardCache=Join-Path -Path $cacheRoot -ChildPath 'midgard'
    Write-Host 'Cleanup old files to force populate midgard cache'
    & "C:\Program Files\Git\cmd\git.exe" clean -xdf
    & "C:\Program Files\Git\cmd\git.exe" reset --hard

    Write-Host 'Warmup midgard-yarn'
    & npx midgard-yarn@1.23.10 --network-concurrency 40 --frozen-lockfile --cache-folder $midgardCache
    popd
}

if ($phase -eq '' -or $phase -eq 'npx') {
    Write-Host 'Warmup npm/npx for template init'
    pushd $warmupRoot
    & npx react-native init npxWarmup
    popd
}

Write-Host 'Completed warmup'