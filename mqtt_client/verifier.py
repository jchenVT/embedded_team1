import paho.mqtt.client as mqtt
import argparse
import json

topics = {}

parser = argparse.ArgumentParser()
parser.add_argument('filename')
args = parser.parse_args()
with open(args.filename, "r") as file:
    for line in file:
        line_list = line.split()
        topics.update({line_list[0] : line_list[1:]})

def on_connect(client, userdata, flags, rc):
    print("connected")
    client.subscribe("$SYS/#")

def on_message(client, userdata, msg):
    fields = topics.get(msg.topic)
    msg_dict = json.dumps(msg.payload)
    missing_fields = []
    for field in fields:
        if msg_dict.get(field) is None:
            missing_fields.append(field) 
        else:
            continue
    if missing_fields.count() > 0:
        client.publish(f'debug/{msg.topic}', f"missing: {' ,'.join(missing_fields)}")
    else:
        client.publish(f'debug/{msg.topic}', 'success')

        


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("josephcchen.com", 1883, 60)

for key in topics:
    client.subscribe(key)

client.loop_forever()
