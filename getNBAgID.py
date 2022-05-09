import http.client
import json

conn = http.client.HTTPSConnection("api-basketball.p.rapidapi.com")

headers = {
    'X-RapidAPI-Host': "api-basketball.p.rapidapi.com",
    'X-RapidAPI-Key': "2a2044f642mshd0c978b998adbd2p1b2e0ajsn77d9e77e5eab"
    }

conn.request("GET", "/games?date=2022-05-09", headers=headers) 

res = conn.getresponse()
data = res.read()
jsondata = json.loads(data)





i = 1
for games in jsondata['response']:
    if(games['league']['id'] == 12):
        print("Game: ", i)
        print("GAME ID: ", games['id'])
        print(games['teams']['home']['name'])
        print(games['teams']['away']['name'])
        i = i + 1