using System.Collections.Generic;

namespace Livlib
{
    public class Tileset
    {
        public string Name { get; set; }
        public int FirstGid { get; set; }
        public string Class { get; set; }
        public int TileWidth { get; set; }
        public int TileHeight { get; set; }
        public int Spacing { get; set; }
        public int Margin { get; set; }
        public int Columns { get; set; }
        public string Image { get; set; }
        public int ImageWidth { get; set; }
        public int ImageHeight { get; set; }
        public string ObjectAlignment { get; set; }
        public string TileRenderSize { get; set; }
        public string FillMode { get; set; }
    }

  

    public class Grid
    {
        public string Orientation { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
    }

    public class Property
    {

    }

    public class Wangset
    {

    }

    public class Animation
    {
        public int TileId { get; set; }
        public int Duration { get; set; }
    }

    public class Tile
    {
        public int Id { get; set; }
        public List<Animation> Animation { get; set; }
        public string Image { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
    }

    public class LayerObject
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string Type { get; set; }
        public string Shape { get; set; }
        public int X { get; set; }
        public int Y { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public int Rotation { get; set; }
        public bool Visible { get; set; }
        public List<Property> Properties { get; set; }
    }

    public class Layer
    {
        public string Type { get; set; }
        public string DrawOrder { get; set; }
        /// <summary>
        /// imagelayer
        /// </summary>
        public string Image { get; set; }
        public int X { get; set; }
        public int Y { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public int Id { get; set; }
        public string Name { get; set; }
        public string Class { get; set; }
        public bool Visible { get; set; }
        public int Opacity { get; set; }
        public int OffsetX { get; set; }
        public int OffsetY { get; set; }
        public int ParallaxX { get; set; }
        public int ParallaxY { get; set; }
        public int RepeatX { get; set; }
        public int RepeatY { get; set; }
        public int[] Tintcolor { get; set; }
        public List<Property> Properties { get; set; }
        public List<LayerObject> Objects { get; set; }
        public string Encoding { get; set; }
        public short[] Data { get; set; }
    }

    public class Tilemap
    {
        public struct Vec2
        {
            public int X { get; set; }
            public int Y { get; set; }
        }

        public string Version { get; set; }
        public string LuaVersion { get; set; }
        public string TiledVersion { get; set; }
        public string Class { get; set; }
        public string Orientation { get; set; }
        public string RenderOrder { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }
        public int TileWidth { get; set; }
        public int TileHeight { get; set; }
        public int NextLayerId { get; set; }
        public int NextObjectId { get; set; }
        public List<Tileset> Tilesets { get; set; }
        public Vec2 TileOffset { get; set; }
        public Grid Grid { get; set; }
        public List<Property> Properties { get; set; }
        public List<Wangset> Wangsets { get; set; }
        public int TileCount { get; set; }
        public List<Tile> Tiles { get; set; }
        public List<Layer> Layers { get; set; }
    }

}
