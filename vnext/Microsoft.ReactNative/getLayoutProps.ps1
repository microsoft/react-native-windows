# This script produces the values for the layoutProperties vector
# in TestHook.cpp
$page = Invoke-WebRequest "https://reactnative.dev/docs/layout-props"
$lines = $page.Content.Split('</a><code>') | select -skip 1
$lines -replace ('\n','')  -replace ('</code>.*', '') | % { Write-Host "`"$_`", "}