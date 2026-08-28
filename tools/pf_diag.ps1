# Diagnostic: launch game, find window, print rects, take screenshot
param(
    [string]$Exe = "D:\github\joy\build\Debug\project_proxy3d.exe"
)
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class DWin {
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
}
"@

$proc = Start-Process -FilePath $Exe -ArgumentList @("--mode=single") -WorkingDirectory (Split-Path $Exe) -PassThru
$script:targetPid = $proc.Id
$h = [IntPtr]::Zero
for ($i = 0; $i -lt 100 -and $h -eq [IntPtr]::Zero; $i++) {
    Start-Sleep -Milliseconds 200
    $proc.Refresh()
    if ($proc.MainWindowHandle -ne [IntPtr]::Zero) { $h = $proc.MainWindowHandle; break }
    $script:f = [IntPtr]::Zero
    $cb = [DWin+EnumWindowsProc]{ param($w, $l)
        $pid2 = [uint32]0
        [DWin]::GetWindowThreadProcessId($w, [ref]$pid2) | Out-Null
        if ($pid2 -eq $script:targetPid -and [DWin]::IsWindowVisible($w)) { $script:f = $w; return $false }
        return $true
    }
    [DWin]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    if ($script:f -ne [IntPtr]::Zero) { $h = $script:f; break }
}
if ($h -eq [IntPtr]::Zero) { Write-Output "NO WINDOW FOUND"; $proc.Kill(); exit 1 }
Write-Output "window handle: $h"
[DWin]::ShowWindow($h, 9) | Out-Null
Start-Sleep -Milliseconds 2000
$r = New-Object DWin+RECT
[DWin]::GetWindowRect($h, [ref]$r) | Out-Null
$cr = New-Object DWin+RECT
[DWin]::GetClientRect($h, [ref]$cr) | Out-Null
Write-Output ("window rect: L={0} T={1} R={2} B={3}  w={4} h={5}" -f $r.Left, $r.Top, $r.Right, $r.Bottom, ($r.Right - $r.Left), ($r.Bottom - $r.Top))
Write-Output ("client rect: w={0} h={1}" -f $cr.Right, $cr.Bottom)
Write-Output ("visible={0} iconic={1} hasExited={2}" -f [DWin]::IsWindowVisible($h), [DWin]::IsIconic($h), $proc.HasExited)

$w = $r.Right - $r.Left; $hh = $r.Bottom - $r.Top
if ($w -gt 0 -and $hh -gt 0) {
    $bmp = New-Object System.Drawing.Bitmap($w, $hh)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size($w, $hh)))
    $out = "D:\github\joy\tools\pf_shots\diag.png"
    $bmp.Save($out, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    Write-Output "saved diag.png"
} else {
    Write-Output "BAD RECT - cannot screenshot"
}
if (-not $proc.HasExited) { $proc.Kill() }
