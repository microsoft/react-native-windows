# For PR builds we dont want to have to build _ALL_ the platforms just to get a nuget and test one platform.
# this script filters the files required by the nuget, so we can produce a test nuget on a single platform

param (
    [string]$nuspec = "$PSScriptRoot\Microsoft.ReactNative.nuspec",
    [string]$outfile = "$PSScriptRoot\Microsoft.ReactNative.PR.nuspec"
 )

Write-Output "Creating filtered version of : $nuspec"
$xml = [xml](gc $nuspec)

$nodesToRemove = @();

foreach($file in $xml.package.files.ChildNodes) {
    if ($file.NodeType -eq "Comment") {
        continue;
    }

    if ($file.src.Contains("ARM") -or $file.src.Contains("x86")) {
        $nodesToRemove += $file
    }
}

foreach ($node in $nodesToRemove) {
    Write-Debug $xml.package.files.RemoveChild($node);
}

$xml.save($outfile);