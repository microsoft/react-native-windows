# Troubleshoot RNW dependencies
param([switch]$Install = $false, [switch]$NoPrompt = $false, [switch]$Clone = $false)
$vsWorkloads = @( 'Microsoft.Component.MSBuild', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', 'Microsoft.VisualStudio.ComponentGroup.UWP.Support', 'Microsoft.VisualStudio.Workload.ManagedDesktop', 'Microsoft.VisualStudio.Workload.NativeDesktop', 'Microsoft.VisualStudio.Workload.Universal', 'Microsoft.VisualStudio.ComponentGroup.UWP.VC');

$v = [System.Environment]::OSVersion.Version;
if ($env:Agent_BuildDirectory) {
    $drive = (Resolve-Path $env:Agent_BuildDirectory).Drive
} else {
    if ($PSCommandPath) {
        $drive = (Resolve-Path $PSCommandPath).Drive
    } else {
        $drive = (Resolve-Path $env:SystemDrive).Drive
    }
}

function CheckVS {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (!(Test-Path $vsWhere)) {
        return $false;
    }
    $output = & $vsWhere -version 16 -requires $vsWorkloads -property productPath
    return ($output -ne $null) -and (Test-Path $output);
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
    $p = Start-Process -PassThru -Wait  -FilePath $vsInstaller -ArgumentList ("modify --channelId $channelId --productId $productId $addWorkloads --quiet" -split ' ')
    return $p.ExitCode
}

function CheckNode {
    try {
        $v = (Get-Command node -ErrorAction Stop).Version.Major
        return $v -eq 12 -or $v -eq 13
    } catch {
        return $false;
    }
}

function EnableDevmode {
    $RegistryKeyPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock"
    
    if (-not(Test-Path -Path $RegistryKeyPath)) {
        New-Item -Path $RegistryKeyPath -ItemType Directory -Force
    }

    $value = get-ItemProperty -Path $RegistryKeyPath -Name AllowDevelopmentWithoutDevLicense -ErrorAction SilentlyContinue
    if (($value -eq $null) -or ($value.AllowDevelopmentWithoutDevLicense -ne 1)) {
        Set-ItemProperty -Path $RegistryKeyPath -Name AllowDevelopmentWithoutDevLicense -Value 1 -ErrorAction Stop
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
        Valid = $drive.Free/1GB -gt $requiredFreeSpaceGB;
        Optional = $true; # this requirement is fuzzy 
    },
    @{
        Name = 'Windows version > 10.0.16299.0';
        Valid = ($v.Major -eq 10 -and $v.Minor -eq 0 -and $v.Build -ge 16299);
    },
    @{
        Name = 'Developer mode is on';
        Valid = try { (Get-WindowsDeveloperLicense).IsValid } catch { $false };
        Install = { EnableDevMode };
    },
    @{
        Name = 'Long path support is enabled';
        Valid = try { (Get-ItemProperty HKLM:/SYSTEM/CurrentControlSet/Control/FileSystem -Name LongPathsEnabled).LongPathsEnabled -eq 1} catch { $false };
        Install = { Set-ItemProperty HKLM:/SYSTEM/CurrentControlSet/Control/FileSystem -Name LongPathsEnabled -Value 1 -Type DWord;  };
    },
    @{
        Name = 'Choco';
        Valid = try { (Get-Command choco -ErrorAction Stop) -ne $null } catch { $false };
        Install = {
            [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; 
            iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'));
        };
    },
    @{
        Name = 'git';
        Valid = try { (Get-Command git.exe -ErrorAction Stop) -ne $null } catch { $false };
        Install = { choco install git };
    },
    @{
        Name = 'VS 2019 with UWP and Desktop/C++';
        Valid = CheckVS;
        Install = { InstallVS };
    },
    @{
        Name = 'NodeJS 12 or 13 installed';
        Valid = CheckNode;
        Install = { choco install -y nodejs.install --version=12.9.1 };
    },
    @{
        Name = 'Chrome';
        Valid = try { ((Get-Item (Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\chrome.exe' -ErrorAction Stop).'(Default)').VersionInfo).ProductMajorPart
        } catch { $false } ;
        Install = { choco install -y GoogleChrome };
    },
    @{
        Name = 'Yarn';
        Valid = try { (Get-Command yarn -ErrorAction Stop) -ne $null } catch { $false };
        Install = { choco install -y yarn };
    },
    @{
        Name = 'Appium';
        Valid = (Test-Path "${env:ProgramFiles}\Appium\Appium.exe");
        Install = { choco install -y Appium-desktop };
        Optional = $true;
    },
    @{
        Name = 'WinAppDriver';
        Valid = (Test-Path "${env:ProgramFiles(x86)}\Windows Application Driver\WinAppDriver.exe");
        Install = { 
            # don't install from choco as we need an exact version match. appium-windows-driver checks the checksum of WAD.
            # See \node_modules\appium-windows-driver\build\lib\installer.js
            $ProgressPreference = 'Ignore';
            Invoke-WebRequest https://github.com/microsoft/WinAppDriver/releases/download/v1.1/WindowsApplicationDriver.msi  -OutFile $env:TEMP\WindowsApplicationDriver.msi 
            & $env:TEMP\WindowsApplicationDriver.msi /q
        };
        Optional = $true;
    },
    @{
        Name = "MSBuild Structured Log Viewer";
        Valid = (cmd "/c assoc .binlog 2>nul" )  -ne $null;
        Install = {
            choco install -y msbuild-structured-log-viewer;
            $slv = gci ${env:LocalAppData}\MSBuildStructuredLogViewer\app-2.0.152\StructuredLogViewer.exe -Recurse;
            cmd /c "assoc .binlog=MSBuildLog";
            cmd /c "ftype MSBuildLog=$($slv.FullName) %1";
         };
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
$Installed = 0
foreach ($req in $requirements)
{
    Write-Host -NoNewline "Checking $($req.Name)    ";
    if (!($req.Valid)) {
        if ($req.Optional) {
            Write-Host -ForegroundColor Yellow " Failed".PadLeft(50 - $req.Name.Length);
        }
        else {
            Write-Host -ForegroundColor Red " Failed".PadLeft(50 - $req.Name.Length);
        }
        if ($req.Install) {
            if ($Install -or (!$NoPrompt -and (Read-Host "Do you want to install? ").ToUpperInvariant() -eq 'Y')) {
                $LASTEXITCODE = 0;
                Invoke-Command $req.Install -ErrorAction Stop;
                if ($LASTEXITCODE -ne 0) { throw "Last exit code was non-zero: $LASTEXITCODE"; }
                else { $Installed++; }
            } elseif (!$req.Optional) {
                $NeedsRerun = $true;
            }
        } else {
            $NeedsRerun = !($req.Optional);
        }
    } else {
        Write-Host -ForegroundColor Green " OK".PadLeft(50 - $req.Name.Length);
    }
}

if ($NeedsRerun) {
    Write-Error "Some dependencies are not met. Re-run with -Install to install them.";
    if (!$NoPrompt) {
        [System.Console]::ReadKey();
    }
    throw;
} else {
    Write-Output "All mandatory requirements met";
}
if ($Installed -ne 0) {
    Write-Output "Installed $Installed dependencies. You may need to close this window for changes to take effect."
}

if ($Clone) {
    & "${env:ProgramFiles}\Git\cmd\git.exe" clone https://github.com/microsoft/react-native-windows.git
}