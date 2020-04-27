import paho.mqtt.client as mqtt
import json
import time

client = mqtt.Client()
client.connect("josephcchen.com", 1883, 60)
current_pos = 'm'

while 1:
    cmd = input("enter a command: ")
    if cmd == "start":
        client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 90, "zPos": 90, "clawPos": 00}))
        current_pos = 'm'
    elif cmd == "lsearch":
        client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 90, "zPos": 90, "clawPos": 0}))
        current_pos = 'l'
    elif cmd == "rsearch":
        client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 90, "zPos": 90, "clawPos": 0}))
        current_pos = 'r'
    elif cmd == "lower":
        if current_pos == 'm':
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 130, "zPos": 40, "clawPos": 90}))
        elif current_pos == 'l':
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 130, "zPos": 40, "clawPos": 90}))
        elif current_pos == 'r':
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 130, "zPos": 40, "clawPos": 90}))

    elif cmd == "lower2":
        if current_pos == 'm':
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 130, "zPos": 40, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 130, "zPos": 40, "clawPos": 90}))

        elif current_pos == 'l':
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 130, "zPos": 40, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 130, "zPos": 40, "clawPos": 90}))

        elif current_pos == 'r':
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 130, "zPos": 40, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 130, "zPos": 40, "clawPos": 90}))

    elif cmd == "pickup":
        if current_pos == 'm':
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 130, "zPos": 40, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 90, "zPos": 90, "clawPos": 40}))
        elif current_pos == 'l':
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 130, "zPos": 40, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 90, "zPos": 90, "clawPos": 40}))
        elif current_pos == 'r':
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 130, "zPos": 40, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 90, "zPos": 90, "clawPos": 40}))

    elif cmd == "release":
        if current_pos == 'm':
           client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 90, "zPos": 90, "clawPos": 100}))
        elif current_pos == 'l':
           client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 90, "zPos": 90, "clawPos": 100}))
        elif current_pos == 'r':
           client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 90, "zPos": 90, "clawPos": 100}))

           
