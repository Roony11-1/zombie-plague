<#
.SYNOPSIS
    Compila, instala y arranca el servidor en un solo paso.
.EXAMPLE
    .\tools\dev.ps1
    .\tools\dev.ps1 -Map de_inferno
#>

param([string]$Map, [int]$MaxPlayers, [string]$ExtraArgs)

Write-Host "=== COMPILANDO ===" -ForegroundColor Cyan
& "$PSScriptRoot\build.ps1"

Write-Host "=== INSTALANDO ===" -ForegroundColor Cyan
& "$PSScriptRoot\install.ps1"

Write-Host "=== INICIANDO SERVIDOR ===" -ForegroundColor Cyan
$runArgs = @()
if ($Map) { $runArgs += "-Map", $Map }
if ($MaxPlayers) { $runArgs += "-MaxPlayers", $MaxPlayers }
if ($ExtraArgs) { $runArgs += "-ExtraArgs", $ExtraArgs }
& "$PSScriptRoot\run.ps1" @runArgs