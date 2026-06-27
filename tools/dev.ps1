<#
.SYNOPSIS
    Compila, instala y arranca el servidor en un solo paso.
.PARAMETER Clean
    Fuerza limpieza de caché CMake (build -Clean) e instalación forzada (install -Force).
.EXAMPLE
    .\tools\dev.ps1
    .\tools\dev.ps1 -Clean -Map de_inferno
#>

param(
    [string]$Map,
    [int]$MaxPlayers,
    [string]$ExtraArgs,
    [switch]$Clean
)

Write-Host "=== COMPILANDO ===" -ForegroundColor Cyan
if ($Clean) {
    & "$PSScriptRoot\build.ps1" -Clean
} else {
    & "$PSScriptRoot\build.ps1"
}

Write-Host "=== INSTALANDO ===" -ForegroundColor Cyan
if ($Clean) {
    & "$PSScriptRoot\install.ps1" -Force
} else {
    & "$PSScriptRoot\install.ps1"
}

Write-Host "=== INICIANDO SERVIDOR ===" -ForegroundColor Cyan
$runArgs = @()
if ($Map) { $runArgs += "-Map", $Map }
if ($MaxPlayers) { $runArgs += "-MaxPlayers", $MaxPlayers }
if ($ExtraArgs) { $runArgs += "-ExtraArgs", $ExtraArgs }
& "$PSScriptRoot\run.ps1" @runArgs