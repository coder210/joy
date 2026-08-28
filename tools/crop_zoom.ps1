# 裁剪放大截图区域，验证血环像素
param(
    [string]$Image,
    [string]$Out,
    [int]$Cx,
    [int]$Cy,
    [int]$Half,
    [int]$Scale
)
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing
$bmp = New-Object System.Drawing.Bitmap($Image)
$w = $bmp.Width
$h = $bmp.Height
$x0 = [Math]::Max(0, $Cx - $Half)
$y0 = [Math]::Max(0, $Cy - $Half)
$x1 = [Math]::Min($w, $Cx + $Half)
$y1 = [Math]::Min($h, $Cy + $Half)
$cw = $x1 - $x0
$ch = $y1 - $y0
$zoom = New-Object System.Drawing.Bitmap($cw * $Scale, $ch * $Scale)
$g = [System.Drawing.Graphics]::FromImage($zoom)
$g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
$g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
$src = New-Object System.Drawing.Rectangle($x0, $y0, $cw, $ch)
$dst = New-Object System.Drawing.Rectangle(0, 0, $cw * $Scale, $ch * $Scale)
$g.DrawImage($bmp, $dst, $src, [System.Drawing.GraphicsUnit]::Pixel)
$zoom.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)

# 统计深色像素（血环浓墨前景 40,40,45 叠加地面后约 55,55,60；敌人本体更暗 22,22,26）
$dark = 0
$ringLike = 0
for ($y = $y0; $y -lt $y1; $y++) {
    for ($x = $x0; $x -lt $x1; $x++) {
        $c = $bmp.GetPixel($x, $y)
        if ($c.R -lt 90 -and $c.G -lt 90 -and $c.B -lt 100) { $dark++ }
        if ($c.R -ge 45 -and $c.R -le 75 -and $c.G -ge 45 -and $c.G -le 75 -and $c.B -ge 50 -and $c.B -le 85) { $ringLike++ }
    }
}
Write-Output ("image {0}x{1} crop {2},{3} {4}x{5} dark={6} ringlike={7}" -f $w, $h, $x0, $y0, $cw, $ch, $dark, $ringLike)
$g.Dispose()
$bmp.Dispose()
$zoom.Dispose()
