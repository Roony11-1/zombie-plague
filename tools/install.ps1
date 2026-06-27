<#
.SYNOPSIS
    Instala los plugins compilados en el servidor y actualiza plugins.ini.
.PARAMETER Force
    Sobrescribe siempre las entradas en plugins.ini incluso si ya existen.
.EXAMPLE
    .\tools\install.ps1
    .\tools\install.ps1 -Force
#>

param([switch]$Force)

$ErrorActionPreference = "Stop"
& "$PSScriptRoot\config.ps1"

$ProjectRoot = (Get-Item "$PSScriptRoot\..").FullName
$BuildDir = Join-Path $ProjectRoot "build"
$Configuration = if (Test-Path Variable:env_BUILD_CONFIG) { $env_BUILD_CONFIG } else { "Release" }

$ServerRoot = if (Test-Path Variable:env_HLDS_ROOT) { $env_HLDS_ROOT } else { "C:\servers\cs16" }
$GameDir = if (Test-Path Variable:env_GAME_DIR) { $env_GAME_DIR } else { "cstrike" }
$ServerDir = Join-Path $ServerRoot $GameDir

if (-not (Test-Path $ServerDir)) {
    Write-Error "La carpeta del juego '$ServerDir' no existe. Verifica HLDS_ROOT y GAME_DIR en .env."
    exit 1
}

$PluginsIniRel = if (Test-Path Variable:env_METAMOD_PLUGINS_INI) { $env_METAMOD_PLUGINS_INI } else { "addons/metamod/plugins.ini" }
$PluginsIniPath = Join-Path $ServerDir $PluginsIniRel
$PluginsDir = Split-Path $PluginsIniPath -Parent
$AddonsDir = Join-Path $ServerDir "addons"

if (-not (Test-Path $PluginsDir)) {
    New-Item -ItemType Directory -Path $PluginsDir -Force | Out-Null
}

Write-Host "==> Instalando plugins en $ServerDir" -ForegroundColor Cyan
Write-Host "    Configuración: $Configuration" -ForegroundColor Cyan
Write-Host "    plugins.ini:   $PluginsIniPath" -ForegroundColor Cyan
Write-Host ""

$Dlls = Get-ChildItem -Path "$BuildDir\plugins" -Recurse -Filter "*_mm.dll" | Where-Object { $_.Directory.Name -eq $Configuration }

if (-not $Dlls) {
    Write-Host "No se encontraron DLLs compilados. Ejecuta primero .\tools\build.ps1" -ForegroundColor Yellow
    exit 0
}

$ExistingLines = @()
if (Test-Path $PluginsIniPath) {
    $ExistingLines = Get-Content $PluginsIniPath
}

foreach ($dll in $Dlls) {
    $pluginName = $dll.BaseName -replace '_mm$', ''
    $pluginDestDir = Join-Path $AddonsDir $pluginName
    
    if (-not (Test-Path $pluginDestDir)) {
        New-Item -ItemType Directory -Path $pluginDestDir -Force | Out-Null
    }
    
    $destDll = Join-Path $pluginDestDir $dll.Name
    Copy-Item $dll.FullName -Destination $destDll -Force
    Write-Host "  Copiado: $($dll.Name) -> $pluginDestDir" -ForegroundColor Green
    
    $entry = "win32 addons/$pluginName/$($dll.Name)"
    
    $existingIdx = -1
    for ($i = 0; $i -lt $ExistingLines.Count; $i++) {
        if ($ExistingLines[$i] -match [regex]::Escape("addons/$pluginName/")) {
            $existingIdx = $i
            break
        }
    }
    
    if ($existingIdx -ge 0) {
        if ($Force) {
            Write-Host "    Actualizando entrada en plugins.ini (forzado)" -ForegroundColor Yellow
            $ExistingLines[$existingIdx] = $entry
        } else {
            Write-Host "    Ya existe entrada en plugins.ini: $($ExistingLines[$existingIdx])" -ForegroundColor DarkGray
        }
    } else {
        Write-Host "    Añadido a plugins.ini" -ForegroundColor Magenta
        $ExistingLines += $entry
    }
}

$FinalLines = @()
foreach ($line in $ExistingLines) {
    if ($line -and $line -notin $FinalLines) {
        $FinalLines += $line
    }
}

Set-Content $PluginsIniPath -Value $FinalLines

Write-Host ""
Write-Host "==> Instalación completada." -ForegroundColor Green
Write-Host "    Reinicia el servidor o ejecuta 'restart' en la consola de HLDS." -ForegroundColor Yellow