<#
.SYNOPSIS
    Compila solo los plugins que no están listados en .buildignore.
.PARAMETER Clean
    Limpia la caché de CMake antes de compilar.
#>

param([switch]$Clean)

$ErrorActionPreference = "Stop"
& "$PSScriptRoot\config.ps1"

$ProjectRoot = (Get-Item "$PSScriptRoot\..").FullName
$BuildDir = Join-Path $ProjectRoot "build"
$CMakeGenerator = "Visual Studio 18 2026"
$CMakeArch = "Win32"
$Configuration = if (Test-Path Variable:env_BUILD_CONFIG) { $env_BUILD_CONFIG } else { "Release" }

# --- Leer .buildignore ---
$IgnoreFile = Join-Path $ProjectRoot ".buildignore"
$IgnoredPlugins = @()
if (Test-Path $IgnoreFile) {
    $IgnoredPlugins = Get-Content $IgnoreFile | ForEach-Object { $_.Trim() } | Where-Object { $_ -and $_ -notmatch '^\s*#' }
    if ($IgnoredPlugins.Count -gt 0) {
        Write-Host "==> Plugins ignorados: $($IgnoredPlugins -join ', ')" -ForegroundColor DarkYellow
    }
}

# --- Lista de plugins a compilar ---
$PluginsDir = Join-Path $ProjectRoot "plugins"
$AllPluginDirs = Get-ChildItem -Path $PluginsDir -Directory
$Targets = $AllPluginDirs | Where-Object { $_.Name -notin $IgnoredPlugins }

if ($Targets.Count -eq 0) {
    Write-Warning "No hay plugins para compilar. Revisa .buildignore o la carpeta plugins."
    exit 0
}

Write-Host "==> Proyecto: $ProjectRoot" -ForegroundColor Cyan
Write-Host "==> Build dir: $BuildDir" -ForegroundColor Cyan

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

$CacheFile = Join-Path $BuildDir "CMakeCache.txt"
if ($Clean -or -not (Test-Path $CacheFile)) {
    if ($Clean) {
        Write-Host "==> Limpiando caché de CMake..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force "$BuildDir\*" -ErrorAction SilentlyContinue
    }
    Write-Host "==> Configurando CMake..." -ForegroundColor Yellow
    Push-Location $BuildDir
    try {
        cmake .. -G $CMakeGenerator -A $CMakeArch
        if ($LASTEXITCODE -ne 0) { throw "Error en la configuración de CMake." }
    } finally { Pop-Location }
} else {
    Write-Host "==> CMake ya configurado. Usando caché existente." -ForegroundColor Green
}

Write-Host "==> Compilando plugins ($Configuration)..." -ForegroundColor Yellow
Push-Location $BuildDir
try {
    foreach ($target in $Targets) {
        Write-Host "   -> $($target.Name)" -ForegroundColor Gray
        cmake --build . --config $Configuration --target $($target.Name)
        if ($LASTEXITCODE -ne 0) { throw "Error compilando $($target.Name)." }
    }
} finally { Pop-Location }

Write-Host "==> Compilación completada exitosamente." -ForegroundColor Green

Write-Host "`nDLLs generados:" -ForegroundColor Cyan
Get-ChildItem -Path "$BuildDir\plugins" -Recurse -Filter "*_mm.dll" | ForEach-Object {
    Write-Host "  $($_.FullName)" -ForegroundColor Gray
}