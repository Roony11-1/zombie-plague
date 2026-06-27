<#
.SYNOPSIS
    Instala los plugins compilados en el servidor y actualiza plugins.ini.
    Solo instala los plugins que NO están listados en .buildignore.
    Si no hay cambios en plugins.ini, no se modifica.
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

# --- Leer .buildignore ---
$IgnoreFile = Join-Path $ProjectRoot ".buildignore"
$IgnoredPlugins = @()
if (Test-Path $IgnoreFile) {
    $IgnoredPlugins = Get-Content $IgnoreFile | ForEach-Object { $_.Trim() } | Where-Object { $_ -and $_ -notmatch '^\s*#' }
}

# --- Obtener solo los plugins activos (no ignorados) ---
$PluginsDir = Join-Path $ProjectRoot "plugins"
$AllPluginDirs = Get-ChildItem -Path $PluginsDir -Directory
$ActivePluginNames = $AllPluginDirs | Where-Object { $_.Name -notin $IgnoredPlugins } | Select-Object -ExpandProperty Name

# --- Rutas del servidor ---
$ServerRoot = if (Test-Path Variable:env_HLDS_ROOT) { $env_HLDS_ROOT } else { "C:\servers\cs16" }
$GameDir = if (Test-Path Variable:env_GAME_DIR) { $env_GAME_DIR } else { "cstrike" }
$ServerDir = Join-Path $ServerRoot $GameDir

if (-not (Test-Path $ServerDir)) {
    Write-Error "La carpeta del juego '$ServerDir' no existe. Verifica HLDS_ROOT y GAME_DIR en .env."
    exit 1
}

$PluginsIniRel = if (Test-Path Variable:env_METAMOD_PLUGINS_INI) { $env_METAMOD_PLUGINS_INI } else { "addons/metamod/plugins.ini" }
$PluginsIniPath = Join-Path $ServerDir $PluginsIniRel
$PluginsDirIni = Split-Path $PluginsIniPath -Parent
$AddonsDir = Join-Path $ServerDir "addons"

if (-not (Test-Path $PluginsDirIni)) {
    New-Item -ItemType Directory -Path $PluginsDirIni -Force | Out-Null
}

# --- Entradas protegidas (siempre deben aparecer en plugins.ini) ---
$ProtectedEntries = @(
    "win32 addons/ebot/dlls/ebot.dll"
)

Write-Host "==> Instalando plugins en $ServerDir" -ForegroundColor Cyan
Write-Host "    Configuración: $Configuration" -ForegroundColor Cyan
Write-Host "    plugins.ini:   $PluginsIniPath" -ForegroundColor Cyan
if ($IgnoredPlugins.Count -gt 0) {
    Write-Host "    Ignorados:     $($IgnoredPlugins -join ', ')" -ForegroundColor DarkYellow
}
Write-Host ""

# --- Comprobar si hay plugins activos ---
if ($ActivePluginNames.Count -eq 0) {
    Write-Host "No hay plugins activos (todos están ignorados o no existen). Nada que instalar." -ForegroundColor Yellow
    exit 0
}

# --- Buscar DLLs compilados solo de los plugins activos ---
$Dlls = Get-ChildItem -Path "$BuildDir\plugins" -Recurse -Filter "*_mm.dll" | Where-Object {
    $_.Directory.Name -eq $Configuration
} | Where-Object {
    $dllPluginName = $_.BaseName -replace '_mm$', ''
    $dllPluginName -in $ActivePluginNames
}

if (-not $Dlls) {
    Write-Host "No se encontraron DLLs compilados para los plugins activos. Ejecuta primero .\tools\build.ps1" -ForegroundColor Yellow
    exit 0
}

# --- Función para reparar líneas pegadas (devuelve array) ---
function Repair-Lines {
    param([string[]]$Lines)
    $repaired = @()
    foreach ($line in $Lines) {
        $fragments = $line -split '(?<=\.dll)(?=win32\s+addons/)' | Where-Object { $_ -match '\S' }
        foreach ($frag in $fragments) {
            $trimmed = $frag.Trim()
            if ($trimmed -notmatch '\.dll$') {
                $trimmed += '.dll'
            }
            $repaired += $trimmed
        }
    }
    return $repaired
}

