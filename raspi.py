import paho.mqtt.client as mqtt
import datetime as datetime
import os

MQTT_Broker = "192.168.43.167"
MQTT_Port = 1883
Keep_Alive_Interval = 45
# MQTT multi topic for subscribe
MQTT_Topic = [("id", 0), ("suhu", 0), ("tekan", 0)]

mqttc = mqtt.Client("Basestation-0001")
# Read Data Function
def on_message(mosq, obj, msg):
    delimiter = data.split(',')
    dataraw = "id={},{}={},".format(delimiter[0], topic, delimiter[1])

    try:
        datetime.strptime(delimiter[2], "%Y-%m-%d %H:%M:%S.%f")
        return "id={},{}={},timestamp={}".format(delimiter[0], topic, delimiter[1], delimiter[2])
    
    except:
        return "id={},{}={},suhu={},tekan={},timestamp={}".format(delimiter[0], topic, delimiter[1], delimiter[2], delimiter[3], delimiter[4])
    
def saveJsonIntoFile(json_string):
    now = datetime.datetime.now()
    lastMonth = str(now.month)
    lastDay = str(now.day)
    lastHour = str(now.hour)
    lastMinute = str(now.minute)
    
    dir = "/home/pi/data/{:s}/{:s}/".format(lastMonth, lastDay)
    if not os.path.exists(dir):
        os.makedirs(dir)
        
    fileku = open(dir+"{:s}-{:s}-{:s}.txt".format(lastMonth, lastDay, lastHour), 'a+')
    fileku.write(json_string)
    fileku.write("\n")
    fileku.close
    # TODO : Add saveToArchive funtion

def on_subscribe(mosq, obj, mid, granted_qos):
    print(str(mosq.topic))
    pass

# Show error if connection unsuccessful
def on_connect(client, userdata, flag, rc):
    if rc==0:
        print("Connection successful\n")
    if rc==1:
        print("Connection refused - incorrect protocol version\n")
    if rc==2:
        print("Connection refused - invalid client identifier\n")
    if rc==3:
        print("Connection refused - server unavailable\n")
    if rc==4:
        print("Connection refused - not authorised\n")

print("Connection to Broker with Address " + str(MQTT_Broker) + ":" + str(MQTT_Port))
print("==================================")

# Assign Event Callbacks
mqttc.on_message = on_message
mqttc.on_subscribe = on_subscribe
mqttc.on_connect = on_connect

# Connect
mqttc.connect(MQTT_Broker, int(MQTT_Port), int(Keep_Alive_Interval))
mqttc.subscribe(MQTT_Topic)

#Continue the network loop
mqttc.loop_forever()