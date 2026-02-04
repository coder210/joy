//#include <SDL3/SDL.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
////'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "external/stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */
//
//#define STB_TRUETYPE_IMPLEMENTATION
//#include "external/stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */
//
//
//int main(int argc, const char* argv[])
//{
//        /* 加载字体（.ttf）文件 */
//        long int size = 0;
//        unsigned char* fontBuffer = NULL;
//
//        FILE* fontFile = fopen("C:\\Windows\\Fonts\\simhei.ttf", "rb");
//        if (fontFile == NULL)
//        {
//                printf("Can not open font file!\n");
//                return 0;
//        }
//        fseek(fontFile, 0, SEEK_END); /* 设置文件指针到文件尾，基于文件尾偏移0字节 */
//        size = ftell(fontFile);       /* 获取文件大小（文件尾 - 文件头  单位：字节） */
//        fseek(fontFile, 0, SEEK_SET); /* 重新设置文件指针到文件头 */
//
//        fontBuffer = calloc(size, sizeof(unsigned char));
//        fread(fontBuffer, size, 1, fontFile);
//        fclose(fontFile);
//
//        /* 初始化字体 */
//        stbtt_fontinfo info;
//        if (!stbtt_InitFont(&info, fontBuffer, 0))
//        {
//                printf("stb init font failed\n");
//        }
//
//        SDL_Color color;
//
//        // 设置文本颜色
//        color.r = 255;
//        color.g = 0;
//        color.b = 0;
//        color.a = 255;
//
//        unsigned short word2[20];
//        //int len2 = utf8_to_unicode('世', word2);
//
//
//        /* "STB"的 unicode 编码 */
//        int word[20] = { 0x6211, 0x754C };
//        int len = 2;
//
//        /* 计算字体缩放 */
//        float pixels = 60.0;                                    /* 字体大小（字号） */
//        float scale = stbtt_ScaleForPixelHeight(&info, pixels); /* scale = pixels / (ascent - descent) */
//
//        /**
//         * 获取垂直方向上的度量
//         * ascent：字体从基线到顶部的高度；
//         * descent：基线到底部的高度，通常为负值；
//         * lineGap：两个字体之间的间距；
//         * 行间距为：ascent - descent + lineGap。
//        */
//        int ascent = 0;
//        int descent = 0;
//        int lineGap = 0;
//        stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
//
//        /* 根据缩放调整字高 */
//        ascent = roundf(ascent * scale);
//        descent = roundf(descent * scale);
//
//        /* 创建位图 */
//        int bitmap_w = pixels * len; /* 位图的宽 */
//        int bitmap_h = pixels; /* 位图的高 */
//        unsigned char* bitmap = calloc(bitmap_w * bitmap_h, sizeof(unsigned char));
//
//        int x = 0; /*位图的x*/
//
//        /* 循环加载word中每个字符 */
//        for (int i = 0; i < len; ++i)
//        {
//                /**
//                  * 获取水平方向上的度量
//                  * advanceWidth：字宽；
//                  * leftSideBearing：左侧位置；
//                */
//                int advanceWidth = 0;
//                int leftSideBearing = 0;
//                stbtt_GetCodepointHMetrics(&info, word[i], &advanceWidth, &leftSideBearing);
//
//                /* 获取字符的边框（边界） */
//                int c_x1, c_y1, c_x2, c_y2;
//                stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
//
//                /* 计算位图的y (不同字符的高度不同） */
//                int y = ascent + c_y1;
//
//                /* 渲染字符 */
//                int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
//                stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word[i]);
//
//                /* 调整x */
//                x += roundf(advanceWidth * scale);
//
//                /* 调整字距 */
//                int kern;
//                kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
//                x += roundf(kern * scale);
//        }
//
//        // 初始化SDL
//        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//                fprintf(stderr, "SDL初始化失败: %s\n", SDL_GetError());
//                return 1;
//        }
//
//        // 创建窗口和渲染器
//        SDL_Window* window = SDL_CreateWindow("SDL3中文渲染", 800, 600, 0);
//        if (!window) {
//                fprintf(stderr, "窗口创建失败: %s\n", SDL_GetError());
//                SDL_Quit();
//                return 1;
//        }
//
//        SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
//        if (!renderer) {
//                fprintf(stderr, "渲染器创建失败: %s\n", SDL_GetError());
//                SDL_DestroyWindow(window);
//                SDL_Quit();
//                return 1;
//        }
//
//
//        /* 将位图数据保存到1通道的png图像中 */
//        //stbi_write_png("D:\\STB.png", bitmap_w, bitmap_h, 1, bitmap, bitmap_w);
//         // 创建纹理
//
//        SDL_Surface* surface = SDL_CreateSurface(bitmap_w, bitmap_h, SDL_PIXELFORMAT_RGBA8888);
//        if (!surface) {
//                printf("无法创建表面: %s\n", SDL_GetError());
//                free(bitmap);
//                return NULL;
//        }
//
//        Uint32* surface_pixels = (Uint32*)surface->pixels;
//        int pitch = surface->pitch / sizeof(Uint32);
//
//        //灰度转RGBA（带透明度）
//        for (int y = 0; y < bitmap_h; ++y) {
//                for (int x = 0; x < bitmap_w; ++x) {
//                        Uint8 alpha = bitmap[y * bitmap_w + x];
//                        // 正确设置RGBA值（白色文字，alpha通道来自位图）
//                        //surface_pixels[y * pitch + x] = (Uint32)((0x00FFFFFF) | (alpha << 24));
//                        //surface_pixels[y * pitch + x] = (Uint32)((0x00FFFFFF) | (alpha << 24));
//                        surface_pixels[y * pitch + x] = alpha;
//                }
//        }
//
//        //for (int y = 0; y < bitmap_h; ++y) {
//        //        for (int x = 0; x < bitmap_w; ++x) {
//        //                //Uint8 alpha = bitmap[y * bitmap_w + x];
//        //                // 正确设置RGBA值（白色文字，alpha通道来自位图）
//        //                //surface_pixels[y * pitch + x] = (Uint32)((0x00FFFFFF) | (alpha << 24));
//        //                //surface_pixels[y * pitch + x] = (Uint32)((0x00FFFFFF) | (alpha << 24));
//        //                int bitmap_idx = y * bitmap_w + x;
//        //                int pixel_idx = y * (pitch / 4) + x;
//        //                surface_pixels[pixel_idx * 4 + 0] = color.r;     // R
//        //                surface_pixels[pixel_idx * 4 + 1] = color.g;     // G
//        //                surface_pixels[pixel_idx * 4 + 2] = color.b;     // B
//        //                surface_pixels[pixel_idx * 4 + 3] = bitmap[bitmap_idx];  // A
//        //        }
//        //}
//        
//
//        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//
//        SDL_Event event;
//        bool quit = false;
//
//        stbi_write_png("D:\\STB.png", bitmap_w, bitmap_h, 1, bitmap, bitmap_w);
//        free(info.data);
//        free(bitmap);
//        while (!quit) {
//                while (SDL_PollEvent(&event)) {
//                        if (event.type == SDL_EVENT_QUIT) quit = 1;
//                }
//
//                // 清屏
//                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//                SDL_RenderClear(renderer);
//                SDL_FRect dest = { 0, 0, bitmap_w, bitmap_h };
//                SDL_RenderTexture(renderer, texture, NULL, &dest);
//                SDL_RenderPresent(renderer);
//                SDL_Delay(16);
//        }
//
//        
//        SDL_DestroyRenderer(renderer);
//        SDL_DestroyWindow(window);
//        SDL_Quit();
//        return 0;
//}