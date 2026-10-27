"""motor de plotagem vetorial svg e png sem dependências externas"""

import os
import struct
import zlib
import numpy as np
import pandas as pd

# mapeamento de caracteres acentuados para fonte bitmap básica
_ACCENT_MAP = {
    "á": "a", "à": "a", "ã": "a", "â": "a",
    "é": "e", "ê": "e",
    "í": "i",
    "ó": "o", "ô": "o", "õ": "o",
    "ú": "u",
    "ç": "c",
}

# tabela compacta de fonte 5x7 para rasterização de texto
_FONT_5X7 = {
    " ": [0x00, 0x00, 0x00, 0x00, 0x00],
    "!": [0x00, 0x00, 0x5F, 0x00, 0x00],
    "(": [0x00, 0x1C, 0x22, 0x41, 0x00],
    ")": [0x00, 0x41, 0x22, 0x1C, 0x00],
    "*": [0x14, 0x08, 0x3E, 0x08, 0x14],
    "+": [0x08, 0x08, 0x3E, 0x08, 0x08],
    ",": [0x00, 0x50, 0x30, 0x00, 0x00],
    "-": [0x08, 0x08, 0x08, 0x08, 0x08],
    ".": [0x00, 0x60, 0x60, 0x00, 0x00],
    "/": [0x20, 0x10, 0x08, 0x04, 0x02],
    "0": [0x3E, 0x51, 0x49, 0x45, 0x3E],
    "1": [0x00, 0x42, 0x7F, 0x40, 0x00],
    "2": [0x42, 0x61, 0x51, 0x49, 0x46],
    "3": [0x21, 0x41, 0x45, 0x4B, 0x31],
    "4": [0x18, 0x14, 0x12, 0x7F, 0x10],
    "5": [0x27, 0x45, 0x45, 0x45, 0x39],
    "6": [0x3C, 0x4A, 0x49, 0x49, 0x30],
    "7": [0x01, 0x71, 0x09, 0x05, 0x03],
    "8": [0x36, 0x49, 0x49, 0x49, 0x36],
    "9": [0x06, 0x49, 0x49, 0x29, 0x1E],
    ":": [0x00, 0x36, 0x36, 0x00, 0x00],
    "<": [0x08, 0x14, 0x22, 0x41, 0x00],
    "=": [0x14, 0x14, 0x14, 0x14, 0x14],
    ">": [0x00, 0x41, 0x22, 0x14, 0x08],
    "?": [0x02, 0x01, 0x51, 0x09, 0x06],
    "_": [0x40, 0x40, 0x40, 0x40, 0x40],
    "%": [0x23, 0x13, 0x08, 0x64, 0x62],
    "a": [0x20, 0x54, 0x54, 0x54, 0x78],
    "b": [0x7F, 0x48, 0x44, 0x44, 0x38],
    "c": [0x38, 0x44, 0x44, 0x44, 0x20],
    "d": [0x38, 0x44, 0x44, 0x48, 0x7F],
    "e": [0x38, 0x54, 0x54, 0x54, 0x18],
    "f": [0x08, 0x7E, 0x09, 0x01, 0x02],
    "g": [0x08, 0x14, 0x54, 0x54, 0x3C],
    "h": [0x7F, 0x08, 0x04, 0x04, 0x78],
    "i": [0x00, 0x44, 0x7D, 0x40, 0x00],
    "j": [0x20, 0x40, 0x44, 0x3D, 0x00],
    "k": [0x7F, 0x10, 0x28, 0x44, 0x00],
    "l": [0x00, 0x41, 0x7F, 0x40, 0x00],
    "m": [0x7C, 0x04, 0x18, 0x04, 0x78],
    "n": [0x7C, 0x08, 0x04, 0x04, 0x78],
    "o": [0x38, 0x44, 0x44, 0x44, 0x38],
    "p": [0x7C, 0x14, 0x14, 0x14, 0x08],
    "q": [0x08, 0x14, 0x14, 0x18, 0x7C],
    "r": [0x7C, 0x08, 0x04, 0x04, 0x08],
    "s": [0x48, 0x54, 0x54, 0x54, 0x20],
    "t": [0x04, 0x3F, 0x44, 0x40, 0x20],
    "u": [0x3C, 0x40, 0x40, 0x20, 0x7C],
    "v": [0x1C, 0x20, 0x40, 0x20, 0x1C],
    "w": [0x3C, 0x40, 0x30, 0x40, 0x3C],
    "x": [0x44, 0x28, 0x10, 0x28, 0x44],
    "y": [0x0C, 0x50, 0x50, 0x50, 0x3C],
    "z": [0x44, 0x64, 0x54, 0x4C, 0x44],
}