# --- Leer plugins.ini y reparar ---
$OriginalLines = @()
if (Test-Path $PluginsIniPath) {
    $rawContent = [System.IO.File]::ReadAllText($PluginsIniPath, [System.Text.Encoding]::UTF8)
    if ($rawContent) {
        $lines = $rawContent -split '\r?\n|\r' | Where-Object { $_.Trim() -ne '' }
        $OriginalLines = Repair-Lines -Lines $lines
    }
}

# Crear una copia modificable (ArrayList)
$WorkingLines = [System.Collections.ArrayList]::new()
foreach ($line in $OriginalLines) {
    $WorkingLines.Add($line) | Out-Null
}

$changesMade = $false

foreach ($dll in $Dlls) {
    $pluginName = $dll.BaseName -replace '_mm$', ''
    $pluginDestDir = Join-Path $AddonsDir $pluginName
    
    if (-not (Test-Path $pluginDestDir)) {
        New-Item -ItemType Directory -Path $pluginDestDir -Force | Out-Null
    }
    
    if ($Force) {
        Get-ChildItem -Path $pluginDestDir -Filter "*_mm.dll" | Remove-Item -Force
        Write-Host "    Limpiados DLLs antiguos en $pluginDestDir" -ForegroundColor DarkYellow
    }
    
    $destDll = Join-Path $pluginDestDir $dll.Name
    Copy-Item $dll.FullName -Destination $destDll -Force
    Write-Host "  Copiado: $($dll.Name) -> $pluginDestDir" -ForegroundColor Green
    
    $entry = "win32 addons/$pluginName/$($dll.Name)"
    
    # Buscar si ya existe una entrada para este plugin
    $foundIdx = -1
    for ($i = 0; $i -lt $WorkingLines.Count; $i++) {
        if ($WorkingLines[$i] -match [regex]::Escape("addons/$pluginName/")) {
            $foundIdx = $i
            break
        }
    }
    
    if ($foundIdx -ge 0) {
        if ($Force) {
            Write-Host "    Actualizando entrada en plugins.ini (forzado)" -ForegroundColor Yellow
            $WorkingLines[$foundIdx] = $entry
            $changesMade = $true
        } else {
            Write-Host "    Ya existe entrada en plugins.ini: $($WorkingLines[$foundIdx])" -ForegroundColor DarkGray
        }
    } else {
        Write-Host "    Añadido a plugins.ini" -ForegroundColor Magenta
        $WorkingLines.Add($entry) | Out-Null
        $changesMade = $true
    }
}

# --- Garantizar entradas protegidas ---
foreach ($protected in $ProtectedEntries) {
    $exists = $false
    foreach ($line in $WorkingLines) {
        if ($line -eq $protected) {
            $exists = $true
            break
        }
    }
    if (-not $exists) {
        Write-Host "  Añadiendo entrada protegida: $protected" -ForegroundColor Magenta
        $WorkingLines.Add($protected) | Out-Null
        $changesMade = $true
    }
}

# Solo escribir si hubo cambios reales
if ($changesMade) {
    # Eliminar duplicados manteniendo orden
    $FinalLines = @()
    foreach ($line in $WorkingLines) {
        if ($line -and $line -notin $FinalLines) {
            $FinalLines += $line
        }
    }
    $FinalContent = $FinalLines -join [Environment]::NewLine
    if ($FinalContent) {
        $FinalContent += [Environment]::NewLine
    }
    # Escribir plugins.ini en UTF-8 sin BOM (Metamod-r requiere sin BOM)
    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($PluginsIniPath, $FinalContent, $utf8NoBom)
    Write-Host ""
    Write-Host "==> plugins.ini actualizado." -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "==> plugins.ini sin cambios (todas las entradas ya existen)." -ForegroundColor DarkGray
}

Write-Host "    Reinicia el servidor o ejecuta 'restart' en la consola de HLDS." -ForegroundColor Yellow