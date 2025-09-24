import folium

# GPS 数据
gps_points = [
    [39.93661, 116.49340],
    [39.93659, 116.49355],
    [39.93658, 116.49381],
    [39.93657, 116.49416],
    [39.93657, 116.49444],
    [39.93657, 116.49479],
    [39.93656, 116.49499],
    [39.93680, 116.49589],
    [39.93711, 116.49697],
    [39.93731, 116.49690],
    [39.93711, 116.49615],
    [39.93764, 116.49600]
]

special_point = [39.93715, 116.49662]
m = folium.Map(location=gps_points[0], zoom_start=15)
folium.Marker(
        location=special_point,
        popup=f"Index: 0 (Special)",
        icon=folium.Icon(color="red", icon="star")
    ).add_to(m)

for idx, point in enumerate(gps_points):
    folium.Marker(
        location=point,
        tooltip=folium.Tooltip(f"Index: {idx}", permanent=True),
        icon=folium.Icon(color="blue", icon="info-sign")
    ).add_to(m)

folium.PolyLine(gps_points, color="red", weight=2.5, opacity=1).add_to(m)

m.save("gps_map.html")
print("地图已生成，请用浏览器打开 gps_map.html 查看。")
