<#
.SYNOPSIS
    Crea un nuevo plugin a partir de la plantilla templates/plugin o templates/plugin_game.
.PARAMETER Name
    Nombre del nuevo plugin (se creará en plugins/<Name>).
.PARAMETER Author
    Autor del plugin (por defecto "Roony11-1").
.PARAMETER Version
    Versión inicial (por defecto "0.1").
.PARAMETER Url
    URL del proyecto (por defecto "https://github.com/Roony11-1").
.PARAMETER GameMode
    Usa la plantilla para modos de juego (incluye SDKs del juego y utilidades para jugadores).
.EXAMPLE
    .\tools\new-plugin.ps1 zombie_plague
    .\tools\new-plugin.ps1 zombie_plague -GameMode -Author "Otro" -Version "1.0"
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$Name,
    [string]$Author = "Roony11-1",
    [string]$Version = "0.1",
    [string]$Url = "https://github.com/Roony11-1",
    [switch]$GameMode
)

$ErrorActionPreference = "Stop"

$ProjectRoot = (Get-Item "$PSScriptRoot\..").FullName

# Seleccionar la plantilla según el parámetro
if ($GameMode) {
    $TemplateDir = Join-Path $ProjectRoot "templates\plugin_game"
} else {
    $TemplateDir = Join-Path $ProjectRoot "templates\plugin"
}

$PluginsDir = Join-Path $ProjectRoot "plugins"
$NewPluginDir = Join-Path $PluginsDir $Name

if (-not (Test-Path $TemplateDir)) {
    Write-Error "No se encontró la plantilla en $TemplateDir. Ejecuta desde la raíz del proyecto."
    exit 1
}

if (Test-Path $NewPluginDir) {
    Write-Error "Ya existe un plugin llamado '$Name' en $NewPluginDir."
    exit 1
}

# Copiar la plantilla
Copy-Item -Path $TemplateDir -Destination $NewPluginDir -Recurse

# Reemplazar marcadores en los archivos copiados
$Replacements = @{
    '{{PLUGIN_NAME}}' = $Name
    '{{VERSION}}'     = $Version
    '{{AUTHOR}}'      = $Author
    '{{URL}}'         = $Url
    '{{LOG_TAG}}'     = $Name.ToUpper()
}

Get-ChildItem -Path $NewPluginDir -Recurse -File | ForEach-Object {
    $content = Get-Content $_.FullName -Raw -Encoding UTF8
    $changed = $false
    foreach ($key in $Replacements.Keys) {
        if ($content -match [regex]::Escape($key)) {
            $content = $content -replace [regex]::Escape($key), $Replacements[$key]
            $changed = $true
        }
    }
    if ($changed) {
        Set-Content $_.FullName -Value $content -Encoding UTF8 -NoNewline
    }
}

Write-Host "Plugin '$Name' creado en $NewPluginDir" -ForegroundColor Green
if ($GameMode) {
    Write-Host "Plantilla: juego (con SDKs de ReGameDLL y utilidades para jugadores)" -ForegroundColor Cyan
}
Write-Host "Autor : $Author"
Write-Host "Versión: $Version"
Write-Host "URL   : $Url"
Write-Host "Puedes empezar a editar main.cpp y luego ejecutar .\tools\build.ps1" -ForegroundColor Yellow