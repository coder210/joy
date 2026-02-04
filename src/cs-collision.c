#include "liv-mono.h"
#include "liv-collision.h"
#include "csclib.h"


void csopen_collision()
{
        livS_mono_add_internal_call("Livlib.Native.Collision2dF::GetRayRectangle", collision2df_get_ray_rectangle);
        livS_mono_add_internal_call("Livlib.Native.Collision2dF::GetRayRectanglex", collision2df_get_ray_rectanglex);
        livS_mono_add_internal_call("Livlib.Native.Collision2dF::Circles", collision2df_check_circles);
        livS_mono_add_internal_call("Livlib.Native.Collision2dF::Rectangles", collision2df_check_rectangles);
        livS_mono_add_internal_call("Livlib.Native.Collision2dF::Circlesx", collision2df_get_circles);
        livS_mono_add_internal_call("Livlib.Native.Collision2dF::Rectanglesx", collision2df_get_rectangles);
        livS_mono_add_internal_call("Livlib.Native.Collision2dF::Polygons", collision2df_get_polygons);

        livS_mono_add_internal_call("Livlib.Native::Collision3dF::RayQuad", collision3df_get_ray_quad);
}
