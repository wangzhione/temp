// UNICODE 让 Windows API 默认使用宽字符版本，保证中文窗口标题正常显示
#define UNICODE
#define _UNICODE

// WIN32_LEAN_AND_MEAN 约定 windows.h 中部分不常用的 Windows API 不导入
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// PI 表示圆周率，用于角度、旋转和正弦动画计算
#define PI 3.14159265358979323846

// TIMER_ID 把“动画刷新定时器”的 ID 定义为 1
#define TIMER_ID 1

// g_frame 动画帧计数器；每次定时器触发时加 1，达到最大值后自动回到 0
static unsigned int g_frame = 0;

// star 保存一颗星星的位置、大小和闪烁相位
typedef struct {
    int x, y;  // x、y 使用固定基准坐标，窗口缩放时再换算成实际坐标
    int r;     // r 表示星星半径
    int phase; // phase 表示闪烁动画的初始相位
} star;

// g_stars 保存 90 颗星星各自的位置、大小和闪烁相位
static star g_stars[90];

// music_step 表示旋律中的一个播放步骤。
// 每个步骤可以同时播放一个主音和两个和声音，形成简单的三音和弦。
// melody、harmony1、harmony2 保存声音频率，单位都是赫兹 Hz。
// beats 保存这个步骤持续的拍数，实际持续时间会根据音乐速度 BPM 计算。
typedef struct {
    // melody 表示当前步骤的主旋律频率，单位是赫兹 Hz。
    // 例如：
    // 261.63 Hz 表示中央 C，也就是 C4；
    // 440.00 Hz 表示标准音 A4。
    // 生成 WAV 数据时，主旋律通常使用相对明显的音量。
    // 如果值为 0，则表示当前步骤不播放主旋律。
    double melody;

    // harmony1 表示第一层和声的频率，单位是赫兹 Hz。
    // 它会与 melody 主旋律同时播放，用于丰富声音层次。
    // 通常选择与主旋律协调的音，例如三度音或五度音。
    // 如果值为 0，则表示当前步骤不播放第一层和声。
    double harmony1;

    // harmony2 表示第二层和声的频率，单位是赫兹 Hz。
    // 它会与 melody 和 harmony1 同时叠加，组成简单的三音和弦。
    // 第二层和声的音量一般应更低，避免盖过主旋律。
    // 如果值为 0，则表示当前步骤不播放第二层和声。
    double harmony2;

    // beats 表示当前步骤持续多少拍。
    // 它不是秒数，必须结合音乐速度 BPM 换算成实际持续时间。
    //
    // 例如音乐速度为 70 BPM：
    // 1 拍持续时间 = 60 / 70 秒；
    // 2 拍持续时间 = 2 * 60 / 70 秒；
    // 0.5 拍持续时间 = 0.5 * 60 / 70 秒。
    //
    // 使用 double 类型可以表示 0.5 拍、1.5 拍等非整数节拍。
    double beats;
} music_step;

// g_music_wav 保存程序运行期间使用的完整 WAV 数据。
// 音乐由 PlaySoundW 直接从这块内存循环播放，不会写入临时文件。
// 因为使用了异步播放，所以必须一直保留到程序退出、停止播放以后才能释放。
static unsigned char * g_music_wav = NULL;


// 把 16 位整数按 WAV 需要的小端字节顺序写入内存
static void write_u16_le(unsigned char *dst, uint16_t value) {
    dst[0] = (unsigned char)(value & 0xffu);
    dst[1] = (unsigned char)((value >> 8) & 0xffu);
}

// 把 32 位整数按 WAV 需要的小端字节顺序写入内存
static void write_u32_le(unsigned char *dst, uint32_t value) {
    dst[0] = (unsigned char)(value & 0xffu);
    dst[1] = (unsigned char)((value >> 8) & 0xffu);
    dst[2] = (unsigned char)((value >> 16) & 0xffu);
    dst[3] = (unsigned char)((value >> 24) & 0xffu);
}

