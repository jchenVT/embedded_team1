import paho.mqtt.client as mqtt
import json

def on_connect(client, userdata, flags, rc):
    print("connected")
    client.subscribe('encoder')

def on_message(client, userdata, msg):
    # try reading json
    try:
        msg_dict = json.loads(msg.payload)
    except:
        print('Improper json format!')

    # if debug, update 
    value = msg_dict['value']
    print(value)
    print("==========================================")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("josephcchen.com", 1883, 60)

client.loop_forever()
