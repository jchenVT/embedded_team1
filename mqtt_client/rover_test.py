import paho.mqtt.client as mqtt
import keyboard
import time

start = False

def on_connect(client, userdata, flags, rc):
    print("Connecting...")
    client.subscribe('start_rover')
    client.publish("rover_sensor", payload="{\"move_to_point\": 0, \"point_x\": 0.2, \"point_y\": 0.1, \"angle_rotate\": 0}")

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
        if (count < 15):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 1}")
        elif (count < 30):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        elif (count < 45):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 1}")
        elif (count < 60):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        elif (count < 75):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 1}")
        elif (count < 90):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        elif (count < 105):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 1}")
        elif (count < 120):
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        elif (count < 145):
            client.publish("rover_sensor", payload="{\"move_to_point\": 0, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": 0}")
        else:
            client.publish("rover_sensor", payload="{\"move_to_point\": 1, \"point_x\": 0, \"point_y\": 0, \"angle_rotate\": -1}")
        
        count += 1
    
    time.sleep(0.2)