// 根据旋律表在堆内存中合成一段低音量 WAV 音乐
static BOOL create_music_data(void) {
    const int sample_rate = 22050;
    const int bits_per_sample = 16;
    const int channels = 1;
    const double bpm = 70.0;

    // melody 保存整段舒缓旋律；较低频率的两个音组成背景和声
    static const music_step melody[] = {
        { 523.25, 261.63, 196.00, 2.0 },
        { 659.25, 261.63, 196.00, 2.0 },
        { 783.99, 261.63, 196.00, 2.0 },
        { 659.25, 261.63, 196.00, 2.0 },
        { 587.33, 293.66, 220.00, 2.0 },
        { 698.46, 293.66, 220.00, 2.0 },
        { 880.00, 293.66, 220.00, 2.0 },
        { 698.46, 293.66, 220.00, 2.0 },
        { 659.25, 261.63, 196.00, 2.0 },
        { 783.99, 261.63, 196.00, 2.0 },
        {1046.50, 261.63, 196.00, 2.0 },
        { 783.99, 261.63, 196.00, 2.0 },
        { 587.33, 196.00, 146.83, 2.0 },
        { 698.46, 196.00, 146.83, 2.0 },
        { 659.25, 261.63, 196.00, 2.0 },
        { 523.25, 261.63, 196.00, 2.0 }
    };

    size_t step_count = sizeof(melody) / sizeof(melody[0]);
    size_t total_samples = 0;

    for (size_t i = 0; i < step_count; ++i) {
        double seconds = melody[i].beats * 60.0 / bpm;
        total_samples += (size_t)(seconds * sample_rate);
    }

    size_t data_bytes = total_samples * channels * (bits_per_sample / 8);
    size_t wav_size = 44 + data_bytes;

    // WAV 文件小于 2 MB，DWORD 足以保存文件长度
    if (wav_size > 0xffffffffu) return FALSE;

    unsigned char *wav = (unsigned char *)malloc(wav_size);
    if (!wav) return FALSE;

    // 写入标准 PCM WAV 文件头
    memcpy(wav + 0, "RIFF", 4);
    write_u32_le(wav + 4, (uint32_t)(36 + data_bytes));
    memcpy(wav + 8, "WAVE", 4);
    memcpy(wav + 12, "fmt ", 4);
    write_u32_le(wav + 16, 16);
    write_u16_le(wav + 20, 1);
    write_u16_le(wav + 22, (uint16_t)channels);
    write_u32_le(wav + 24, (uint32_t)sample_rate);
    write_u32_le(wav + 28, (uint32_t)(sample_rate * channels * bits_per_sample / 8));
    write_u16_le(wav + 32, (uint16_t)(channels * bits_per_sample / 8));
    write_u16_le(wav + 34, (uint16_t)bits_per_sample);
    memcpy(wav + 36, "data", 4);
    write_u32_le(wav + 40, (uint32_t)data_bytes);

    size_t write_offset = 44;

    for (size_t step = 0; step < step_count; ++step) {
        double seconds = melody[step].beats * 60.0 / bpm;
        size_t step_samples = (size_t)(seconds * sample_rate);

        for (size_t i = 0; i < step_samples; ++i) {
            double t = (double)i / sample_rate;
            double remaining = seconds - t;
            double envelope = 1.0;

            // 每个音缓慢淡入淡出，减少音与音衔接时的爆音
            if (t < 0.10) envelope *= t / 0.10;
            if (remaining < 0.30) envelope *= remaining / 0.30;
            if (envelope < 0.0) envelope = 0.0;

            double tremolo = 0.94 + 0.06 * sin(2.0 * PI * 0.35 * t);
            double value =
                0.50 * sin(2.0 * PI * melody[step].melody * t) +
                0.18 * sin(2.0 * PI * melody[step].harmony1 * t) +
                0.14 * sin(2.0 * PI * melody[step].harmony2 * t) +
                0.08 * sin(2.0 * PI * melody[step].harmony1 * 0.5 * t);

            int16_t sample = (int16_t)(value * envelope * tremolo * 7000.0);
            write_u16_le(wav + write_offset, (uint16_t)sample);
            write_offset += 2;
        }
    }

    // 保存 WAV 内存地址；异步播放期间不能释放这块内存
    g_music_wav = wav;
    return TRUE;
}

