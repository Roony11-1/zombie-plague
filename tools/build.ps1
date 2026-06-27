<#
.SYNOPSIS
    Compila todos los plugins del proyecto.
.PARAMETER Clean
    Si se especifica, borra la caché de CMake antes de compilar.
.EXAMPLE
    .\tools\build.ps1
    .\tools\build.ps1 -Clean
#>

param([switch]$Clean)

$ErrorActionPreference = "Stop"
& "$PSScriptRoot\config.ps1"

$ProjectRoot = (Get-Item "$PSScriptRoot\..").FullName
$BuildDir = Join-Path $ProjectRoot "build"
$CMakeGenerator = "Visual Studio 18 2026"
$CMakeArch = "Win32"
$Configuration = if (Test-Path Variable:env_BUILD_CONFIG) { $env_BUILD_CONFIG } else { "Release" }

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

Write-Host "==> Compilando todos los plugins ($Configuration)..." -ForegroundColor Yellow
Push-Location $BuildDir
try {
    cmake --build . --config $Configuration
    if ($LASTEXITCODE -ne 0) { throw "Error en la compilación." }
} finally { Pop-Location }

Write-Host "==> Compilación completada exitosamente." -ForegroundColor Green

Write-Host "`nDLLs generados:" -ForegroundColor Cyan
Get-ChildItem -Path "$BuildDir\plugins" -Recurse -Filter "*_mm.dll" | ForEach-Object {
    Write-Host "  $($_.FullName)" -ForegroundColor Gray
}