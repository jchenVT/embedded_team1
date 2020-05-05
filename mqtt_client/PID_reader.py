import paho.mqtt.client as mqtt
import matplotlib.pyplot as plt
import keyboard
import time
import json
import sys

KP = input("Enter KP value: ")
KI = input("Enter KI value: ")

first_msg = True

maxSize = 100
start = time.time()
end = start

plt.figure()
ln, = plt.plot([])
plt.ylim(0, 127)
plt.ion()
plt.ylabel('Encoder Ticks converted to Motor Speed Values (0-127)')
plt.xlabel('Time in seconds (s)')
plt.title('PI Algorithm for Encoder 128')
plt.show()

xdata = []
ydata = []

def EXIT_NOW():
    plt.savefig('PID_values.png')    
    exit();

def on_connect(client, userdata, flags, rc):
    print("Connecting...")
    print("Sending current PID constants: ")
    print("....KP: "+KP)
    print("....KI: "+KI)
    client.subscribe('encoder')
    client.publish("PID_param", payload="{\"KP\": "+KP+", \"KI\": "+KI+"}", retain=True)
    client.publish("rover_sensor", payload="{\"move_to_point\": \"0\", \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}", retain=True)
    
def on_message(client, userdata, msg):
    # try reading json
    try:
        msg_dict = json.loads(msg.payload)
        print("....PID value received")
    except:
        print('Improper json format!')

    # update    
    end = time.time()
    
    if len(xdata) == maxSize:
        xdata.pop(0)
        ydata.pop(0)
    
    xdata.append(end - start)
    ydata.append(msg_dict['value'])
    
    ln.set_xdata(xdata)
    ln.set_ydata(ydata)
    
    plt.xlim(xdata[0], xdata[-1])

    if len(xdata) > 1:
        plt.draw()
        plt.pause(0.001)
    
    if len(xdata) == 5:
        client.publish("rover_sensor", payload="{\"move_to_point\": \"1\", \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 1}")
    
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