// 异步循环播放内存中的背景音乐，不阻塞窗口动画和消息循环
static BOOL start_music(void) {
    if (!g_music_wav && !create_music_data()) return FALSE;

    return PlaySoundW(
        (LPCWSTR)g_music_wav,
        NULL,
        SND_MEMORY | SND_ASYNC | SND_LOOP | SND_NODEFAULT
    );
}

// 程序退出时先停止播放，再释放保存 WAV 数据的堆内存
static void cleanup_music(void) {
    PlaySoundW(NULL, NULL, 0);

    free(g_music_wav);
    g_music_wav = NULL;
}

// 把整数 v 限制在 [lo, hi] 范围内
static int clamp_int(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// 按比例 t 混合颜色 a 和颜色 b，用于绘制渐变色
static COLORREF mix_color(COLORREF a, COLORREF b, double t) {
    int ar = GetRValue(a), ag = GetGValue(a), ab = GetBValue(a);
    int br = GetRValue(b), bg = GetGValue(b), bb = GetBValue(b);
    int r = (int)(ar + (br - ar) * t);
    int g = (int)(ag + (bg - ag) * t);
    int bl = (int)(ab + (bb - ab) * t);
    return RGB(clamp_int(r, 0, 255), clamp_int(g, 0, 255), clamp_int(bl, 0, 255));
}

// 使用指定颜色填充矩形
static void fill_rect_color(HDC hdc, int l, int t, int r, int b, COLORREF color) {
    RECT rc = {l, t, r, b};
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(hdc, &rc, brush);
    DeleteObject(brush);
}

// 绘制带填充色和轮廓线的椭圆
static void fill_ellipse(HDC hdc, int l, int t, int r, int b,
                         COLORREF fill, COLORREF outline, int outline_width) {
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, outline_width, outline);
    HGDIOBJ old_brush = SelectObject(hdc, brush);
    HGDIOBJ old_pen = SelectObject(hdc, pen);

    Ellipse(hdc, l, t, r, b);

    // 恢复旧 GDI 对象后，才能安全释放新创建的画笔和画刷
    SelectObject(hdc, old_pen);
    SelectObject(hdc, old_brush);
    DeleteObject(pen);
    DeleteObject(brush);
}

// 逐行填充不同颜色，绘制垂直渐变背景
static void draw_gradient(HDC hdc, int top, int bottom, int width,
                          COLORREF c1, COLORREF c2) {
    int height = bottom - top;
    if (height <= 0) return;

    for (int y = top; y < bottom; ++y) {
        double t = (double)(y - top) / (double)height;
        fill_rect_color(hdc, 0, y, width, y + 1, mix_color(c1, c2, t));
    }
}

// 初始化星星；固定随机种子可以保证每次运行时星空布局相同
static void init_stars(void) {
    srand(20260803);

    for (size_t i = 0; i < (sizeof(g_stars) / sizeof(g_stars[0])); ++i) {
        g_stars[i].x = rand() % 1000;
        g_stars[i].y = rand() % 480;
        g_stars[i].r = 1 + rand() % 2;
        g_stars[i].phase = rand() % 360;
    }
}

// 绘制星星；g_frame 和 phase 共同控制每颗星星的闪烁亮度
static void draw_stars(HDC hdc, int width, int sky_bottom) {
    for (size_t i = 0; i < (sizeof(g_stars) / sizeof(g_stars[0])); ++i) {
        int x = g_stars[i].x * width / 1000;
        int y = g_stars[i].y * sky_bottom / 480;

        // sin 的结果从 -1 到 1，转换后 s 的范围是 0 到 1
        double s = 0.5 + 0.5 * sin(
            g_frame * 0.06 + g_stars[i].phase * PI / 180.0
        );

        int bright = 150 + (int)(100 * s);
        COLORREF c = RGB(bright, bright, clamp_int(bright + 5, 0, 255));
        int r = g_stars[i].r;
        fill_ellipse(hdc, x - r, y - r, x + r + 1, y + r + 1, c, c, 1);
    }
}

