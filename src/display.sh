python3 graph.py $1
# write json file for interface.html about the folder name
echo "{\"folder_name\": $1}" > current.json
python3 interface.py $1
