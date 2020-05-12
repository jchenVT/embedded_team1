import paho.mqtt.client as mqtt
#import matplotlib.pyplot as plt
import keyboard
import json


RED_THRESH = 120
GREEN_THRESH = 120
BLUE_THRESH = 120

currentColor = '---'


def EXIT_NOW():
    exit()


def on_connect(client, userdata, flags, rc):
    print("Connecting to the \'arm_sensor\' topic...")
    client.subscribe('arm_sensor')


def on_message(client, userdata, msg):
    global currentColor

    try:
        msg_dict = json.loads(msg.payload)
    except:
        print('[ERROR]: Message in wrong format')
        EXIT_NOW()
    
    #if msg_dict['sensorID'] == 1:

    #if msg_dict['sensorID'] == 2:
    
    if msg_dict['sensorID'] == 3:

        print(msg.payload)

        if msg_dict['r'] > RED_THRESH:
            currentColor = 'RED'
        if msg_dict['g'] > GREEN_THRESH:
            currentColor = 'GREEN'
        if msg_dict['b'] > BLUE_THRESH:
            currentColor = 'BLUE'
        if msg_dict['r'] < RED_THRESH and msg_dict['g'] < GREEN_THRESH and msg_dict['b'] < BLUE_THRESH:
            currentColor = '---'

    print("\rCURRENT COLOR = {}     ".format(currentColor), end="\r")

    if keyboard.is_pressed('q'):
        print('\nDisconnecting...')
        client.loop_stop()
        client.disconnect()
        EXIT_NOW()



client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("josephcchen.com", 1883, 60)

client.loop_forever()