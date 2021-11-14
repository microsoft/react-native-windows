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

$allSlices = @("x64.Release", "x64.Debug", "x86.Release", "x86.Debug", "ARM64.Release", "ARM64.Debug")
$allTokens = @("x64", "x86", "ARM64", "Debug", "Release")
$nodesToRemove = @();

# Validate slices args are all valid slices
foreach($s in $slices) {
    if (!$allSlices.Contains($s)) {
        Write-Output "Invalid slice '$s'";
        Exit 1;
    }
}

$nodesSection = $xml.package.files;

foreach($node in $nodesSection.ChildNodes) {
    if ($node.NodeType -eq "Comment") {
        continue;
    }

    Write-Debug "Testing file: $($node.src)"
    foreach($slice in $allSlices) {
        $sliceInfo = $slice.split('.');
        $platform = $sliceInfo[0];
        $flavor = $sliceInfo[1];
        if ($node.src.Contains("\$platform\$flavor\")) {
            Write-Debug "     Matched to slice: $slice"
            if (!$slices.Contains("$platform.$flavor")) {
                Write-Debug "          remove due to not in slice"
                $nodesToRemove += $node
                break;
            }

            if ($preferRelease -and $flavor -eq "Debug" -and $slices.Contains("$platform.Release")) {
                Write-Debug "          remove because it is debug and there is a release version requested"
                # We are checking for the debug version. There is a release version and we prefer release bits
                $nodesToRemove += $node
                break;
            }
        }
    }

    # NuSpec files can parameterize part of the slice, so we also want to check
    # for partial matches that cannot be fulfilled by the passed slices.
    foreach($token in $allTokens) {
        if (!$nodesToRemove.Contains($node) -and $node.src.Contains("\$token\")) {
            $sliceExistsWithToken = $slices.Where({ $_.Contains($token) }).Length -gt 0
            if (!$sliceExistsWithToken) {
                Write-Debug "          remove due to not in slice"
                $nodesToRemove += $node
                break;
            }
        }
    }
}

foreach ($node in $nodesToRemove) {
    Write-Debug "Removing file $($node.src)";
    $x = $nodesSection.RemoveChild($node);
}

$xml.save($outfile);
