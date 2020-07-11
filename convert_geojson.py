import json

with open("melbourne.geojson", "r") as f:
    data = json.load(f)

feature_data = {}
for feature in data["features"]:
    feature_id = feature['properties']['cartodb_id']
    feature_data[feature_id] = []
    for coord in feature['geometry']['coordinates'][0]:
        for point in coord:
            feature_data[feature_id].append(point)

out = open("melbourne.txt", "w+")
for key in feature_data.keys():
    for coord in feature_data[key]:
        out.write(str(key)+" "+str(coord[0])+" "+str(coord[1])+"\n")
