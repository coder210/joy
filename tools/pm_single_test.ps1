# Quick single-mode map check: launch, single click start, screenshot + [pmap] log.
param(
    [string]$Exe = "D:\github\joy\build\Debug\project_proxy3d.exe",
    [string]$OutDir = "D:\github\joy\tools\pm_shots",
    [string]$Tag = "s1"
)
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

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
function Press-Key($vk) {
    [W32]::keybd_event($vk, 0, 0, [UIntPtr]::Zero)       # down
    Start-Sleep -Milliseconds 60
    [W32]::keybd_event($vk, 0, 2, [UIntPtr]::Zero)       # up (KEYEVENTF_KEYUP)
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

$outLog = Join-Path $OutDir "$Tag.out.log"
$errLog = Join-Path $OutDir "$Tag.err.log"
$proc = Start-Process -FilePath $Exe -ArgumentList @("--mode=single") -WorkingDirectory (Split-Path $Exe) `
        -RedirectStandardOutput $outLog -RedirectStandardError $errLog -PassThru

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
$h = [IntPtr]::Zero
$script:tpid = $proc.Id
for ($i = 0; $i -lt 100 -and $h -eq [IntPtr]::Zero; $i++) {
    Start-Sleep -Milliseconds 200
    $proc.Refresh()
    if ($proc.MainWindowHandle -ne [IntPtr]::Zero) { $h = $proc.MainWindowHandle; break }
    $script:fH = [IntPtr]::Zero
    $cb = [WFind+EnumWindowsProc]{ param($w, $l)
        $p2 = [uint32]0
        [WFind]::GetWindowThreadProcessId($w, [ref]$p2) | Out-Null
        if ($p2 -eq $script:tpid -and [WFind]::IsWindowVisible($w)) { $script:fH = $w; return $false }
        return $true
    }
    [WFind]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    if ($script:fH -ne [IntPtr]::Zero) { $h = $script:fH; break }
}
if ($h -eq [IntPtr]::Zero) { Write-Error "no window"; exit 1 }
[W32]::ShowWindow($h, 9) | Out-Null
[W32]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 3000
Save-Shot $h (Join-Path $OutDir "$Tag.menu.png")

# SPACE 键进入游戏（主菜单支持 SPACE 直接开始）
[W32]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 300
Press-Key 0x20
Start-Sleep -Milliseconds 4000
Save-Shot $h (Join-Path $OutDir "$Tag.game.png")

$proc.Refresh()
if (-not $proc.HasExited) { $proc.Kill() }

Write-Output "done"
$log = ""
if (Test-Path $outLog) { $log += (Get-Content $outLog -Raw) }
if (Test-Path $errLog) { $log += (Get-Content $errLog -Raw) }
$log -split "`n" | Where-Object { $_ -match "\[pmap\]" } | ForEach-Object { Write-Output $_ }
