# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
  Downloads a file from a URI with retry logic.

.PARAMETER Uri
  The URI to download from.

.PARAMETER OutFile
  The output file path.

.PARAMETER MaxRetries
  Maximum number of download attempts. Default is 3.
#>
param(
  [Parameter(Mandatory)]
  [string]$Uri,

  [Parameter(Mandatory)]
  [string]$OutFile,

  [int]$MaxRetries = 3
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

for ($i = 1; $i -le $MaxRetries; $i++) {
  try {
    Write-Host "Downloading $OutFile (attempt $i of $MaxRetries)"
    Invoke-WebRequest -Uri $Uri -OutFile $OutFile
    return
  } catch {
    Write-Host "Attempt $i failed: $_"
    if ($i -eq $MaxRetries) { throw }
    Start-Sleep -Seconds (5 * $i)
  }
}
