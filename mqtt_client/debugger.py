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
    for key in topics:
        client.subscribe(key)

def on_message(client, userdata, msg):
    print ('received')
    fields = topics.get(msg.topic)
    try:
        msg_dict = json.loads(msg.payload)
    except:
        print('Impropert json format!')
        client.publish(f'{msg.topic}/debug', json.dumps({'success': False, 'error' : 'improper json'}))
    missing_fields = []
    for field in fields:
        if msg_dict.get(field) is None:
            missing_fields.append(field) 
        else:
            continue

    if len(missing_fields) > 0:
        print("some missing")
        try:
            client.publish(f'{msg.topic}/debug', json.dumps({'success': False, 'error' : 'missing field', 'missing': ' ,'.join(missing_fields)}))
        except:
            print('publish fail')
            return
    else:
        print("none missing")
        client.publish(f'{msg.topic}/debug', json.dumps({'success': True}))



client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("josephcchen.com", 1883, 60)


client.loop_forever()
