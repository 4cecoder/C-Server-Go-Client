# Check if Chocolatey is installed
if ((Get-Command choco -ErrorAction SilentlyContinue) -eq $null) {
    # Install Chocolatey
    Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
}

# Check if libmicrohttpd is installed
if ((choco list --local-only -r libmicrohttpd) -eq $null) {
    # Install libmicrohttpd
    choco install libmicrohttpd
}

# Check if sqlite3 is installed
if ((choco list --local-only -r sqlite) -eq $null) {
    # Install sqlite3
    choco install sqlite
}

# Refresh environment
refreshenv


New-Item -ItemType Directory -Force -Path build
Set-Location -Path build
cmake ..
make
./medi_example
