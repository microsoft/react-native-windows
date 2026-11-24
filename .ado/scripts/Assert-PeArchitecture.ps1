param(
  [Parameter(Mandatory)]
  [ValidateSet('ARM64', 'ARM64EC')]
  [string]$Expected,
  [Parameter(Mandatory)]
  [string[]]$Path
)

$ErrorActionPreference = 'Stop'
Write-Host "OSArchitecture: $([System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture)"
$dumpbin = $null

foreach ($file in $Path) {
  $resolved = (Resolve-Path -LiteralPath $file).ProviderPath
  $stream = [System.IO.File]::OpenRead($resolved)
  $reader = [System.IO.BinaryReader]::new($stream)
  try {
    if ($stream.Length -lt 64 -or $reader.ReadUInt16() -ne 0x5A4D) {
      throw "Missing or truncated DOS header: $file"
    }
    $stream.Position = 0x3C
    $peOffset = $reader.ReadUInt32()
    if ($peOffset -lt 64 -or $peOffset -gt ($stream.Length - 6)) {
      throw "PE header offset is outside the file: $file"
    }
    $stream.Position = $peOffset
    if ($reader.ReadUInt32() -ne 0x00004550) {
      throw "Invalid PE signature: $file"
    }
    $machine = $reader.ReadUInt16()
  } finally {
    $reader.Dispose()
  }

  $expectedMachine = if ($Expected -eq 'ARM64') { 0xAA64 } else { 0x8664 }
  if ($machine -ne $expectedMachine) {
    throw ('Expected {0}, found PE machine 0x{1:X4}: {2}' -f $Expected, $machine, $file)
  }

  if ($Expected -eq 'ARM64EC') {
    # ARM64EC uses the x64 machine field; only recognized hybrid metadata distinguishes it from plain x64.
    if (-not $dumpbin) {
      $command = Get-Command dumpbin.exe -ErrorAction SilentlyContinue
      if ($command) {
        $dumpbin = $command.Source
      } else {
        $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
        if (-not (Test-Path -LiteralPath $vswhere -PathType Leaf)) {
          throw 'ARM64EC inspection requires Visual Studio dumpbin.'
        }
        $candidates = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
          -find 'VC\Tools\MSVC\**\bin\Hostx64\x64\dumpbin.exe'
        if ($LASTEXITCODE -ne 0) { throw "vswhere failed with exit code $LASTEXITCODE" }
        $dumpbin = $candidates | Sort-Object -Descending | Select-Object -First 1
        if (-not $dumpbin) { throw 'ARM64EC inspection requires Visual Studio dumpbin.' }
      }
    }
    $output = & $dumpbin /headers /loadconfig $resolved 2>&1
    if ($LASTEXITCODE -ne 0) { throw "dumpbin failed with exit code ${LASTEXITCODE}: $file" }
    $text = $output -join "`n"
    if ($text -notmatch '(?m)^\s*8664 machine \(x64\)' -or
        $text -notmatch '(?s)Section contains the following load config:\s*(.*?)(?:Section contains|\z)') {
      throw "Unrecognized ARM64EC headers or load configuration: $file"
    }
    $loadConfig = $Matches[1]
    if ($loadConfig -notmatch '(?m)^\s*([0-9A-Fa-f]{16}) Hybrid metadata pointer\s*$' -or
        [Convert]::ToUInt64($Matches[1], 16) -eq 0 -or
        $text -notmatch 'Section contains the following hybrid metadata:') {
      throw "Missing, zero, or unrecognized ARM64EC hybrid metadata: $file"
    }
  }
  Write-Host "Verified ${Expected}: $resolved"
}
