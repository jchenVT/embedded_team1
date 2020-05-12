import paho.mqtt.client as mqtt
import json
import time

client = mqtt.Client()
client.connect("josephcchen.com", 1883, 60)
arm_state = 0;
current_pos = 'm'

while 1:
    if arm_state == 0: # start phase
        client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 90, "zPos": 180, "clawPos": 00}))
        time.sleep(1)
        print("arm is starting")
        current_pos = 'm'
        time.sleep(1)
        cmd_in = input("are you looking for a blue cube? (y/n) ")
        while cmd_in != "y":
            cmd_in = input("i think you want a blue cube (y/n) ")
        arm_state = 1;
    elif arm_state == 1: # search phase
        print("entering search phase: searching for the color blue")
        client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 90, "zPos": 180, "clawPos": 0}))
        time.sleep(1)
        sensor_input = input("enter if blue or not (y/n): ")
        while sensor_input != 'y' and sensor_input != 'n':
            sensor_input = input("enter if blue or not (y/n): ")
        if sensor_input == 'y':
            print("detected blue")
            current_pos = 'l'
            arm_state = 2;
            continue
        else:
            print("did not detect blue")
        
        client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 90, "zPos": 180, "clawPos": 00}))
        time.sleep(1)
        sensor_input = input("enter if blue or not (y/n): ")
        while sensor_input != 'y' and sensor_input != 'n':
            sensor_input = input("enter if blue or not (y/n): ")
        if sensor_input == 'y':
            print("detected blue")
            current_pos = 'm'
            arm_state = 2;
            continue
        else:
            print("did not detect blue")
            
        client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 90, "zPos": 180, "clawPos": 0}))
        time.sleep(1)
        sensor_input = input("enter if blue or not (y/n): ")
        while sensor_input != 'y' and sensor_input != 'n':
            sensor_input = input("enter if blue or not (y/n): ")
        if sensor_input == 'y':
            print("detected blue")
            current_pos = 'r'
            arm_state = 2;
            continue
        else:
            print("did not detect blue")
    
    elif arm_state == 2: 
        print("picking up object")
        if current_pos == 'm':
            
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 140, "zPos": 140, "clawPos": 90}))
        elif current_pos == 'l':
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 140, "zPos": 140, "clawPos": 90}))
        elif current_pos == 'r':
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 140, "zPos": 140, "clawPos": 90}))
        
        time.sleep(1)
            
        if current_pos == 'm':
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 130, "zPos": 140, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 90, "zPos": 180, "clawPos": 40}))
        elif current_pos == 'l':
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 130, "zPos": 140, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 90, "zPos": 180, "clawPos": 40}))
        elif current_pos == 'r':
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 130, "zPos": 140, "clawPos": 40}))
            time.sleep(1)
            client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 90, "zPos": 180, "clawPos": 40}))
            
        arm_state = 3
        
    elif arm_state == 3:
        print("waiting for rover")
        rover_input = input("is the rover ready? (y/n) ")
        while rover_input != 'y':
            rover_input = input("is the rover ready? (y/n) ")
        
        arm_state = 4
        
    elif arm_state == 4:
        print("releasing object")
        if current_pos == 'm':
           client.publish('arm_pos', json.dumps({"xPos": 90, "yPos": 90, "zPos": 180, "clawPos": 100}))
        elif current_pos == 'l':
           client.publish('arm_pos', json.dumps({"xPos": 0, "yPos": 90, "zPos": 180, "clawPos": 100}))
        elif current_pos == 'r':
           client.publish('arm_pos', json.dumps({"xPos": 180, "yPos": 90, "zPos": 180, "clawPos": 100}))
        
        arm_state = 0
        print("returning to starting position")
        
        
        