# 自动化验证脚下血环：启动单机模式 -> 点击单人游戏 -> 连续截屏
param(
    [string]$Exe = "D:\github\joy\build\Debug\project_proxy3d.exe",
    [string]$OutDir = "D:\github\joy\tools\hp_shots",
    [string]$Mode = "single"
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
    [Win32]::mouse_event(0x0002, 0, 0, 0, [UIntPtr]::Zero)   # LEFTDOWN
    Start-Sleep -Milliseconds 40
    [Win32]::mouse_event(0x0004, 0, 0, 0, [UIntPtr]::Zero)   # LEFTUP
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

# ---- 启动游戏（必须指定工作目录，否则找不到 shader/音频资源会立即退出） ----
$argList = @("--mode=single")
if ($Mode -eq "server") { $argList = @("--mode=server") }
$proc = Start-Process -FilePath $Exe -ArgumentList $argList -WorkingDirectory (Split-Path $Exe) -PassThru

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
    if ($proc.MainWindowHandle -ne [IntPtr]::Zero) {
        $h = $proc.MainWindowHandle
        break
    }
    $script:foundH = [IntPtr]::Zero
    $cb = [WinFind+EnumWindowsProc]{ param($w, $l)
        $pid2 = [uint32]0
        [WinFind]::GetWindowThreadProcessId($w, [ref]$pid2) | Out-Null
        if ($pid2 -eq $script:targetPid -and [WinFind]::IsWindowVisible($w)) {
            $script:foundH = $w
            return $false
        }
        return $true
    }
    [WinFind]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    if ($script:foundH -ne [IntPtr]::Zero) { $h = $script:foundH; break }
}
if ($h -eq [IntPtr]::Zero) { Write-Error "no main window"; exit 1 }
Write-Output ("handle found: {0}" -f $h)
[Win32]::ShowWindow($h, 9) | Out-Null          # SW_RESTORE
[Win32]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 1500
$proc.Refresh()
Write-Output ("exited={0} mainwin={1}" -f $proc.HasExited, $proc.MainWindowHandle)
$r0 = Get-Rect $h
Write-Output ("win rect: L={0} T={1} R={2} B={3} iconic={4}" -f $r0.Left, $r0.Top, $r0.Right, $r0.Bottom, [Win32]::IsIconic($h))
if ($r0.Right -le $r0.Left -or $r0.Bottom -le $r0.Top) {
    Write-Output "invalid rect, retrying restore..."
    [Win32]::ShowWindow($h, 5) | Out-Null   # SW_SHOW
    [Win32]::SetForegroundWindow($h) | Out-Null
    Start-Sleep -Milliseconds 1500
    $r0 = Get-Rect $h
    Write-Output ("win rect2: L={0} T={1} R={2} B={3}" -f $r0.Left, $r0.Top, $r0.Right, $r0.Bottom)
}
if ($r0.Right -le $r0.Left -or $r0.Bottom -le $r0.Top) { Write-Error "still invalid rect"; exit 1 }

# ---- 点击"单人游戏"（逻辑坐标 240,55），仅 single 模式需要 ----
if ($Mode -eq "single") {
    $activated = [Win32]::SetForegroundWindow($h)
    Write-Output ("SetForegroundWindow={0}" -f $activated)
    Click-At $h 240 55
    Start-Sleep -Milliseconds 400
    Click-At $h 240 55
}
Start-Sleep -Milliseconds 2500   # 等待场景加载 + 敌人生成

# ---- 连续截屏 ----
$frames = 0
for ($i = 0; $i -lt 12; $i++) {
    $frames++
    if (Save-Shot $h (Join-Path $OutDir ("hp_{0:D2}.png" -f $i))) {
        Write-Output ("saved frame {0}" -f $i)
    }
    Start-Sleep -Milliseconds 100
}

Start-Sleep -Milliseconds 300
$proc.Refresh()
if (-not $proc.HasExited) { $proc.Kill() }
Write-Output "captured $frames frames to $OutDir"
