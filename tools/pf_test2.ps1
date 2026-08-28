# Verify A* chase with log capture + crash detection
param(
    [string]$Exe = "D:\github\joy\build\Debug\project_proxy3d.exe",
    [string]$OutDir = "D:\github\joy\tools\pf_shots"
)
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$errLog = "D:\github\joy\tools\pf_game_err.log"
$outLog = "D:\github\joy\tools\pf_game_out.log"
Remove-Item $errLog, $outLog -ErrorAction SilentlyContinue

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W32 {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, out POINT lpPoint);
    [DllImport("user32.dll")] public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern void mouse_event(uint dwFlags, int dx, int dy, uint dwData, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int X, int Y);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
}
"@
function Get-Rect($h) { $r = New-Object W32+RECT; [W32]::GetWindowRect($h, [ref]$r) | Out-Null; return $r }
function Click-At($h, $logicalX, $logicalY) {
    $cr = New-Object W32+RECT; [W32]::GetClientRect($h, [ref]$cr) | Out-Null
    $p = New-Object W32+POINT; $p.X = 0; $p.Y = 0
    [W32]::ClientToScreen($h, [ref]$p) | Out-Null
    $sx = $p.X + [int]($logicalX * ($cr.Right / 480.0))
    $sy = $p.Y + [int]($logicalY * ($cr.Bottom / 320.0))
    [W32]::SetCursorPos($sx, $sy) | Out-Null
    Start-Sleep -Milliseconds 60
    [W32]::mouse_event(0x0002, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 40
    [W32]::mouse_event(0x0004, 0, 0, 0, [UIntPtr]::Zero)
}
function Save-Shot($h, $path) {
    if ($h -eq [IntPtr]::Zero) { Write-Output "  [save] no handle"; return $false }
    $r = Get-Rect $h
    $w = $r.Right - $r.Left; $hh = $r.Bottom - $r.Top
    if ($w -le 0 -or $hh -le 0) { Write-Output "  [save] bad rect w=$w h=$hh"; return $false }
    $bmp = New-Object System.Drawing.Bitmap($w, $hh)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size($w, $hh)))
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    Write-Output "  [save] OK $path"
    return $true
}
function Check-Alive($proc, $tag) {
    $proc.Refresh()
    if ($proc.HasExited) { Write-Output "[CHECK] $tag : PROCESS EXITED code=$($proc.ExitCode)"; return $false }
    Write-Output "[CHECK] $tag : alive"; return $true
}

$proc = Start-Process -FilePath $Exe -ArgumentList @("--mode=single") -WorkingDirectory (Split-Path $Exe) -RedirectStandardError $errLog -RedirectStandardOutput $outLog -PassThru
$script:targetPid = $proc.Id
$h = [IntPtr]::Zero
for ($i = 0; $i -lt 100 -and $h -eq [IntPtr]::Zero; $i++) {
    Start-Sleep -Milliseconds 200
    $proc.Refresh()
    if ($proc.MainWindowHandle -ne [IntPtr]::Zero) { $h = $proc.MainWindowHandle; break }
    $script:f = [IntPtr]::Zero
    $cb = [W32+EnumWindowsProc]{ param($w, $l)
        $pid2 = [uint32]0
        [W32]::GetWindowThreadProcessId($w, [ref]$pid2) | Out-Null
        if ($pid2 -eq $script:targetPid -and [W32]::IsWindowVisible($w)) { $script:f = $w; return $false }
        return $true
    }
    [W32]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    if ($script:f -ne [IntPtr]::Zero) { $h = $script:f; break }
}
if ($h -eq [IntPtr]::Zero) { Write-Output "NO WINDOW"; $proc.Kill(); exit 1 }
[W32]::ShowWindow($h, 9) | Out-Null
[W32]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 1500

Write-Output "=== click single-player ==="
Start-Sleep -Milliseconds 2500
[W32]::SetForegroundWindow($h) | Out-Null
for ($i = 0; $i -lt 3; $i++) { Click-At $h 240 55; Start-Sleep -Milliseconds 300 }
Start-Sleep -Milliseconds 2000
Check-Alive $proc "after click"
Save-Shot $h (Join-Path $OutDir "pf_1.png")

Write-Output "=== move player S+D 3s ==="
[W32]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 100
[W32]::keybd_event(0x53, 0, 0, [UIntPtr]::Zero)
[W32]::keybd_event(0x44, 0, 0, [UIntPtr]::Zero)
Start-Sleep -Milliseconds 3000
[W32]::keybd_event(0x53, 0, 2, [UIntPtr]::Zero)
[W32]::keybd_event(0x44, 0, 2, [UIntPtr]::Zero)
Start-Sleep -Milliseconds 3000
Check-Alive $proc "after chase"
Save-Shot $h (Join-Path $OutDir "pf_2.png")

$proc.Refresh()
if (-not $proc.HasExited) { $proc.Kill() }
Write-Output "=== logs (err) ==="
if (Test-Path $errLog) { Get-Content $errLog -Tail 40 }
Write-Output "=== logs (out) ==="
if (Test-Path $outLog) { Get-Content $outLog -Tail 10 }
