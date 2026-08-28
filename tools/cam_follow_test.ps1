# 验证相机跟随：单机模式 -> 玩家移动前后，玩家是否始终在屏幕中心
param(
    [string]$Exe = "D:\github\joy\build\Debug\project_proxy3d.exe",
    [string]$OutDir = "D:\github\joy\tools\cam_shots"
)
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class Win32 {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, out POINT lpPoint);
    [DllImport("user32.dll")] public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern void mouse_event(uint dwFlags, int dx, int dy, uint dwData, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int X, int Y);
    [DllImport("user32.dll")] public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr hWnd);
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

# ---- 启动游戏（single 模式） ----
$proc = Start-Process -FilePath $Exe -ArgumentList @("--mode=single") -WorkingDirectory (Split-Path $Exe) -PassThru

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
Start-Sleep -Milliseconds 1500

# ---- 点击"单人游戏"（等待 splash 完全结束后多点几次） ----
Start-Sleep -Milliseconds 2500
[Win32]::SetForegroundWindow($h) | Out-Null
for ($i = 0; $i -lt 3; $i++) {
    Click-At $h 240 55
    Start-Sleep -Milliseconds 300
}
Start-Sleep -Milliseconds 2000   # 等场景加载 + 敌人生成
Save-Shot $h (Join-Path $OutDir "cam_entered.png")   # 确认已进入游戏（非菜单）

# ---- 初始帧（玩家出生在 (0,0)，相机应使其居中） ----
Save-Shot $h (Join-Path $OutDir "cam_before.png")

# ---- 按住 W 2 秒（向 -Z 移动，屏幕上方），随后松开 ----
[Win32]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 100
[Win32]::keybd_event(0x57, 0, 0, [UIntPtr]::Zero)   # W down
Start-Sleep -Milliseconds 2000
[Win32]::keybd_event(0x57, 0, 2, [UIntPtr]::Zero)   # W up

# ---- 等相机平滑追上，再截帧 ----
Start-Sleep -Milliseconds 1200
Save-Shot $h (Join-Path $OutDir "cam_after.png")

$proc.Refresh()
if (-not $proc.HasExited) { $proc.Kill() }
Write-Output "saved cam_before.png / cam_after.png"