def _encode_png(img: np.ndarray) -> bytes:
    """codifica array numpy rgb em bytes válidos no padrão png"""
    h, w, _ = img.shape
    raw = bytearray()
    for row in range(h):
        raw.append(0)
        raw.extend(img[row].tobytes())
    idat_data = zlib.compress(bytes(raw), level=6)

    chunks = [b"\x89PNG\r\n\x1a\n"]

    # ihdr chunk
    ihdr = struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)
    chunks.append(struct.pack(">I", len(ihdr)) + b"IHDR" + ihdr + struct.pack(">I", zlib.crc32(b"IHDR" + ihdr)))

    # idat chunk
    chunks.append(struct.pack(">I", len(idat_data)) + b"IDAT" + idat_data + struct.pack(">I", zlib.crc32(b"IDAT" + idat_data)))

    # iend chunk
    chunks.append(struct.pack(">I", 0) + b"IEND" + struct.pack(">I", zlib.crc32(b"IEND")))

    return b"".join(chunks)


def _hex_to_rgb(hex_code: str) -> tuple[int, int, int]:
    """converte string de cor hexadecimal em tupla rgb"""
    clean = hex_code.lstrip("#")
    if len(clean) == 3:
        clean = "".join(c * 2 for c in clean)
    return (int(clean[0:2], 16), int(clean[2:4], 16), int(clean[4:6], 16))


