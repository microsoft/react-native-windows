param(
  [Parameter(Mandatory)][string]$Arm64Path,
  [Parameter(Mandatory)][string]$Arm64EcPath,
  [Parameter(Mandatory)][string]$X64Path
)
$ErrorActionPreference = 'Stop'
$checker = Join-Path $PSScriptRoot '../scripts/Assert-PeArchitecture.ps1'
function Assert-Rejected([string]$Expected, [string]$File) {
  try { & $checker -Expected $Expected -Path $File } catch {
    Write-Host "Rejected ${Expected}: $File"
    return
  }
  throw "Unexpectedly accepted ${Expected}: $File"
}
& $checker -Expected ARM64 -Path $Arm64Path
& $checker -Expected ARM64EC -Path $Arm64EcPath
Assert-Rejected ARM64 $Arm64EcPath
Assert-Rejected ARM64EC $Arm64Path
$temp = Join-Path ([System.IO.Path]::GetTempPath()) ([guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $temp | Out-Null
try {
  $header = [byte[]]::new(64)
  $stream = [System.IO.File]::OpenRead((Resolve-Path -LiteralPath $Arm64Path).ProviderPath)
  try { $null = $stream.Read($header, 0, 64) } finally { $stream.Dispose() }
  foreach ($expected in 'ARM64', 'ARM64EC') {
    Assert-Rejected $expected $X64Path
    Assert-Rejected $expected (Join-Path $temp 'missing.exe')
    # Prefix lengths cover empty files, partial DOS headers, and an out-of-file PE offset.
    foreach ($length in 0, 2, 63, 64) {
      $file = Join-Path $temp 'truncated.exe'
      $prefix = [byte[]]::new($length)
      [Array]::Copy($header, $prefix, $length)
      [System.IO.File]::WriteAllBytes($file, $prefix)
      Assert-Rejected $expected $file
    }
  }
} finally {
  Remove-Item -LiteralPath $temp -Recurse -Force
}
Write-Host 'PE architecture smoke checks passed.'
