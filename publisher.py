from random import randrange, uniform
import time
import paho.mqtt.client as mqtt

client_name = "Publisher"
broker_address = "192.168.1.2"
port = 1883
topic = "topic_name"

client = mqtt.Client(client_name)
client.connect(broker_address, port)

while True:
    nbRandom = uniform(1, 10)
    client.publish(topic, nbRandom)
    print("Just published " + str(nbRandom) + " to topic " + topic)
    time.sleep(1)