// 绘制月亮、月光光晕和简单的月面阴影
static void draw_moon(HDC hdc, int cx, int cy, int radius) {
    for (int r = radius + 26; r >= radius; --r) {
        double t = (double)(r - radius) / 26.0;
        int alpha_like = (int)(35 * (1.0 - t));
        COLORREF glow = RGB(180 + alpha_like, 185 + alpha_like, 150 + alpha_like);
        fill_ellipse(hdc, cx - r, cy - r, cx + r, cy + r, glow, glow, 1);
    }

    fill_ellipse(hdc, cx - radius, cy - radius, cx + radius, cy + radius,
                 RGB(252, 246, 194), RGB(255, 250, 210), 1);
    fill_ellipse(hdc, cx - radius / 3, cy - radius / 4,
                 cx + radius / 8, cy + radius / 6,
                 RGB(237, 230, 177), RGB(237, 230, 177), 1);
    fill_ellipse(hdc, cx + radius / 5, cy + radius / 5,
                 cx + radius / 2, cy + radius / 2,
                 RGB(241, 234, 183), RGB(241, 234, 183), 1);
}

// 用多条左右摆动的短线绘制水中的月光倒影
static void draw_moon_reflection(HDC hdc, int cx, int water_top,
                                 int water_bottom, int moon_radius) {
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(202, 199, 135));
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    int h = water_bottom - water_top;

    for (int i = 0; i < 34; ++i) {
        int y = water_top + 15 + i * h / 38;
        double sway = sin(i * 0.9 + g_frame * 0.045) * 18.0;
        int half = (int)((moon_radius * 0.75) * (1.0 - i / 42.0));
        if (half < 8) half = 8;

        MoveToEx(hdc, cx - half + (int)sway, y, NULL);
        LineTo(hdc, cx + half + (int)sway, y);
    }

    SelectObject(hdc, old_pen);
    DeleteObject(pen);
}

// 将局部坐标点旋转后，再移动到屏幕中心点 cx、cy
static POINT rotate_point(double x, double y, double angle, int cx, int cy) {
    double ca = cos(angle);
    double sa = sin(angle);
    POINT p;

    p.x = cx + (LONG)(x * ca - y * sa);
    p.y = cy + (LONG)(x * sa + y * ca);
    return p;
}

// 使用两段贝塞尔曲线绘制一片花瓣
static void draw_petal(HDC hdc, int cx, int cy, double angle, int width, int height,
                       COLORREF fill, COLORREF outline) {
    // pts 保存花瓣轮廓的起点、控制点、顶点和终点
    POINT pts[7];
    pts[0] = rotate_point(-width * 0.42, 0, angle, cx, cy);
    pts[1] = rotate_point(-width * 0.50, -height * 0.36, angle, cx, cy);
    pts[2] = rotate_point(-width * 0.18, -height * 0.82, angle, cx, cy);
    pts[3] = rotate_point(0, -height, angle, cx, cy);
    pts[4] = rotate_point(width * 0.18, -height * 0.82, angle, cx, cy);
    pts[5] = rotate_point(width * 0.50, -height * 0.36, angle, cx, cy);
    pts[6] = rotate_point(width * 0.42, 0, angle, cx, cy);

    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, outline);
    HGDIOBJ old_brush = SelectObject(hdc, brush);
    HGDIOBJ old_pen = SelectObject(hdc, pen);

    BeginPath(hdc);
    MoveToEx(hdc, pts[0].x, pts[0].y, NULL);
    PolyBezierTo(hdc, &pts[1], 3);
    PolyBezierTo(hdc, &pts[4], 3);
    CloseFigure(hdc);
    EndPath(hdc);
    StrokeAndFillPath(hdc);

    SelectObject(hdc, old_pen);
    SelectObject(hdc, old_brush);
    DeleteObject(pen);
    DeleteObject(brush);
}

