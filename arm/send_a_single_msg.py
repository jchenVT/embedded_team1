import paho.mqtt.client as mqtt
import json
import time

client = mqtt.Client()
client.connect("josephcchen.com", 1883, 60)

for i in range(5):
    client.publish('arm_sensor', json.dumps({"sensorID": 1, "sensorValue": 30}))
    time.sleep(10)