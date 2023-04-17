import paho.mqtt.client as mqttClient
import time

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to broker")
        global Connected
        Connected = True
    else:                               
        print("Connection failed")
  
def on_message(client, userdata, message):
    print("Message received: " + str(message.payload.decode("utf-8")))
  
Connected = False

client_name = "Subscriber"
broker_address="192.168.1.2"
port = 1883
topic = "topic_name"

client = mqttClient.Client(client_name)
client.on_connect= on_connect
client.on_message= on_message
  
client.connect(broker_address, port=port)
client.loop_start()
  
while Connected != True:
    time.sleep(0.1)

client.subscribe(topic)
  
try:                                                              
    while True:
        time.sleep(1)

except KeyboardInterrupt:
    print ("Arrêt du subscriber ", client_name)
    client.disconnect()
    client.loop_stop()