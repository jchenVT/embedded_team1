import paho.mqtt.client as mqtt
import matplotlib.pyplot as plt
import keyboard
import json
import sys

xdata = []
ydata = []

first_msg = True

def EXIT_NOW():
    plt.axis([0,xdata[-1], 0, 127])
    plt.plot(xdata, ydata)
    plt.ylabel('Encoder Ticks converted to Motor Speed Values (0-127)')
    plt.xlabel('Time in seconds (s)')
    plt.title('PI Algorithm for Encoder 128')
    plt.savefig('PID_values.png')
    
    exit();

def on_connect(client, userdata, flags, rc):
    print("Connecting...")
    client.subscribe('encoder')
    
def on_message(client, userdata, msg):
    # try reading json
    try:
        msg_dict = json.loads(msg.payload)
        print("....PID value received")
    except:
        print('Improper json format!')

    # update 
    ydata.append(msg_dict['value'])
    xdata.append(len(xdata)*0.2);
    
    if keyboard.is_pressed('q'):
        print('Disconnecting...')
        client.loop_stop()
        client.disconnect()
        EXIT_NOW()
    
    

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("josephcchen.com", 1883, 60)

client.loop_forever()