// 绘制荷花茎
static void draw_stem(HDC hdc, int x1, int y1, int x2, int y2, int width) {
    HPEN pen = CreatePen(PS_SOLID, width, RGB(43, 104, 68));
    HGDIOBJ old_pen = SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, old_pen);
    DeleteObject(pen);
}

// 组合多层、不同方向的花瓣，绘制一朵完整荷花
static void draw_lotus(HDC hdc, int cx, int cy, double scale) {
    int w1 = (int)(34 * scale);
    int h1 = (int)(55 * scale);
    int w2 = (int)(29 * scale);
    int h2 = (int)(46 * scale);

    COLORREF deep = RGB(200, 78, 122);
    COLORREF mid = RGB(235, 124, 158);
    COLORREF light = RGB(252, 182, 198);
    COLORREF edge = RGB(154, 57, 97);

    draw_petal(hdc, cx, cy, -1.05, w1, h1, deep, edge);
    draw_petal(hdc, cx, cy,  1.05, w1, h1, deep, edge);
    draw_petal(hdc, cx, cy, -0.55, w1, h1, mid, edge);
    draw_petal(hdc, cx, cy,  0.55, w1, h1, mid, edge);
    draw_petal(hdc, cx, cy,  0.00, w1, (int)(h1 * 1.08), light, edge);

    draw_petal(hdc, cx, cy + (int)(8 * scale), -0.82, w2, h2, light, edge);
    draw_petal(hdc, cx, cy + (int)(8 * scale),  0.82, w2, h2, light, edge);
    draw_petal(hdc, cx, cy + (int)(12 * scale), -0.30, w2, h2,
               RGB(255, 205, 214), edge);
    draw_petal(hdc, cx, cy + (int)(12 * scale),  0.30, w2, h2,
               RGB(255, 205, 214), edge);

    fill_ellipse(hdc, cx - (int)(8 * scale), cy - (int)(7 * scale),
                 cx + (int)(8 * scale), cy + (int)(7 * scale),
                 RGB(239, 192, 66), RGB(214, 157, 38), 1);
}

// 使用 36 个 POINT 点近似椭圆形荷叶，并绘制叶脉
static void draw_leaf(HDC hdc, int cx, int cy, int rx, int ry, double angle) {
    // points 保存荷叶轮廓上的 36 个点
    POINT points[36];

    for (int i = 0; i < 36; ++i) {
        double a = 2.0 * PI * i / 36.0;
        double x = rx * cos(a);
        double y = ry * sin(a);
        points[i] = rotate_point(x, y, angle, cx, cy);
    }

    HBRUSH brush = CreateSolidBrush(RGB(35, 101, 72));
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(21, 71, 52));
    HGDIOBJ old_brush = SelectObject(hdc, brush);
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    Polygon(hdc, points, 36);

    HPEN vein_pen = CreatePen(PS_SOLID, 1, RGB(79, 139, 90));
    SelectObject(hdc, vein_pen);

    POINT edge = rotate_point(rx * 0.78, 0, angle, cx, cy);
    MoveToEx(hdc, cx, cy, NULL);
    LineTo(hdc, edge.x, edge.y);

    for (int i = -2; i <= 2; ++i) {
        double a = angle + i * 0.30;
        POINT p = rotate_point(rx * 0.62, i * ry * 0.18, a, cx, cy);
        MoveToEx(hdc, cx, cy, NULL);
        LineTo(hdc, p.x, p.y);
    }

    SelectObject(hdc, old_pen);
    SelectObject(hdc, old_brush);
    DeleteObject(vein_pen);
    DeleteObject(pen);
    DeleteObject(brush);
}

