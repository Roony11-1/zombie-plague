<#
.SYNOPSIS
    Inicia el servidor HLDS con la configuración del .env.
.PARAMETER Map
    Mapa inicial (por defecto usa DEFAULT_MAP del .env o de_dust2).
.PARAMETER MaxPlayers
    Número máximo de jugadores (por defecto 16).
.PARAMETER ExtraArgs
    Argumentos adicionales para hlds.exe (ej. "-port 27016 +sv_lan 1").
.PARAMETER ShowHost
    Muestra la IP local y el puerto que usará el servidor, sin iniciarlo.
.EXAMPLE
    .\tools\run.ps1
    .\tools\run.ps1 -Map de_inferno -MaxPlayers 20
    .\tools\run.ps1 -ShowHost
#>

param(
    [string]$Map,
    [int]$MaxPlayers,
    [string]$ExtraArgs,
    [switch]$ShowHost
)

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

# --- Determinar el puerto ---
$Port = 27015   # valor por defecto
if ($ExtraArgs) {
    if ($ExtraArgs -match '-port\s+(\d+)') {
        $Port = [int]$Matches[1]
    }
}
else {
    # Si no se pasó -ExtraArgs, usar la variable de entorno si existe
    if (Test-Path Variable:env_EXTRA_ARGS) {
        if ($env_EXTRA_ARGS) {
            $ExtraArgs = $env_EXTRA_ARGS
            if ($ExtraArgs -match '-port\s+(\d+)') {
                $Port = [int]$Matches[1]
            }
        }
    }
}

# --- Obtener IP local ---
function Get-LocalIP {
    $ip = Get-NetIPAddress -AddressFamily IPv4 -PrefixOrigin Dhcp, Manual |
        Where-Object { $_.IPAddress -ne '127.0.0.1' } |
        Select-Object -First 1 -ExpandProperty IPAddress
    if (-not $ip) {
        $ip = (Get-NetIPAddress -AddressFamily IPv4 | Where-Object { $_.IPAddress -ne '127.0.0.1' }).IPAddress
    }
    if (-not $ip) { $ip = '127.0.0.1' }
    return $ip
}

$LocalIP = Get-LocalIP
$HostInfo = "${LocalIP}:${Port}"

# --- Si solo se quiere ver el host ---
if ($ShowHost) {
    Write-Host "El servidor se anunciaría como: $HostInfo" -ForegroundColor Green
    Write-Host "(puerto por defecto o tomado de -port en EXTRA_ARGS)" -ForegroundColor DarkGray
    exit 0
}

# --- Construir argumentos para HLDS ---
$ArgsList = @("-console", "-game", $GameDir, "+map", $Map, "+maxplayers", $MaxPlayers)
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
Write-Host " Host      : $HostInfo"
if ($ExtraArgs) { Write-Host " Extra     : $ExtraArgs" }
Write-Host "==============================================" -ForegroundColor Cyan
Write-Host ""

Push-Location $ServerRoot
try { & $HldsExe @ArgsList }
finally { Pop-Location }