<#
.SYNOPSIS
    Inicia el servidor HLDS con la configuración del .env.
.PARAMETER Map
    Mapa inicial (por defecto usa DEFAULT_MAP del .env o de_dust2).
.PARAMETER MaxPlayers
    Número máximo de jugadores (por defecto 16).
.PARAMETER ExtraArgs
    Argumentos adicionales para hlds.exe (ej. "-port 27016 +sv_lan 1").
.EXAMPLE
    .\tools\run.ps1
    .\tools\run.ps1 -Map de_inferno -MaxPlayers 20
#>

param([string]$Map, [int]$MaxPlayers, [string]$ExtraArgs)

$ErrorActionPreference = "Stop"
& "$PSScriptRoot\config.ps1"

$ServerRoot = if (Test-Path Variable:env_HLDS_ROOT) { $env_HLDS_ROOT } else { "C:\servers\cs16" }
$GameDir = if (Test-Path Variable:env_GAME_DIR) { $env_GAME_DIR } else { "cstrike" }
if (-not $Map) {
    $Map = if (Test-Path Variable:env_DEFAULT_MAP) { $env_DEFAULT_MAP } else { "de_dust2" }
}
if (-not $MaxPlayers) {
    $MaxPlayers = if (Test-Path Variable:env_DEFAULT_MAXPLAYERS) { [int]$env_DEFAULT_MAXPLAYERS } else { 16 }
}

$HldsExe = Join-Path $ServerRoot "hlds.exe"
if (-not (Test-Path $HldsExe)) {
    Write-Error "No se encontró hlds.exe en '$HldsExe'. Verifica HLDS_ROOT en .env."
    exit 1
}

$ArgsList = @("-console", "-game", $GameDir, "+map", $Map, "+maxplayers", $MaxPlayers)
if (-not $ExtraArgs -and (Test-Path Variable:env_EXTRA_ARGS) -and $env_EXTRA_ARGS) {
    $ExtraArgs = $env_EXTRA_ARGS
}
if ($ExtraArgs) {
    $ArgsList += $ExtraArgs -split ' '
}

Write-Host "==============================================" -ForegroundColor Cyan
Write-Host " Iniciando servidor Half-Life" -ForegroundColor Cyan
Write-Host "==============================================" -ForegroundColor Cyan
Write-Host " HLDS Root : $ServerRoot"
Write-Host " Juego     : $GameDir"
Write-Host " Mapa      : $Map"
Write-Host " Jugadores : $MaxPlayers"
if ($ExtraArgs) { Write-Host " Extra     : $ExtraArgs" }
Write-Host "==============================================" -ForegroundColor Cyan
Write-Host ""

Push-Location $ServerRoot
try { & $HldsExe @ArgsList }
finally { Pop-Location }