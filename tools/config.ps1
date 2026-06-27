<#
.SYNOPSIS
    Carga variables de entorno desde el archivo .env en la raíz del proyecto.
    Si .env no existe, se crea a partir de .env.example y se detiene la ejecución.
#>

$ProjectRoot = (Get-Item "$PSScriptRoot\..").FullName
$EnvFile = Join-Path $ProjectRoot ".env"
$EnvExample = Join-Path $ProjectRoot ".env.example"

# Si no existe .env, copiar desde el ejemplo
if (-not (Test-Path $EnvFile)) {
    if (Test-Path $EnvExample) {
        Copy-Item $EnvExample $EnvFile
        Write-Warning "Archivo .env creado desde .env.example en la raíz del proyecto."
        Write-Host "Por favor, edita '.env' con tus rutas y vuelve a ejecutar el script." -ForegroundColor Yellow
        exit 1
    }
    else {
        Write-Error "No se encontró .env ni .env.example en la raíz del proyecto."
        exit 1
    }
}

# Cargar variables en el ámbito global con prefijo "env_"
$envContent = Get-Content $EnvFile -Encoding UTF8
foreach ($line in $envContent) {
    if ($line -match '^\s*#' -or $line -match '^\s*$') { continue }
    if ($line -match '^\s*([^=]+)\s*=\s*(.+)\s*$') {
        $key = $matches[1].Trim()
        $value = $matches[2].Trim()
        # Quitar comillas si las tiene
        $value = $value -replace '^"|"$', ''
        Set-Variable -Name "env_$key" -Value $value -Scope Global
    }
}