// 绘制水面波纹，并使用 g_frame 控制波纹左右移动
static void draw_ripples(HDC hdc, int water_top, int width, int height) {
    int usable_width = (width > 0) ? width : 1;
    int usable_height = height - water_top - 45;
    if (usable_height < 1) usable_height = 1;

    for (int i = 0; i < 15; ++i) {
        int x = (i * 157 + 53) % usable_width;
        int y = water_top + 30 + ((i * 79) % usable_height);
        int span = 25 + (i % 5) * 18;
        int offset = (int)(8 * sin(g_frame * 0.035 + i));
        COLORREF c = (i % 2 == 0) ? RGB(68, 111, 120) : RGB(52, 91, 103);
        HPEN pen = CreatePen(PS_SOLID, 1, c);
        HGDIOBJ old_pen = SelectObject(hdc, pen);

        Arc(hdc, x - span + offset, y - 6, x + span + offset, y + 7,
            x - span + offset, y, x + span + offset, y);

        SelectObject(hdc, old_pen);
        DeleteObject(pen);
    }
}

// 绘制左右两侧随风摆动的芦苇
static void draw_reeds(HDC hdc, int water_top, int height, int width) {
    HPEN stem_pen = CreatePen(PS_SOLID, 2, RGB(36, 80, 56));
    HGDIOBJ old_pen = SelectObject(hdc, stem_pen);

    for (int i = 0; i < 18; ++i) {
        int x = (i < 9) ? i * 17 : width - (i - 8) * 17;
        int base = height;
        int top = water_top + 100 + (i * 37) % 160;
        int sway = (int)(5 * sin(g_frame * 0.025 + i * 0.8));
        MoveToEx(hdc, x, base, NULL);
        LineTo(hdc, x + sway, top);
    }

    SelectObject(hdc, old_pen);
    DeleteObject(stem_pen);
}

// 绘制会移动和闪烁的萤火虫
static void draw_fireflies(HDC hdc, int width, int water_top) {
    for (int i = 0; i < 10; ++i) {
        double t = g_frame * 0.02 + i * 1.7;
        int x = width / 10 + (i * width / 11) + (int)(18 * sin(t * 0.7));
        int y = water_top - 25 - (i % 4) * 24 + (int)(12 * cos(t));
        int glow = 2 + (int)(2 * (0.5 + 0.5 * sin(t * 1.8)));

        fill_ellipse(hdc, x - glow, y - glow, x + glow, y + glow,
                     RGB(244, 229, 112), RGB(244, 229, 112), 1);
    }
}

// draw_scene 负责按背景到前景的顺序绘制整幅“荷塘月色”场景
static void draw_scene(HDC hdc, int width, int height) {
    int water_top = (int)(height * 0.58);

    // 先绘制天空和水面背景
    draw_gradient(hdc, 0, water_top, width, RGB(9, 15, 48), RGB(24, 55, 78));
    draw_gradient(hdc, water_top, height, width, RGB(20, 58, 75), RGB(8, 33, 48));

    draw_stars(hdc, width, water_top);

    int moon_x = (int)(width * 0.75);
    int moon_y = (int)(height * 0.20);
    int moon_r = clamp_int(width / 18, 34, 68);
    draw_moon(hdc, moon_x, moon_y, moon_r);
    draw_moon_reflection(hdc, moon_x, water_top, height, moon_r);

    fill_rect_color(hdc, 0, water_top - 4, width, water_top + 2, RGB(27, 69, 82));
    draw_ripples(hdc, water_top, width, height);
    draw_reeds(hdc, water_top, height, width);
    draw_fireflies(hdc, width, water_top);

    // 三个不同的摆动值让三朵荷花不会完全同步移动
    int sway1 = (int)(3 * sin(g_frame * 0.025));
    int sway2 = (int)(4 * sin(g_frame * 0.021 + 1.3));
    int sway3 = (int)(3 * sin(g_frame * 0.018 + 2.1));

    draw_stem(hdc, (int)(width * 0.28), height,
              (int)(width * 0.31) + sway1, (int)(height * 0.60), 4);
    draw_stem(hdc, (int)(width * 0.55), height,
              (int)(width * 0.53) + sway2, (int)(height * 0.67), 4);
    draw_stem(hdc, (int)(width * 0.78), height,
              (int)(width * 0.76) + sway3, (int)(height * 0.64), 4);

    draw_leaf(hdc, (int)(width * 0.19), (int)(height * 0.73),
              width / 11, height / 28, -0.18);
    draw_leaf(hdc, (int)(width * 0.38), (int)(height * 0.82),
              width / 10, height / 26, 0.20);
    draw_leaf(hdc, (int)(width * 0.61), (int)(height * 0.75),
              width / 9, height / 25, -0.12);
    draw_leaf(hdc, (int)(width * 0.83), (int)(height * 0.84),
              width / 10, height / 28, 0.18);
    draw_leaf(hdc, (int)(width * 0.49), (int)(height * 0.92),
              width / 8, height / 24, 0.02);

    draw_lotus(hdc, (int)(width * 0.31) + sway1, (int)(height * 0.60), 1.0);
    draw_lotus(hdc, (int)(width * 0.53) + sway2, (int)(height * 0.67), 0.78);
    draw_lotus(hdc, (int)(width * 0.76) + sway3, (int)(height * 0.64), 0.90);

    fill_rect_color(hdc, 0, height - 18, width, height, RGB(7, 27, 29));
}

