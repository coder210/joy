# 手动启动 server 模式，隔 2 秒截两张图对比（验证场景动画与血环）
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W2 {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L,T,R,B; }
}
"@

$exe = 'D:\github\joy\build\Debug\project_proxy3d.exe'
$p = Start-Process -FilePath $exe -ArgumentList @('--mode=server') -WorkingDirectory (Split-Path $exe) -PassThru
Start-Sleep -Seconds 3
$p.Refresh()
$h = $p.MainWindowHandle
if ($h -eq [IntPtr]::Zero) { Write-Output 'no window'; exit 1 }
[W2]::ShowWindow($h, 9) | Out-Null
[W2]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 800

function Save-Shot2($h, $path) {
    $r = New-Object W2+RECT
    [W2]::GetWindowRect($h, [ref]$r) | Out-Null
    $w = $r.R - $r.L; $hh = $r.B - $r.T
    $bmp = New-Object System.Drawing.Bitmap($w, $hh)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($r.L, $r.T, 0, 0, (New-Object System.Drawing.Size($w, $hh)))
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
}

Save-Shot2 $h 'D:\github\joy\tools\hp_shots\live_1.png'
Start-Sleep -Seconds 2
Save-Shot2 $h 'D:\github\joy\tools\hp_shots\live_2.png'
Start-Sleep -Milliseconds 300
$p.Refresh()
if (-not $p.HasExited) { $p.Kill() }
Write-Output "done: live_1/live_2 captured, mainwin=$h"
