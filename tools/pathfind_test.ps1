# Verify enemy A* pathfinding chase: move player near enemy (10,10) to trigger chase.
# Key mapping (movement_system.h): W=INPUT_UP(+Z), A=INPUT_LEFT(+X), S=INPUT_DOWN(-Z), D=INPUT_RIGHT(-X).
# Enemy spawns at (10,10), detect radius 5m; player spawns at (0,0).
# Plan: hold A 1.6s (+X to x~=8, avoiding the (3,3) box), then hold W 1.4s (+Z to z~=7).
# Player ends near (8,7): distance to enemy sqrt(4+9)=3.6m < 5m -> chase triggers, enemy A* follows.
param(
    [string]$Exe = "D:\github\joy\build\Debug\project_proxy3d.exe",
    [string]$OutDir = "D:\github\joy\tools\pf_shots"
)
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class Win32 {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, out POINT lpPoint);
    [DllImport("user32.dll")] public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern void mouse_event(uint dwFlags, int dx, int dy, uint dwData, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int X, int Y);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
}
"@
function Get-Rect($h) {
    $r = New-Object Win32+RECT
    [Win32]::GetWindowRect($h, [ref]$r) | Out-Null
    return $r
}
function Click-At($h, $logicalX, $logicalY) {
    $cr = New-Object Win32+RECT
    [Win32]::GetClientRect($h, [ref]$cr) | Out-Null
    $p = New-Object Win32+POINT
    $p.X = 0; $p.Y = 0
    [Win32]::ClientToScreen($h, [ref]$p) | Out-Null
    $sx = $p.X + [int]($logicalX * ($cr.Right / 480.0))
    $sy = $p.Y + [int]($logicalY * ($cr.Bottom / 320.0))
    [Win32]::SetCursorPos($sx, $sy) | Out-Null
    Start-Sleep -Milliseconds 60
    [Win32]::mouse_event(0x0002, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 40
    [Win32]::mouse_event(0x0004, 0, 0, 0, [UIntPtr]::Zero)
}
function Save-Shot($h, $path) {
    $r = Get-Rect $h
    $w = $r.Right - $r.Left; $hh = $r.Bottom - $r.Top
    if ($w -le 0 -or $hh -le 0) { return $false }
    $bmp = New-Object System.Drawing.Bitmap($w, $hh)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size($w, $hh)))
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    return $true
}
function KeyDown($vk) { [Win32]::keybd_event($vk, 0, 0, [UIntPtr]::Zero) }
function KeyUp($vk)   { [Win32]::keybd_event($vk, 0, 2, [UIntPtr]::Zero) }

$outLog = Join-Path $OutDir "pf_stdout.log"
$errLog = Join-Path $OutDir "pf_stderr.log"
$proc = Start-Process -FilePath $Exe -ArgumentList @("--mode=single") -WorkingDirectory (Split-Path $Exe) `
        -RedirectStandardOutput $outLog -RedirectStandardError $errLog -PassThru

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class WinFind {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
}
"@
$h = [IntPtr]::Zero
$script:targetPid = $proc.Id
for ($i = 0; $i -lt 100 -and $h -eq [IntPtr]::Zero; $i++) {
    Start-Sleep -Milliseconds 200
    $proc.Refresh()
    if ($proc.MainWindowHandle -ne [IntPtr]::Zero) { $h = $proc.MainWindowHandle; break }
    $script:foundH = [IntPtr]::Zero
    $cb = [WinFind+EnumWindowsProc]{ param($w, $l)
        $pid2 = [uint32]0
        [WinFind]::GetWindowThreadProcessId($w, [ref]$pid2) | Out-Null
        if ($pid2 -eq $script:targetPid -and [WinFind]::IsWindowVisible($w)) { $script:foundH = $w; return $false }
        return $true
    }
    [WinFind]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    if ($script:foundH -ne [IntPtr]::Zero) { $h = $script:foundH; break }
}
if ($h -eq [IntPtr]::Zero) { Write-Error "no main window"; exit 1 }
[Win32]::ShowWindow($h, 9) | Out-Null
[Win32]::SetForegroundWindow($h) | Out-Null

# ---- click "single player", wait for scene load ----
Start-Sleep -Milliseconds 2500
[Win32]::SetForegroundWindow($h) | Out-Null
for ($i = 0; $i -lt 3; $i++) {
    Click-At $h 240 55
    Start-Sleep -Milliseconds 300
}
Start-Sleep -Milliseconds 2000   # wait for scene load
Save-Shot $h (Join-Path $OutDir "pf_1.png")   # enemy initial (patrolling near (10,10))

# ---- phase 1: hold A 1.6s, +X from x=0 to x~=8 (avoids (3,3) box) ----
[Win32]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 100
KeyDown 0x41   # A down
Start-Sleep -Milliseconds 1600
KeyUp 0x41     # A up
Start-Sleep -Milliseconds 300

# ---- phase 2: hold W 1.4s, +Z from z=0 to z~=7 -> player near (8,7), inside 5m detect ----
KeyDown 0x57   # W down
Start-Sleep -Milliseconds 1400
KeyUp 0x57     # W up
Start-Sleep -Milliseconds 500
Save-Shot $h (Join-Path $OutDir "pf_2.png")   # player in range, enemy starts A* chase

# ---- wait for enemy to follow the A* path to the player ----
Start-Sleep -Milliseconds 2000
Save-Shot $h (Join-Path $OutDir "pf_3.png")   # enemy chased to player

$proc.Refresh()
if (-not $proc.HasExited) { $proc.Kill() }

Write-Output "saved pf_1.png / pf_2.png / pf_3.png"
Write-Output "=== [pf] pathfinding logs ==="
$log = ""
if (Test-Path $outLog) { $log += (Get-Content $outLog -Raw) }
if (Test-Path $errLog) { $log += (Get-Content $errLog -Raw) }
$log -split "`n" | Where-Object { $_ -match "\[pf\]" } | ForEach-Object { Write-Output $_ }
