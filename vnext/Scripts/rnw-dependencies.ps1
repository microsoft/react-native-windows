# Troubleshoot RNW dependencies
param(
    [switch]$Install = $false, 
    [switch]$NoPrompt = $false, 
    [switch]$Clone = $false, 

    [Parameter(ValueFromRemainingArguments)]
    [ValidateSet('appDev', 'rnwDev', 'buildLab', 'vs2019', 'clone')]
    [String[]]$Tags = @('appDev')
)

# Create a set to handle with case insensitivy of the tags
$tagsToInclude = New-Object System.Collections.Generic.HashSet[string]([System.StringComparer]::OrdinalIgnorecase)
foreach ($tag in $Tags) { $tagsToInclude.Add($tag) | Out-null }

# Convert legacy flags to tasks:
if ($Clone) {
    $tagsToInclude.Add('clone') | Out-null;
}

# Handle expansion of tasks
if ($tagsToInclude.Contains('buildLab')) {
    # The build lab needs the same steps as a react-native dev
    $tagsToInclude.Add('rnwDev') | Out-null;
}
if ($tagsToInclude.Contains('rnwDev')) {
    # A react-native dev needs the same as the default
    $tagsToInclude.Add('appDev') | Out-null;
}

$vsComponents = @('Microsoft.Component.MSBuild', 
    'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
    'Microsoft.VisualStudio.ComponentGroup.UWP.Support',
    'Microsoft.VisualStudio.ComponentGroup.UWP.VC');

$vsWorkloads = @('Microsoft.VisualStudio.Workload.ManagedDesktop',
    'Microsoft.VisualStudio.Workload.NativeDesktop',
    'Microsoft.VisualStudio.Workload.Universal');

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
    $output = & $vsWhere -version 16 -requires $vsComponents -property productPath
    $vsComponents | % {
        Write-Output "Checking VS component $_";
        & $vsWhere -version 16 -requires $_ -property productPath;
    }
    return ($output -ne $null) -and (Test-Path $output);
}

function InstallVS {
    $installerPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer";
    $vsWhere = "$installerPath\vswhere.exe"
    if (!(Test-Path $vsWhere)) {
        # No VSWhere / VS_Installer
        if ($Enterprise) {
            # The CI machines need the enterprise version of VS as that is what is hardcoded in all the scripts
            & choco install -y visualstudio2019enterprise
        } else {
            & choco install -y visualstudio2019community
        }
    }
    $channelId = & $vsWhere -version 16 -property channelId
    $productId = & $vsWhere -version 16 -property productId
    $vsInstaller = "$installerPath\vs_installer.exe"
    $addWorkloads = ($vsWorkloads + $vsComponents) | % { '--add', $_ };
    $p = Start-Process -PassThru -Wait  -FilePath $vsInstaller -ArgumentList ("modify --channelId $channelId --productId $productId $addWorkloads --quiet" -split ' ')
    return $p.ExitCode
}

