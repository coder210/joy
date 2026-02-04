using Livlib.FixMath;

namespace Livlib.Collision
{
    public class FixAABB
    {
        public FixVec2 Min { get; set; }
        public FixVec2 Max { get;set; }

        public FixAABB(FixVec2 min, FixVec2 max)
        {
            this.Min = min;
            this.Max = max;
        }

        public FixAABB(float minX, float minY, float maxX, float maxY)
        {
            this.Min = new FixVec2(minX, minY);
            this.Max = new FixVec2(maxX, maxY);
        }
    }
}
