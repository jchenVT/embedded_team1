import paho.mqtt.client as mqtt
import argparse
import json

topics = {}
topics_num = {}
topics_msgs = {}
topics_time_last = {}

parser = argparse.ArgumentParser()
parser.add_argument('filename')
args = parser.parse_args()
with open(args.filename, "r") as file:
    for line in file:
        line_list = line.split()
        topics.update({line_list[0] : line_list[1:]})
        topics_num.update({line_list[0] : (0,0)})
        topics_msgs.update({line_list[0] : 0})

def on_connect(client, userdata, flags, rc):
    print("connected")
    for key in topics:
        client.subscribe(key)
    client.subscribe('debug')

def on_message(client, userdata, msg):
    # try reading json
    try:
        msg_dict = json.loads(msg.payload)
    except:
        print('Improper json format!')
        client.publish(f'{msg.topic}/debug', json.dumps({'success': False, 'error' : 'improper json'}))

    # if debug, update 
    if msg.topic == 'debug':
        attempted, received = msg_dict['attemptPubCount'], msg_dict['recvSubCount']
        topics_num.update({msg.topic : (attempted, received)}) 
        subTopic = msg_dict['subTopic'] 
        pubTopic = msg_dict['pubTopic'] 
        print("==========================================")
        print(f'From {pubTopic}, status: {msg_dict["status"]}')
        print(f'{pubTopic}: attempted - {attempted}, recorded = {topics_msgs.get(pubTopic)}')
        print(f'{subTopic}: received  - {received }, recorded = {topics_msgs.get(subTopic)}')

        if attempted != topics_msgs.get(pubTopic):
            topics_msgs.update({pubTopic : attempted})
        if received != topics_msgs.get(pubTopic):
            topics_msgs.update({subTopic : received})

    else:
        count = topics_msgs.get(msg.topic) 
        count += 1
        topics_msgs.update({msg.topic : count}) 
        fields = topics.get(msg.topic)
        missing_fields = []
        for field in fields:
            if msg_dict.get(field) is None:
                missing_fields.append(field) 
            else:
                continue

        if len(missing_fields) > 0:
            print(f"{msg.topic}: some topics missing")
            try:
                client.publish(f'{msg.topic}/debug', json.dumps({'success': False, 'error' : 'missing field', 'missing': ' ,'.join(missing_fields)}))
            except:
                print('publish fail')
                return
        else:
            # print(f"{msg.topic}: all fields found")
            client.publish(f'{msg.topic}/debug', json.dumps({'success': True}))



client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("josephcchen.com", 1883, 60)


client.loop_forever()
