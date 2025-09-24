import json
import folium
import sys

def main():
    if len(sys.argv) < 2:
        print("用法: python draw_route.py route.json")
        return

    filename = sys.argv[1]

    # 读取 JSON 文件
    with open(filename, "r", encoding="utf-8") as f:
        data = json.load(f)

    # 提取 linePoints
    coords = []
    for route in data["routeResponse"]["routeInfos"]:
        for link in route["routeLinkDescriptions"]:
            for pt in link["linePoints"]:
                coords.append((pt["latitude"], pt["longitude"]))

    if not coords:
        print("未找到任何 linePoints 数据")
        return

    # 创建地图
    m = folium.Map(location=coords[0], zoom_start=18)

    # 画路线
    folium.PolyLine(coords, color="blue", weight=5, opacity=0.8).add_to(m)

    # 起点和终点标记
    folium.Marker(coords[0], popup="起点", icon=folium.Icon(color="green")).add_to(m)
    folium.Marker(coords[-1], popup="终点", icon=folium.Icon(color="red")).add_to(m)

    # 中间点（可选）
    for idx, (lat, lon) in enumerate(coords[1:-1], 1):
        folium.CircleMarker([lat, lon], radius=3, color="blue", fill=True).add_to(m)

    # 保存到 HTML
    out_file = "gps_map.html"
    m.save(out_file)
    print(f"地图已生成: {out_file}")

if __name__ == "__main__":
    main()
