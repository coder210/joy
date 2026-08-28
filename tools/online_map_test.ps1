# Online map consistency test: host + client, both enter game, compare obstacle layout.
param(
    [string]$Exe = "D:\github\joy\build\Debug\project_proxy3d.exe",
    [string]$OutDir = "D:\github\joy\tools\online_shots",
    [string]$Tag = "o1"
)
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

# --- temporary loopback settings ---
$cfg = "D:\github\joy\build\Debug\settings.cfg"
$cfgBackup = Join-Path $OutDir "settings.cfg.bak"
if (Test-Path $cfg) { Copy-Item $cfg $cfgBackup -Force }
@"
server_ip=127.0.0.1
server_port=30000
bgm_volume=0.000000
sfx_volume=0.000000
fullscreen=0
"@ | Set-Content $cfg -Encoding ascii

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W32 {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, out POINT lpPoint);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int X, int Y);
    [DllImport("user32.dll")] public static extern void mouse_event(uint dwFlags, int dx, int dy, uint dwData, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
}
"@
# 点击窗口标题栏位置夺焦（真实用户输入允许抢前台），再发 SPACE
function Focus-And-Start($h) {
    $r = New-Object W32+RECT
    [W32]::GetWindowRect($h, [ref]$r) | Out-Null
    $cx = ($r.Left + $r.Right) / 2
    $cy = $r.Top + 10
    [W32]::SetCursorPos([int]$cx, [int]$cy) | Out-Null
    Start-Sleep -Milliseconds 100
    [W32]::mouse_event(0x0002, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 60
    [W32]::mouse_event(0x0004, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 300
    [W32]::keybd_event(0x20, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 60
    [W32]::keybd_event(0x20, 0, 2, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 120
}
function Save-Shot($h, $path) {
    $r = New-Object W32+RECT
    [W32]::GetWindowRect($h, [ref]$r) | Out-Null
    $w = $r.Right - $r.Left; $hh = $r.Bottom - $r.Top
    if ($w -le 0 -or $hh -le 0) { return $false }
    $bmp = New-Object System.Drawing.Bitmap($w, $hh)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size($w, $hh)))
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    return $true
}
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class WFind {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
}
"@
function Wait-Window($proc) {
    $script:tpid = $proc.Id
    $script:fH = [IntPtr]::Zero
    for ($i = 0; $i -lt 100 -and $script:fH -eq [IntPtr]::Zero; $i++) {
        Start-Sleep -Milliseconds 200
        $proc.Refresh()
        if ($proc.MainWindowHandle -ne [IntPtr]::Zero) { return $proc.MainWindowHandle }
        $script:fH = [IntPtr]::Zero
        $cb = [WFind+EnumWindowsProc]{ param($w, $l)
            $p2 = [uint32]0
            [WFind]::GetWindowThreadProcessId($w, [ref]$p2) | Out-Null
            if ($p2 -eq $script:tpid -and [WFind]::IsWindowVisible($w)) { $script:fH = $w; return $false }
            return $true
        }
        [WFind]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    }
    return $script:fH
}

$hostOut = Join-Path $OutDir "$Tag.host.out.log"
$hostErr = Join-Path $OutDir "$Tag.host.err.log"
$cliOut  = Join-Path $OutDir "$Tag.cli.out.log"
$cliErr  = Join-Path $OutDir "$Tag.cli.err.log"

# --- start host ---
$hostProc = Start-Process -FilePath $Exe -ArgumentList @("--mode=host") -WorkingDirectory (Split-Path $Exe) `
    -RedirectStandardOutput $hostOut -RedirectStandardError $hostErr -PassThru
$hHost = Wait-Window $hostProc
if ($hHost -eq [IntPtr]::Zero) { Write-Error "no host window"; exit 1 }
[W32]::ShowWindow($hHost, 9) | Out-Null
Start-Sleep -Milliseconds 3500
Focus-And-Start $hHost   # click title bar to focus + SPACE -> host enters game
Start-Sleep -Milliseconds 3500

# --- start client ---
$cliProc = Start-Process -FilePath $Exe -ArgumentList @("--mode=client") -WorkingDirectory (Split-Path $Exe) `
    -RedirectStandardOutput $cliOut -RedirectStandardError $cliErr -PassThru
$hCli = Wait-Window $cliProc
if ($hCli -eq [IntPtr]::Zero) { Write-Error "no client window"; exit 1 }
[W32]::ShowWindow($hCli, 9) | Out-Null
Start-Sleep -Milliseconds 3500
Focus-And-Start $hCli   # click title bar to focus + SPACE -> client connects
Start-Sleep -Milliseconds 6000

Save-Shot $hHost (Join-Path $OutDir "$Tag.host.png")
Save-Shot $hCli  (Join-Path $OutDir "$Tag.cli.png")

$hostProc.Refresh(); if (-not $hostProc.HasExited) { $hostProc.Kill() }
$cliProc.Refresh();  if (-not $cliProc.HasExited)  { $cliProc.Kill() }

# restore settings
if (Test-Path $cfgBackup) { Copy-Item $cfgBackup $cfg -Force } else { Remove-Item $cfg -Force -ErrorAction SilentlyContinue }

Write-Output "=== host pmap ==="
Get-Content $hostErr -ErrorAction SilentlyContinue | Where-Object { $_ -match "\[pmap\]|loading|join|snapshot" } | Select-Object -First 10
Write-Output "=== client pmap/loading ==="
Get-Content $cliErr -ErrorAction SilentlyContinue | Where-Object { $_ -match "\[pmap\]|handle_loading|loading|snapshot" } | Select-Object -First 10
Write-Output "done"
