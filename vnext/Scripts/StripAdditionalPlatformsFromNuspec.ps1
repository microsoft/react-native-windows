# For PR builds we dont want to have to build _ALL_ the platforms just to get a nuget and test one platform.
# this script filters the files required by the nuget, so we can produce a test nuget on a single platform

param (
    [string]$nuspec = "$PSScriptRoot\Microsoft.ReactNative.nuspec",
    [string]$outfile = "$PSScriptRoot\Microsoft.ReactNative.PR.nuspec",
    [string[]]$slices = @("x64.Release")
 )

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

foreach($file in $xml.package.files.ChildNodes) {
    if ($file.NodeType -eq "Comment") {
        continue;
    }

    foreach($slice in $allSlices) {
        $sliceInfo = $slice.split('.');
        $platform = $sliceInfo[0];
        $flavor = $sliceInfo[1];

        if ($file.src.Contains($platform) -and $file.src.Contains($flavor) -and !$slices.Contains("$platform.$flavor")) {
            $nodesToRemove += $file
            break;
        }
    }
}

foreach ($node in $nodesToRemove) {
    Write-Debug $xml.package.files.RemoveChild($node);
}

$xml.save($outfile);