function CheckNode {
    try {
        $v = (Get-Command node -ErrorAction Stop).Version.Major
        return $v -eq 12 -or $v -eq 13 -or $v -eq 14
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

function GetMsBuild64BitConfigFile{
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $msbExeConfigPath=& $vsWhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\amd64\MSBuild.exe.config
    return $msbExeConfigPath;
}

$requiredFreeSpaceGB = 15;

$requirements = @(
    @{
        Name = "Free space on $drive`: > $requiredFreeSpaceGB GB";
        Tags = @('appDev');
        Valid = $drive.Free/1GB -gt $requiredFreeSpaceGB;
        Optional = $true; # this requirement is fuzzy 
    },
    @{
        Name = "Installed memory >= 16 GB";
        Tags = @('appDev');
        Valid = (Get-WmiObject -Class win32_computersystem).TotalPhysicalMemory -ge 15GB;
        Optional = $true;
    },
    @{
        Name = 'Windows version > 10.0.16299.0';
        Tags = @('appDev');
        Valid = ($v.Major -eq 10 -and $v.Minor -eq 0 -and $v.Build -ge 16299);
    },
    @{
        Name = 'Developer mode is on';
        Tags = @('appDev');
        Valid = try { (Get-WindowsDeveloperLicense).IsValid } catch { $false };
        Install = { EnableDevMode };
    },
    @{
        Name = 'Long path support is enabled';
        Tags = @('appDev');
        Valid = try { (Get-ItemProperty HKLM:/SYSTEM/CurrentControlSet/Control/FileSystem -Name LongPathsEnabled).LongPathsEnabled -eq 1} catch { $false };
        Install = { Set-ItemProperty HKLM:/SYSTEM/CurrentControlSet/Control/FileSystem -Name LongPathsEnabled -Value 1 -Type DWord;  };
    },
    @{
        Name = 'Choco';
        Tags = @('appDev');
        Valid = try { (Get-Command choco -ErrorAction Stop) -ne $null } catch { $false };
        Install = {
            [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; 
            iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'));
        };
    },
    @{
        Name = 'git';
        Tags = @('appDev');
        Valid = try { (Get-Command git.exe -ErrorAction Stop) -ne $null } catch { $false };
        Install = { choco install git };
    },
    @{
        Name = 'VS 2019 with UWP and Desktop/C++';
        Tags = @('appDev', 'vs2019');
        Valid = CheckVS;
        Install = { InstallVS };
    },
    @{
        Name = 'NodeJS 12, 13 or 14 installed';
        Tags = @('appDev');
        Valid = CheckNode;
        Install = { choco install -y nodejs.install --version=12.9.1 };
    },
    @{
        Name = 'Chrome';
        Tags = @('appDev'); # For now this is still required. Edge has been added, but only when it is already running...
        Valid = try { ((Get-Item (Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\chrome.exe' -ErrorAction Stop).'(Default)').VersionInfo).ProductMajorPart
        } catch { $false } ;
        Install = { choco install -y GoogleChrome };
    },
    @{
        Name = 'Yarn';
        Tags = @('appDev');
        Valid = try { (Get-Command yarn -ErrorAction Stop) -ne $null } catch { $false };
        Install = { choco install -y yarn };
    },
    @{
        Name = 'Appium';
        Tags = @('rnwDev');
        Valid = (Test-Path "${env:ProgramFiles}\Appium\Appium.exe");
        Install = { choco install -y Appium-desktop };
        Optional = $true;
    },
    @{
        Name = 'WinAppDriver';
        Tags = @('rnwDev');
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
        Tags = @('rnwDev');
        Valid = (cmd "/c assoc .binlog 2>nul" )  -ne $null;
        Install = {
            choco install -y msbuild-structured-log-viewer;
            $slv = gci ${env:LocalAppData}\MSBuildStructuredLogViewer\StructuredLogViewer.exe -Recurse | select FullName | Sort-Object -Property FullName -Descending | Select-Object -First 1
            cmd /c "assoc .binlog=MSBuildLog >nul";
            cmd /c "ftype MSBuildLog=$($slv.FullName) %1 >nul";
         };
         Optional = $true;
    },
    @{
        # The 64-bit version of MsBuild does not support long paths. A temp fix for v16 is: https://github.com/microsoft/msbuild/issues/5331
        Name = "MSBuild 64-bit Long Path Support"
        Tags = @('buildLab');
        Valid = try { 
            [System.IO.File]::ReadAllText( (GetMsBuild64BitConfigFile) ).Contains("Switch.System.Security.Cryptography.UseLegacyFipsThrow=false;Switch.System.IO.UseLegacyPathHandling=false;Switch.System.IO.BlockLongPaths=false") 
            } catch { $false };
        Install = {
            [ xml ]$msbExeConfig = Get-Content -Path (GetMsBuild64BitConfigFile)
            $msbExeConfig.configuration.runtime.AppContextSwitchOverrides.SetAttribute("value", "Switch.System.Security.Cryptography.UseLegacyFipsThrow=false;Switch.System.IO.UseLegacyPathHandling=false;Switch.System.IO.BlockLongPaths=false")
            $msbExeConfig.Save( (GetMsBuild64BitConfigFile) )
        };
        Optional = $true
    },
    @{
        # Install the Windows ADK (Assessment and Deployment Kit) to install the wpt (Windows Performance Toolkit) so we can use wpr (Windows Performance Recorder) for performance analysis
        Name = 'Windows ADK';
        Tags = @('buildLab');
        Valid = (Test-Path "${env:ProgramFiles(x86)}\Windows Kits\10\Windows Performance Toolkit\wpr.exe");
        Install = { choco install -y windows-adk };
    },
    @{
        Name = "React-Native-Windows clone"
        Tags = @('clone')
        Valid = try { 
            Test-Path -Path react-native-windows
            } catch { $false };
        Install = {
            & "${env:ProgramFiles}\Git\cmd\git.exe" clone https://github.com/microsoft/react-native-windows.git
        };
        Optional = $true
    }
);

function IsElevated {
    return [bool](([System.Security.Principal.WindowsIdentity]::GetCurrent()).groups -match "S-1-5-32-544");
}

if (!(IsElevated)) {
    Write-Output "rnw-dependencies - this script must run elevated. Exiting.";
    return;
}

$NeedsRerun = 0;
$Installed = 0;
$filteredRequirements = New-Object System.Collections.Generic.List[object]
foreach ($req in $requirements) 
{
    foreach ($tag in $req.Tags) 
    {
        if ($tagsToInclude.Contains($tag))
        {
            $filteredRequirements.Add($req);
            break;
        }
    }
}

foreach ($req in $filteredRequirements)
{
    Write-Host -NoNewline "Checking $($req.Name)    ";
    if (!($req.Valid)) {
        if ($req.Optional) {
            Write-Host -ForegroundColor Yellow " Failed (warn)".PadLeft(50 - $req.Name.Length);
        }
        else {
            Write-Host -ForegroundColor Red " Failed".PadLeft(50 - $req.Name.Length);
        }
        if ($req.Install) {
            if ($Install -or (!$NoPrompt -and (Read-Host "Do you want to install? [y/N]").ToUpperInvariant() -eq 'Y')) {
                $LASTEXITCODE = 0;
                Invoke-Command $req.Install -ErrorAction Stop;
                if ($LASTEXITCODE -ne 0) { throw "Last exit code was non-zero: $LASTEXITCODE"; }
                else { $Installed++; }
            } else {
                $NeedsRerun += !($req.Optional); # don't let failures from optional components fail the script 
            }
        } else {
            $NeedsRerun += !($req.Optional);
        }
    } else {
        Write-Host -ForegroundColor Green " OK".PadLeft(50 - $req.Name.Length);
    }
}


if ($Installed -ne 0) {
    Write-Output "Installed $Installed dependencies. You may need to close this window for changes to take effect."
}

if ($NeedsRerun -ne 0) {
    Write-Error "Some dependencies are not met. Re-run with -Install to install them.";
    if (!$NoPrompt) {
        [System.Console]::ReadKey();
    }
    throw;
} else {
    Write-Output "All mandatory requirements met";
    exit 0;
}

