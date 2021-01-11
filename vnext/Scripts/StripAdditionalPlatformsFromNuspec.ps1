# For PR builds we dont want to have to build _ALL_ the platforms just to get a NuGet and test one platform.
# this script filters the files required by the NuGet, so we can produce a test NuGet on a single platform
[CmdletBinding()]
param (
    [string]$nuspec = "$PSScriptRoot\Microsoft.ReactNative.nuspec",
    [string]$outfile = "$PSScriptRoot\Microsoft.ReactNative.PR.nuspec",
    [string[]]$slices = @("x64.Release"),
    [boolean]$preferRelease = $false
)

# Allow write-debug to work in CI without prompting and breaking there.
If ($PSBoundParameters['Debug']) {
    $DebugPreference = 'Continue'
}

Write-Output "Creating filtered version of : $nuspec"
$xml = [xml](gc $nuspec)

$allSlices = @("x64.Release", "x64.Debug", "x86.Release", "x86.Debug", "ARM.Release", "ARM.Debug", "ARM64.Release", "ARM64.Debug")

$nodesToRemove = @();

# Validate slices args are all valid slices
foreach($s in $slices) {
    if (!$allSlices.Contains($s)) {
        Write-Output "Invalid slice '$s'";
        Exit 1;
    }
}

$filesSection = $xml.package.files;

foreach($file in $filesSection.ChildNodes) {
    if ($file.NodeType -eq "Comment") {
        continue;
    }

    Write-Debug "Testing file: $($file.src)"
    foreach($slice in $allSlices) {
        $sliceInfo = $slice.split('.');
        $platform = $sliceInfo[0];
        $flavor = $sliceInfo[1];

        if ($file.src.Contains("\$platform\$flavor\")) {
            Write-Debug "     Matched to slice: $slice"
            if (!$slices.Contains("$platform.$flavor")) {
                Write-Debug "          remove due to not in slice"
                $nodesToRemove += $file
                break;
            }

            if ($preferRelease -and $flavor -eq "Debug" -and $slices.Contains("$platform.Release")) {
                Write-Debug "          remove because it is debug and there is a release version requested"
                # We are checking for the debug version. There is a release version and we prefer release bits
                $nodesToRemove += $file
                break;
            }

        }
    }
}

foreach ($node in $nodesToRemove) {
    Write-Debug "Removing file $($node.src)";
    $x = $filesSection.RemoveChild($node);
}

$xml.save($outfile);
