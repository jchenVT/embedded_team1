import paho.mqtt.client as mqtt
import keyboard
import time

start = False

def on_connect(client, userdata, flags, rc):
    print("Connecting...")
    client.subscribe('start_rover')
    client.publish("PID_param", payload="{\"KP\": 0.2, \"KI\": 0.1}", retain=True)

def on_message(client, userdata, msg):
    print("Starting...")
    global start
    start = True
    

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("josephcchen.com", 1883, 60)
client.loop_start()

count = 0

while True:
        
    if keyboard.is_pressed('q'):
        print('Exiting...')
        client.loop_stop()
        client.disconnect()
        exit()
                
    if start:
        if (count < 12):
            client.publish("rover_sensor", payload="{\"move_to_point\": \"1\", \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 1}")
        elif (count < 24):
            client.publish("rover_sensor", payload="{\"move_to_point\": \"0\", \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        elif (count < 36):
            client.publish("rover_sensor", payload="{\"move_to_point\": \"1\", \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        elif (count < 48):
            client.publish("rover_sensor", payload="{\"move_to_point\": \"0\", \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        else:
            client.publish("rover_sensor", payload="{\"move_to_point\": \"1\", \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": -1}")
        
        count += 1
    
    time.sleep(0.5)
