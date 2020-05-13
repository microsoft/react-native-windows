# Troubleshoot RNW dependencies
param([switch]$Install = $false, [switch]$NoPrompt = $false)
$vsWorkloads = @( 'Microsoft.Component.MSBuild', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', 'Microsoft.VisualStudio.ComponentGroup.UWP.Support');

$v = [System.Environment]::OSVersion.Version;
$drive = (Resolve-Path $PSCommandPath).Drive

function CheckVS {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (!(Test-Path $vsWhere)) {
        return $false;
    }
    $output = & $vsWhere -version 16 -requires $vsWorkloads -property productPath
    return Test-Path $output;
}

function InstallVS {
    $installerPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer";
    $vsWhere = "$installerPath\vswhere.exe"
    if (!(Test-Path $vsWhere)) {
        # No VSWhere / VS_Installer
        & choco install -y visualstudio2019community
    }
    $channelId = & $vsWhere -version 16 -property channelId
    $productId = & $vsWhere -version 16 -property productId
    $vsInstaller = "$installerPath\vs_installer.exe"
    $addWorkloads = $vsWorkloads | % { '--add', $_ };
    Start-Process -PassThru -Wait  -Path $vsInstaller -ArgumentList ("install --channelId $channelId --productId $productId $addWorkloads --quiet" -split ' ')

}

function CheckNode {
    try {
        $v = (Get-Command node).Version.Major
        return $v -eq 12 -or $v -eq 13
    } catch {
        return $false;
    }
}

function GetChocoPkgVersion{
    params([string]$packageId)
    [version]$version = (& choco list --local-only $packageId -r -e).Substring($packageId.Length + 1);
    return $version;
}

$requiredFreeSpaceGB = 15;

$requirements = @(
    @{
        Name = "Free space on $drive`: > $requiredFreeSpaceGB GB";
        Valid = $drive.Free/1GB -gt $requiredFreeSpaceGB
    },
    @{
        Name = 'Windows version > 10.0.16299.0';
        Valid = ($v.Major -eq 10 -and $v.Minor -eq 0 -and $v.Build -ge 16299)
    },
    @{
        Name = 'Developer mode is on';
        Valid = try { (Get-WindowsDeveloperLicense).IsValid } catch { $false }
    },
    @{
        Name = 'git';
        Valid = try { (Get-Command git.exe) -ne $null } catch { $false }
    }
    @{
        Name = 'Choco';
        Valid = try { (Get-Command choco) -ne $null } catch { $false };
        Install = {
            [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; 
            iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
        };
    },
    @{
        Name = 'VS 2019 with UWP and Desktop/C++';
        Valid = CheckVS
    },
    @{
        Name = 'NodeJS 12 or 13 installed';
        Valid = CheckNode
        Install = { choco install -y nodejs.install --version=12.9.1 }
    },
    @{
        Name = 'Chrome';
        Valid = try { ((Get-Item (Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\chrome.exe').'(Default)').VersionInfo).ProductMajorPart
        } catch { $false } ;
        Install = { choco install -y GoogleChrome }
    },
    @{
        Name = 'Yarn';
        Valid = try { (Get-Command yarn) -ne $null } catch { $false };
        Install = { choco install -y yarn }
    },
    @{
        Name = 'Appium';
        Valid = (Test-Path "${env:ProgramFiles}\Appium\Appium.exe");
        Install = { choco install -y Appium-desktop }
    },
    @{
        Name = 'WinAppDriver';
        Valid = (Test-Path "${env:ProgramFiles(x86)}\Windows Application Driver\WinAppDriver.exe");
        Install = { choco install -y WinAppDriver }
    }

    );

function IsElevated {
    return [bool](([System.Security.Principal.WindowsIdentity]::GetCurrent()).groups -match "S-1-5-32-544");
}

if (!(IsElevated)) {
    Write-Output "rnw-dependencies - this script must run elevated. Exiting.";
    return;
}

$NeedsRerun = $false
foreach ($req in $requirements)
{
    if (!($req.Valid)) {
        Write-Output "Requirement failed: $($req.Name)"
        if ($req.Install) {
            if ($Install -or (!$NoPrompt -and (Read-Host "Do you want to install? ").ToUpperInvariant() -eq 'Y')) {
                Invoke-Command $req.Install -ErrorAction Stop
                if ($LASTEXITCODE -ne 0) { throw "Last exit code was non-zero: $LASTEXITCODE"; }
            } else {
                $NeedsRerun = $true;
            }
        } else {
            $NeedsRerun = $true;
        }
    }
}

if ($NeedsRerun) {
    Write-Output "Some dependencies are not met. Re-run with -Install to install them.";
    exit 1;
} else {
    exit 0;
}