// window_proc 是窗口消息处理函数，负责定时器、键盘、绘制和退出消息
static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg,
                                    WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CREATE:
            // 每 33 毫秒触发一次 WM_TIMER，刷新率大约为 30 FPS
            SetTimer(hwnd, TIMER_ID, 33, NULL);

            // 窗口创建后开始循环播放舒缓背景音乐
            start_music();
            return 0;

        case WM_TIMER:
            if (wparam == TIMER_ID) {
                ++g_frame;

                // 标记整个窗口需要重绘，随后 Windows 会发送 WM_PAINT
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        case WM_KEYDOWN:
            // 按 Esc 键关闭窗口
            if (wparam == VK_ESCAPE) {
                DestroyWindow(hwnd);
            }
            return 0;

        case WM_ERASEBKGND:
            // 场景会完整覆盖背景，返回 1 可以减少动画闪烁
            return 1;

        case WM_PAINT: {
            // PAINTSTRUCT 保存本次窗口绘制的信息
            PAINTSTRUCT ps;
            HDC window_dc = BeginPaint(hwnd, &ps);

            // RECT 保存窗口客户区的坐标范围
            RECT rc;
            GetClientRect(hwnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;

            if (width <= 0 || height <= 0) {
                EndPaint(hwnd, &ps);
                return 0;
            }

            // 双缓冲：先在内存位图中完成整幅画面，再一次复制到窗口
            HDC mem_dc = CreateCompatibleDC(window_dc);
            HBITMAP mem_bitmap = CreateCompatibleBitmap(window_dc, width, height);
            HGDIOBJ old_bitmap = SelectObject(mem_dc, mem_bitmap);

            draw_scene(mem_dc, width, height);
            BitBlt(window_dc, 0, 0, width, height,
                   mem_dc, 0, 0, SRCCOPY);

            SelectObject(mem_dc, old_bitmap);
            DeleteObject(mem_bitmap);
            DeleteDC(mem_dc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            // 窗口销毁时停止动画和音乐、释放 WAV 内存，再结束消息循环
            KillTimer(hwnd, TIMER_ID);
            cleanup_music();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

// WinMain 是 Windows GUI 程序的入口函数
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance,
                   LPSTR cmd_line, int show_cmd) {
    (void)prev_instance;
    (void)cmd_line;

    init_stars();

    // 使用宽字符字符串和 W 版本 API，确保中文窗口标题正常显示
    const wchar_t *class_name = L"LotusPondMoonlightWindow";
    const wchar_t *window_title = L"荷塘月色";

    // WNDCLASSW 保存窗口类的样式、窗口处理函数、图标和光标等信息
    WNDCLASSW wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = class_name;

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"窗口类注册失败。", L"错误", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowExW(
        0,
        class_name,
        window_title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1100, 720,
        NULL, NULL, instance, NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"窗口创建失败。", L"错误", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, show_cmd);
    UpdateWindow(hwnd);

    // MSG 保存消息循环中取得的 Windows 消息
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}