class Canvas:
    """canvas duplo de svg e mapa de bits para plotagem de gráficos"""

    def __init__(self, width: int = 800, height: int = 500, bg: str = "#ffffff"):
        self.width = width
        self.height = height
        self.bg_hex = bg
        self.bg_rgb = _hex_to_rgb(bg)
        self.img = np.full((height, width, 3), self.bg_rgb, dtype=np.uint8)
        self.svg_elements: list[str] = []

    def draw_rect(
        self,
        x: float,
        y: float,
        w: float,
        h: float,
        fill: str | None = None,
        stroke: str | None = None,
        stroke_width: int = 1,
    ) -> None:
        """draws filled or outlined rectangle on svg and raster canvas"""
        x_int = max(0, min(self.width, int(round(x))))
        y_int = max(0, min(self.height, int(round(y))))
        w_int = max(0, min(self.width - x_int, int(round(w))))
        h_int = max(0, min(self.height - y_int, int(round(h))))

        if fill is not None:
            rgb_fill = _hex_to_rgb(fill)
            self.img[y_int : y_int + h_int, x_int : x_int + w_int] = rgb_fill

        if stroke is not None:
            rgb_stroke = _hex_to_rgb(stroke)
            self.draw_line(x, y, x + w, y, stroke, width=stroke_width)
            self.draw_line(x, y + h, x + w, y + h, stroke, width=stroke_width)
            self.draw_line(x, y, x, y + h, stroke, width=stroke_width)
            self.draw_line(x + w, y, x + w, y + h, stroke, width=stroke_width)

        fill_attr = fill if fill is not None else "none"
        stroke_attr = stroke if stroke is not None else "none"
        self.svg_elements.append(
            f'<rect x="{x:.1f}" y="{y:.1f}" width="{w:.1f}" height="{h:.1f}" '
            f'fill="{fill_attr}" stroke="{stroke_attr}" stroke-width="{stroke_width}"/>'
        )

    def draw_line(
        self,
        x0: float,
        y0: float,
        x1: float,
        y1: float,
        color: str,
        width: int = 1,
        dash: str | None = None,
    ) -> None:
        """draws straight line segment using bresenham algorithm on raster"""
        rgb = _hex_to_rgb(color)
        xi0, yi0 = int(round(x0)), int(round(y0))
        xi1, yi1 = int(round(x1)), int(round(y1))

        # rasterize line onto numpy array
        dx = abs(xi1 - xi0)
        dy = abs(yi1 - yi0)
        sx = 1 if xi0 < xi1 else -1
        sy = 1 if yi0 < yi1 else -1
        err = dx - dy
        rad = max(0, int(width) // 2)

        curr_x, curr_y = xi0, yi0
        step_count = 0
        while True:
            # handle dashed pattern if requested
            draw_pixel = True
            if dash is not None:
                step_count += 1
                if (step_count // 6) % 2 == 1:
                    draw_pixel = False

            if draw_pixel and 0 <= curr_x < self.width and 0 <= curr_y < self.height:
                if rad == 0:
                    self.img[curr_y, curr_x] = rgb
                else:
                    y_min = max(0, curr_y - rad)
                    y_max = min(self.height, curr_y + rad + 1)
                    x_min = max(0, curr_x - rad)
                    x_max = min(self.width, curr_x + rad + 1)
                    self.img[y_min:y_max, x_min:x_max] = rgb

            if curr_x == xi1 and curr_y == yi1:
                break
            e2 = 2 * err
            if e2 > -dy:
                err -= dy
                curr_x += sx
            if e2 < dx:
                err += dx
                curr_y += sy

        dash_attr = f' stroke-dasharray="{dash}"' if dash is not None else ""
        self.svg_elements.append(
            f'<line x1="{x0:.1f}" y1="{y0:.1f}" x2="{x1:.1f}" y2="{y1:.1f}" '
            f'stroke="{color}" stroke-width="{width}"{dash_attr}/>'
        )

    def draw_polyline(self, points: list[tuple[float, float]], color: str, width: int = 2) -> None:
        """draws connected sequence of line segments"""
        if len(points) < 2:
            return

        for i in range(len(points) - 1):
            p0, p1 = points[i], points[i + 1]
            self.draw_line(p0[0], p0[1], p1[0], p1[1], color, width=width)

        pts_str = " ".join(f"{x:.1f},{y:.1f}" for x, y in points)
        self.svg_elements.append(
            f'<polyline points="{pts_str}" fill="none" stroke="{color}" '
            f'stroke-width="{width}" stroke-linecap="round" stroke-linejoin="round"/>'
        )

    def draw_shaded_band(
        self,
        x_coords: list[float],
        y_upper: list[float],
        y_lower: list[float],
        color: str,
        alpha: float = 0.25,
    ) -> None:
        """draws filled polygon ribbon between upper and lower curves"""
        if len(x_coords) < 2:
            return

        # rasterize alpha blended vertical columns
        rgb = np.array(_hex_to_rgb(color), dtype=np.float32)
        x_arr = np.array(x_coords, dtype=np.float64)
        yu_arr = np.array(y_upper, dtype=np.float64)
        yl_arr = np.array(y_lower, dtype=np.float64)

        x_min_int = max(0, int(np.floor(np.min(x_arr))))
        x_max_int = min(self.width, int(np.ceil(np.max(x_arr))))

        if x_max_int > x_min_int:
            x_dense = np.arange(x_min_int, x_max_int)
            yu_interp = np.interp(x_dense, x_arr, yu_arr)
            yl_interp = np.interp(x_dense, x_arr, yl_arr)

            for x_col, y1, y2 in zip(x_dense, yu_interp, yl_interp):
                ya = max(0, min(self.height - 1, int(round(min(y1, y2)))))
                yb = max(0, min(self.height - 1, int(round(max(y1, y2)))))
                if yb > ya:
                    curr = self.img[ya:yb, x_col].astype(np.float32)
                    blended = (1.0 - alpha) * curr + alpha * rgb
                    self.img[ya:yb, x_col] = np.clip(blended, 0, 255).astype(np.uint8)

        # construct svg polygon element
        poly_pts = []
        for x, y in zip(x_coords, y_upper):
            poly_pts.append(f"{x:.1f},{y:.1f}")
        for x, y in reversed(list(zip(x_coords, y_lower))):
            poly_pts.append(f"{x:.1f},{y:.1f}")
        pts_str = " ".join(poly_pts)
        self.svg_elements.append(
            f'<polygon points="{pts_str}" fill="{color}" fill-opacity="{alpha:.2f}" stroke="none"/>'
        )

    def draw_circle(self, cx: float, cy: float, r: float, color: str, alpha: float = 0.8) -> None:
        """draws filled circle for scatter points"""
        rgb = np.array(_hex_to_rgb(color), dtype=np.float32)
        xi, yi = int(round(cx)), int(round(cy))
        ri = int(round(r))

        y_min = max(0, yi - ri)
        y_max = min(self.height, yi + ri + 1)
        x_min = max(0, xi - ri)
        x_max = min(self.width, xi + ri + 1)

        for y in range(y_min, y_max):
            for x in range(x_min, x_max):
                if (x - xi) ** 2 + (y - yi) ** 2 <= ri**2:
                    if alpha >= 0.99:
                        self.img[y, x] = rgb.astype(np.uint8)
                    else:
                        curr = self.img[y, x].astype(np.float32)
                        blended = (1.0 - alpha) * curr + alpha * rgb
                        self.img[y, x] = np.clip(blended, 0, 255).astype(np.uint8)

        self.svg_elements.append(
            f'<circle cx="{cx:.1f}" cy="{cy:.1f}" r="{r:.1f}" fill="{color}" fill-opacity="{alpha:.2f}"/>'
        )

    def draw_text(
        self,
        text: str,
        x: float,
        y: float,
        size: int = 12,
        color: str = "#222222",
        anchor: str = "start",
        bold: bool = False,
        rotate: int = 0,
    ) -> None:
        """desenha texto no svg e no mapa de bits raster"""
        clean_text = str(text)
        rgb = _hex_to_rgb(color)

        # rasteriza texto usando fonte de mapa de bits 5x7
        scale = max(1, size // 11)
        char_w = 6 * scale
        text_w = len(clean_text) * char_w

        x0 = int(round(x))
        if anchor == "middle":
            x0 -= text_w // 2
        elif anchor == "end":
            x0 -= text_w
        y0 = int(round(y)) - 7 * scale // 2

        if rotate == 0:
            for ch in clean_text.lower():
                ch_mapped = _ACCENT_MAP.get(ch, ch)
                bmp = _FONT_5X7.get(ch_mapped, _FONT_5X7.get(" ", [0] * 5))
                for col_idx, col_byte in enumerate(bmp):
                    for row_idx in range(7):
                        if (col_byte >> row_idx) & 1:
                            px = x0 + col_idx * scale
                            py = y0 + row_idx * scale
                            if 0 <= px < self.width and 0 <= py < self.height:
                                if scale == 1:
                                    self.img[py, px] = rgb
                                else:
                                    py_max = min(self.height, py + scale)
                                    px_max = min(self.width, px + scale)
                                    self.img[py:py_max, px:px_max] = rgb
                x0 += char_w
        elif rotate == -90:
            # desenha verticalmente de baixo para cima
            for ch in clean_text.lower():
                ch_mapped = _ACCENT_MAP.get(ch, ch)
                bmp = _FONT_5X7.get(ch_mapped, _FONT_5X7.get(" ", [0] * 5))
                for col_idx, col_byte in enumerate(bmp):
                    for row_idx in range(7):
                        if (col_byte >> row_idx) & 1:
                            px = int(round(x)) + row_idx * scale
                            py = int(round(y)) - col_idx * scale
                            if 0 <= px < self.width and 0 <= py < self.height:
                                self.img[py, px] = rgb
                y = y - char_w

        rot_attr = f' transform="rotate({rotate} {x:.1f} {y:.1f})"' if rotate != 0 else ""
        weight_attr = ' font-weight="bold"' if bold else ""
        self.svg_elements.append(
            f'<text x="{x:.1f}" y="{y:.1f}" font-family="sans-serif" font-size="{size}" '
            f'text-anchor="{anchor}" fill="{color}"{weight_attr}{rot_attr}>{clean_text}</text>'
        )

    def to_svg(self) -> str:
        """serializes entire canvas to well-formed xml svg string"""
        header = (
            f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {self.width} {self.height}" '
            f'width="{self.width}" height="{self.height}" style="background-color: {self.bg_hex}; font-family: sans-serif;">\n'
        )
        bg_rect = f'  <rect width="100%" height="100%" fill="{self.bg_hex}"/>\n'
        body = "\n".join("  " + el for el in self.svg_elements)
        footer = "\n</svg>\n"
        return header + bg_rect + body + footer

    def to_png_bytes(self) -> bytes:
        """serializes raster bitmap into valid standard png bytes"""
        return _encode_png(self.img)

    def save(self, output_path: str) -> None:
        """saves both svg and png files ensuring directory exists"""
        out_dir = os.path.dirname(os.path.abspath(output_path))
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)

        base, ext = os.path.splitext(output_path)
        svg_content = self.to_svg()
        png_bytes = self.to_png_bytes()

        if ext.lower() == ".svg":
            with open(output_path, "w", encoding="utf-8") as f:
                f.write(svg_content)
            # companion png
            with open(base + ".png", "wb") as f:
                f.write(png_bytes)
        elif ext.lower() == ".png":
            with open(output_path, "wb") as f:
                f.write(png_bytes)
            # companion svg
            with open(base + ".svg", "w", encoding="utf-8") as f:
                f.write(svg_content)
        else:
            # write both default formats
            with open(output_path + ".svg", "w", encoding="utf-8") as f:
                f.write(svg_content)
            with open(output_path + ".png", "wb") as f:
                f.write(png_bytes)


def _load_data_series(source: str | pd.DataFrame | list[str]) -> pd.DataFrame:
    """reads csv path, dataframe, or list of csv paths into unified dataframe"""
    if isinstance(source, pd.DataFrame):
        return source.copy()
    if isinstance(source, str):
        return pd.read_csv(source)
    dfs = [pd.read_csv(f) for f in source]
    return pd.concat(dfs, ignore_index=True)


def _calculate_nice_ticks(v_min: float, v_max: float, target_count: int = 6) -> list[float]:
    """generates human-friendly round tick intervals"""
    if v_max <= v_min:
        v_max = v_min + 1.0
    raw_range = v_max - v_min
    raw_step = raw_range / max(1, target_count)
    exp = np.floor(np.log10(max(1e-12, raw_step)))
    fraction = raw_step / (10**exp)

    if fraction < 1.5:
        nice_step = 1.0 * (10**exp)
    elif fraction < 3.0:
        nice_step = 2.0 * (10**exp)
    elif fraction < 7.0:
        nice_step = 5.0 * (10**exp)
    else:
        nice_step = 10.0 * (10**exp)

    start = np.floor(v_min / nice_step) * nice_step
    end = np.ceil(v_max / nice_step) * nice_step

    ticks = []
    val = start
    while val <= end + 0.5 * nice_step:
        if val >= v_min - 1e-9 and val <= v_max + 1e-9:
            ticks.append(val)
        val += nice_step

    if not ticks:
        ticks = [v_min, v_max]
    return ticks


def _setup_plot_axes(
    canvas: Canvas,
    x_min: float,
    x_max: float,
    y_min: float,
    y_max: float,
    x_label: str,
    y_label: str,
    title: str,
) -> tuple[float, float, float, float]:
    """desenha borda do gráfico, marcas, linhas de grade, rótulos e título"""
    x0, y0 = 75.0, 50.0
    w, h = 680.0, 380.0
    x1, y1 = x0 + w, y0 + h

    # plot background and boundary box
    canvas.draw_rect(x0, y0, w, h, fill="#fafafa", stroke="#cccccc", stroke_width=1)

    # title
    canvas.draw_text(title, canvas.width / 2.0, 30.0, size=15, color="#111111", anchor="middle", bold=True)

    # x-ticks and vertical grid lines
    x_ticks = _calculate_nice_ticks(x_min, x_max, target_count=7)
    for xt in x_ticks:
        if x_max > x_min:
            px = x0 + ((xt - x_min) / (x_max - x_min)) * w
            canvas.draw_line(px, y0, px, y1, color="#eeeeee", width=1)
            canvas.draw_line(px, y1, px, y1 + 5, color="#555555", width=1)
            label = f"{int(xt)}" if abs(xt - round(xt)) < 1e-6 else f"{xt:.2g}"
            canvas.draw_text(label, px, y1 + 18, size=10, color="#444444", anchor="middle")

    # y-ticks and horizontal grid lines
    y_ticks = _calculate_nice_ticks(y_min, y_max, target_count=6)
    for yt in y_ticks:
        if y_max > y_min:
            py = y1 - ((yt - y_min) / (y_max - y_min)) * h
            canvas.draw_line(x0, py, x1, py, color="#eeeeee", width=1)
            canvas.draw_line(x0 - 5, py, x0, py, color="#555555", width=1)
            label = f"{int(yt)}" if abs(yt - round(yt)) < 1e-6 else f"{yt:.2g}"
            canvas.draw_text(label, x0 - 8, py, size=10, color="#444444", anchor="end")

    # axis labels
    canvas.draw_text(x_label, x0 + w / 2.0, y1 + 42, size=12, color="#222222", anchor="middle")
    canvas.draw_text(y_label, 20.0, y0 + h / 2.0, size=12, color="#222222", anchor="middle", rotate=-90)

    return x0, y0, w, h


def _draw_legend(
    canvas: Canvas,
    entries: list[tuple[str, str, str]],
    x: float = 560.0,
    y: float = 62.0,
) -> None:
    """desenha caixa de legenda com linhas coloridas, marcadores e rótulos"""
    if not entries:
        return
    item_h = 20.0
    box_w = 185.0
    box_h = len(entries) * item_h + 10.0

    canvas.draw_rect(x, y, box_w, box_h, fill="#ffffff", stroke="#dddddd", stroke_width=1)

    curr_y = y + 14.0
    for label, color, style in entries:
        line_y = curr_y - 2.0
        if style == "line":
            canvas.draw_line(x + 10, line_y, x + 35, line_y, color=color, width=2)
        elif style == "dash":
            canvas.draw_line(x + 10, line_y, x + 35, line_y, color=color, width=2, dash="4 4")
        elif style == "circle":
            canvas.draw_circle(x + 22, line_y, 4, color=color, alpha=0.9)
        canvas.draw_text(label, x + 42, curr_y, size=10, color="#333333", anchor="start")
        curr_y += item_h


def plot_convergence(
    csv_path: str | pd.DataFrame | list[str],
    output_path: str,
    title: str = "convergência",
) -> None:
    """plota melhor rmse de treino, rmse médio de treino e melhor rmse de teste ao longo das gerações"""
    df = _load_data_series(csv_path)

    # group by generation to aggregate multiple runs
    grouped = df.groupby("generation")
    gens = np.array(sorted(grouped.groups.keys()), dtype=np.float64)

    curves = []
    # best train
    if "best_train_rmse" in df.columns:
        mean_v = grouped["best_train_rmse"].mean().values
        std_v = grouped["best_train_rmse"].std().fillna(0).values
        curves.append(("melhor rmse de treino", "#1f77b4", mean_v, std_v))

    # mean train
    if "mean_train_rmse" in df.columns:
        mean_v = grouped["mean_train_rmse"].mean().values
        std_v = grouped["mean_train_rmse"].std().fillna(0).values
        curves.append(("rmse médio de treino", "#ff7f0e", mean_v, std_v))

    # best test
    if "best_test_rmse" in df.columns:
        mean_v = grouped["best_test_rmse"].mean().values
        std_v = grouped["best_test_rmse"].std().fillna(0).values
        curves.append(("melhor rmse de teste", "#2ca02c", mean_v, std_v))

    # determine bounds
    x_min, x_max = float(np.min(gens)), float(np.max(gens))
    y_min = 0.0
    all_high = [np.max(c[2] + c[3]) for c in curves]
    y_max = float(max(1.0, max(all_high) * 1.05))

    canvas = Canvas(800, 500)
    x0, y0, w, h = _setup_plot_axes(canvas, x_min, x_max, y_min, y_max, "geração", "rmse", title)

    legend_items = []
    for label, color, mean_vals, std_vals in curves:
        pts = []
        for g, val in zip(gens, mean_vals):
            px = x0 + ((g - x_min) / max(1.0, x_max - x_min)) * w
            py = (y0 + h) - ((val - y_min) / max(1e-9, y_max - y_min)) * h
            pts.append((px, py))

        # draw ribbon if std > 0
        if np.max(std_vals) > 1e-6:
            x_pts = []
            yu_pts = []
            yl_pts = []
            for g, m, s in zip(gens, mean_vals, std_vals):
                px = x0 + ((g - x_min) / max(1.0, x_max - x_min)) * w
                py_u = (y0 + h) - ((m + s - y_min) / max(1e-9, y_max - y_min)) * h
                py_l = (y0 + h) - ((max(0.0, m - s) - y_min) / max(1e-9, y_max - y_min)) * h
                x_pts.append(px)
                yu_pts.append(py_u)
                yl_pts.append(py_l)
            canvas.draw_shaded_band(x_pts, yu_pts, yl_pts, color, alpha=0.18)

        canvas.draw_polyline(pts, color, width=2)
        legend_items.append((label, color, "line"))

    _draw_legend(canvas, legend_items)
    canvas.save(output_path)


def plot_bloat(
    csv_path: str | pd.DataFrame | list[str],
    output_path: str,
    title: str = "tamanho e profundidade da árvore",
) -> None:
    """plota contagem média de nós e profundidade média ao longo das gerações"""
    df = _load_data_series(csv_path)

    grouped = df.groupby("generation")
    gens = np.array(sorted(grouped.groups.keys()), dtype=np.float64)

    curves = []
    if "avg_nodes" in df.columns:
        mean_v = grouped["avg_nodes"].mean().values
        std_v = grouped["avg_nodes"].std().fillna(0).values
        curves.append(("número médio de nós", "#1f77b4", mean_v, std_v))

    if "avg_depth" in df.columns:
        mean_v = grouped["avg_depth"].mean().values
        std_v = grouped["avg_depth"].std().fillna(0).values
        curves.append(("profundidade média", "#9467bd", mean_v, std_v))

    x_min, x_max = float(np.min(gens)), float(np.max(gens))
    y_min = 0.0
    all_high = [np.max(c[2] + c[3]) for c in curves]
    y_max = float(max(1.0, max(all_high) * 1.1))

    canvas = Canvas(800, 500)
    x0, y0, w, h = _setup_plot_axes(canvas, x_min, x_max, y_min, y_max, "geração", "tamanho / profundidade", title)

    legend_items = []
    for label, color, mean_vals, std_vals in curves:
        pts = []
        for g, val in zip(gens, mean_vals):
            px = x0 + ((g - x_min) / max(1.0, x_max - x_min)) * w
            py = (y0 + h) - ((val - y_min) / max(1e-9, y_max - y_min)) * h
            pts.append((px, py))

        if np.max(std_vals) > 1e-6:
            x_pts, yu_pts, yl_pts = [], [], []
            for g, m, s in zip(gens, mean_vals, std_vals):
                px = x0 + ((g - x_min) / max(1.0, x_max - x_min)) * w
                py_u = (y0 + h) - ((m + s - y_min) / max(1e-9, y_max - y_min)) * h
                py_l = (y0 + h) - ((max(0.0, m - s) - y_min) / max(1e-9, y_max - y_min)) * h
                x_pts.append(px)
                yu_pts.append(py_u)
                yl_pts.append(py_l)
            canvas.draw_shaded_band(x_pts, yu_pts, yl_pts, color, alpha=0.18)

        canvas.draw_polyline(pts, color, width=2)
        legend_items.append((label, color, "line"))

    _draw_legend(canvas, legend_items)
    canvas.save(output_path)


def plot_operator_success(
    csv_path: str | pd.DataFrame | list[str],
    output_path: str,
    title: str = "sucesso do operador de cruzamento",
) -> None:
    """plota contagens ou taxas de cruzamento melhores vs piores ao longo das gerações"""
    df = _load_data_series(csv_path)

    grouped = df.groupby("generation")
    gens = np.array(sorted(grouped.groups.keys()), dtype=np.float64)

    curves = []
    if "crossover_better" in df.columns:
        mean_v = grouped["crossover_better"].mean().values
        std_v = grouped["crossover_better"].std().fillna(0).values
        curves.append(("cruzamento melhor", "#2ca02c", mean_v, std_v))

    if "crossover_worse" in df.columns:
        mean_v = grouped["crossover_worse"].mean().values
        std_v = grouped["crossover_worse"].std().fillna(0).values
        curves.append(("cruzamento pior", "#d62728", mean_v, std_v))

    if "crossover_equal" in df.columns:
        mean_v = grouped["crossover_equal"].mean().values
        std_v = grouped["crossover_equal"].std().fillna(0).values
        curves.append(("cruzamento igual", "#7f7f7f", mean_v, std_v))

    x_min, x_max = float(np.min(gens)), float(np.max(gens))
    y_min = 0.0
    all_high = [np.max(c[2] + c[3]) for c in curves]
    y_max = float(max(1.0, max(all_high) * 1.1))

    canvas = Canvas(800, 500)
    x0, y0, w, h = _setup_plot_axes(canvas, x_min, x_max, y_min, y_max, "geração", "contagem de descendentes", title)

    legend_items = []
    for label, color, mean_vals, std_vals in curves:
        pts = []
        for g, val in zip(gens, mean_vals):
            px = x0 + ((g - x_min) / max(1.0, x_max - x_min)) * w
            py = (y0 + h) - ((val - y_min) / max(1e-9, y_max - y_min)) * h
            pts.append((px, py))

        if np.max(std_vals) > 1e-6:
            x_pts, yu_pts, yl_pts = [], [], []
            for g, m, s in zip(gens, mean_vals, std_vals):
                px = x0 + ((g - x_min) / max(1.0, x_max - x_min)) * w
                py_u = (y0 + h) - ((m + s - y_min) / max(1e-9, y_max - y_min)) * h
                py_l = (y0 + h) - ((max(0.0, m - s) - y_min) / max(1e-9, y_max - y_min)) * h
                x_pts.append(px)
                yu_pts.append(py_u)
                yl_pts.append(py_l)
            canvas.draw_shaded_band(x_pts, yu_pts, yl_pts, color, alpha=0.18)

        canvas.draw_polyline(pts, color, width=2)
        legend_items.append((label, color, "line"))

    _draw_legend(canvas, legend_items)
    canvas.save(output_path)


def plot_selection_diversity(
    csv_paths: dict[str, str | pd.DataFrame | list[str]],
    output_path: str,
    title: str = "diversidade de seleção",
) -> None:
    """plota contagens de árvores canônicas únicas entre métodos de seleção"""
    palette = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    curves = []

    global_x_min = 1e9
    global_x_max = -1e9
    global_y_max = 1.0

    for idx, (label, src) in enumerate(csv_paths.items()):
        df = _load_data_series(src)
        grouped = df.groupby("generation")
        gens = np.array(sorted(grouped.groups.keys()), dtype=np.float64)
        mean_v = grouped["unique_canonical_trees"].mean().values
        std_v = grouped["unique_canonical_trees"].std().fillna(0).values

        global_x_min = min(global_x_min, float(np.min(gens)))
        global_x_max = max(global_x_max, float(np.max(gens)))
        global_y_max = max(global_y_max, float(np.max(mean_v + std_v)))

        color = palette[idx % len(palette)]
        curves.append((label, color, gens, mean_v, std_v))

    canvas = Canvas(800, 500)
    x0, y0, w, h = _setup_plot_axes(
        canvas,
        global_x_min,
        global_x_max,
        0.0,
        global_y_max * 1.1,
        "geração",
        "árvores canônicas únicas",
        title,
    )

    legend_items = []
    for label, color, gens, mean_vals, std_vals in curves:
        pts = []
        for g, val in zip(gens, mean_vals):
            px = x0 + ((g - global_x_min) / max(1.0, global_x_max - global_x_min)) * w
            py = (y0 + h) - (val / max(1e-9, global_y_max * 1.1)) * h
            pts.append((px, py))

        if np.max(std_vals) > 1e-6:
            x_pts, yu_pts, yl_pts = [], [], []
            for g, m, s in zip(gens, mean_vals, std_vals):
                px = x0 + ((g - global_x_min) / max(1.0, global_x_max - global_x_min)) * w
                py_u = (y0 + h) - ((m + s) / max(1e-9, global_y_max * 1.1)) * h
                py_l = (y0 + h) - (max(0.0, m - s) / max(1e-9, global_y_max * 1.1)) * h
                x_pts.append(px)
                yu_pts.append(py_u)
                yl_pts.append(py_l)
            canvas.draw_shaded_band(x_pts, yu_pts, yl_pts, color, alpha=0.18)

        canvas.draw_polyline(pts, color, width=2)
        legend_items.append((label, color, "line"))

    _draw_legend(canvas, legend_items)
    canvas.save(output_path)


def plot_parity(
    y_true: np.ndarray,
    y_pred: np.ndarray,
    output_path: str,
    title: str = "predições vs real",
) -> None:
    """gráfico de dispersão comparando o alvo real contra valores preditos com a linha y=x"""
    yt = np.asarray(y_true, dtype=np.float64).ravel()
    yp = np.asarray(y_pred, dtype=np.float64).ravel()

    # compute common bounds for true square aspect ratio
    v_min = float(min(np.min(yt), np.min(yp)))
    v_max = float(max(np.max(yt), np.max(yp)))
    if v_max <= v_min:
        v_max = v_min + 1.0
    margin = 0.05 * (v_max - v_min)
    bound_min = v_min - margin
    bound_max = v_max + margin

    canvas = Canvas(800, 500)
    x0, y0, w, h = _setup_plot_axes(
        canvas,
        bound_min,
        bound_max,
        bound_min,
        bound_max,
        "alvo real",
        "alvo predito",
        title,
    )

    # draw ideal diagonal reference line y=x
    canvas.draw_line(x0, y0 + h, x0 + w, y0, color="#888888", width=2, dash="4 4")

    # draw scatter points
    for t_val, p_val in zip(yt, yp):
        px = x0 + ((t_val - bound_min) / (bound_max - bound_min)) * w
        py = (y0 + h) - ((p_val - bound_min) / (bound_max - bound_min)) * h
        canvas.draw_circle(px, py, r=3.5, color="#1f77b4", alpha=0.7)

    legend_items = [
        ("ideal (y=x)", "#888888", "dash"),
        ("amostras de teste", "#1f77b4", "circle"),
    ]
    _draw_legend(canvas, legend_items, x=580.0, y=65.0)

    canvas.save(output_path)


def plot_metric_comparison(
    series_dict: dict[str, str | pd.DataFrame | list[str]],
    output_path: str,
    metric_col: str = "best_train_rmse",
    y_label: str = "rmse",
    title: str = "comparação",
) -> None:
    """plota curvas de múltiplas séries com faixas sombreadas de desvio padrão para uma dada métrica"""
    palette = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    curves = []

    global_x_min = 1e9
    global_x_max = -1e9
    global_y_max = 1.0

    for idx, (label, src) in enumerate(series_dict.items()):
        df = _load_data_series(src)
        grouped = df.groupby("generation")
        gens = np.array(sorted(grouped.groups.keys()), dtype=np.float64)
        mean_v = grouped[metric_col].mean().values
        std_v = grouped[metric_col].std().fillna(0).values

        global_x_min = min(global_x_min, float(np.min(gens)))
        global_x_max = max(global_x_max, float(np.max(gens)))
        global_y_max = max(global_y_max, float(np.max(mean_v + std_v)))

        color = palette[idx % len(palette)]
        curves.append((label, color, gens, mean_v, std_v))

    canvas = Canvas(800, 500)
    x0, y0, w, h = _setup_plot_axes(
        canvas,
        global_x_min,
        global_x_max,
        0.0,
        global_y_max * 1.1,
        "geração",
        y_label,
        title,
    )

    legend_items = []
    for label, color, gens, mean_vals, std_vals in curves:
        pts = []
        for g, val in zip(gens, mean_vals):
            px = x0 + ((g - global_x_min) / max(1.0, global_x_max - global_x_min)) * w
            py = (y0 + h) - (val / max(1e-9, global_y_max * 1.1)) * h
            pts.append((px, py))

        if np.max(std_vals) > 1e-6:
            x_pts, yu_pts, yl_pts = [], [], []
            for g, m, s in zip(gens, mean_vals, std_vals):
                px = x0 + ((g - global_x_min) / max(1.0, global_x_max - global_x_min)) * w
                py_u = (y0 + h) - ((m + s) / max(1e-9, global_y_max * 1.1)) * h
                py_l = (y0 + h) - (max(0.0, m - s) / max(1e-9, global_y_max * 1.1)) * h
                x_pts.append(px)
                yu_pts.append(py_u)
                yl_pts.append(py_l)
            canvas.draw_shaded_band(x_pts, yu_pts, yl_pts, color, alpha=0.18)

        canvas.draw_polyline(pts, color, width=2)
        legend_items.append((label, color, "line"))

    _draw_legend(canvas, legend_items)
    canvas.save(output_path)

