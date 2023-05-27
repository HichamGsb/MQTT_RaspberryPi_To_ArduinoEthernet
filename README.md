# MQTT_RaspberryPi_To_ArduinoEthernet

## Description

Ce projet permet de faire communiquer une *Raspberry Pi 4* et une carte *Arduino Ethernet*.

Le code Arduino permet de contrôler des moteurs à courant continu via une MD25, les moteurs s'arrêtent initialement lorsque la distance envoyé par la Raspberry Pi est inférieure à 15 cm.

### Fonctionnement du protocole MQTT

- __Broker :__ Serveur par lequel les données transites via des topics.

- __Topic :__ Identifiant d’un type de message (par exemple : “__Temperature__” ou “__Distance__”) servant à factoriser l’information reçu sur le serveur.

- __Client :__ Dispositif se connectant au serveur pour envoyer et / ou recevoir des messages sur un ou plusieurs __topics__.

*Le fonctionnement du protocole __MQTT__ est basé sur les __topics__ présents sur le __Broker__ :*

1. *Lancement du __broker__.*
2. *Un __client “publisher”__ se connecte au Broker MQTT, crée un topic pour y publier des informations.*
3. *Un __client “subscriber”__ se connecte au Broker MQTT et s’inscrit à un ou plusieurs topics afin d’en recevoir les messages en temps réel.*

---

## Pré-requis

### Librairies Arduino

| Librairie | Lien (si disponible) | Version | Author |
| --- | --- | --- | --- |
| Arduino.h | <https://www.arduino.cc/reference/en/> | ——— | —————— |
| PubSubClient.h | <https://github.com/knolleary/pubsubclient.git> | 2.8.0 | Nick O’Leary |
| SoftwareSerial.h | <https://docs.arduino.cc/learn/built-in-libraries/software-serial> | ——— | —————— |
| avr/sleep.h | Included in Arduino IDE | ——— | —————— |
| SPI.h | <https://github.com/bxparks/AceSPI.git> | 0.3.0 | Brian T. Park |
| Ethernet.h | <https://github.com/arduino-libraries/Ethernet.git> | 2.0.2 | Various authors |

---

### Librairies Python

| Librairie | Lien | Version | Author |
| --- | --- | --- | --- |
| paho-mqtt | <https://github.com/eclipse/paho.mqtt.python.git> | 1.6.1 | Eclipse |

---

## Installation

### Réseau

1. Brancher le câble ethernet à la Raspberry Pi et l’Arduino Ethernet
2. Re-configurer l’interface ethernet 0 (eth0) afin de désactiver le DHCP

    ```bash
    sudo vi /etc/dhcpcd.conf
    ```

3. Modifier les lignes associées.

    ```bash
    interface eth0
    static ip_address=192.168.1.2/24
    static routers=#nothing
    static domain_name_servers=#nothing
    noipv6
    static domain_search=#nothing
    ```

4. Redémarrer le réseau

    ```bash
    sudo systemctl restart networking
    ```

---

### Arduino

[MotorControlTroughRaspi.ino](/MotorControlTroughRaspi.ino)

- Définir l’adresse mac de l’arduino :

    ```arduino
    // A changer selon l'adresse mac de l'arduino
    uint8_t mac[] = {0x90, 0xA2,0xDA, 0x00, 0x72, 0x8E}
    ```

- Définir les attributs de la connexion :

    ```arduino
    #include <Ethernet.h> //Version 2.0.1 lors des tests
    
    IPAddress ip(192, 168, 1, 3) //Adresse assignée à l'arduino (adresse de la pi+1)
    IPAddress dns(192, 168, 1, 2)
    IPAddress gateway(192, 168, 1, 2)
    IPAddress subnet(255, 255, 255, 0)
    IPAddress mqtt_server(192, 168, 1, 2)
    const int mqtt_port = 1883;
    ```

---

### Mosquitto

[mosquitto.conf](/mosquitto.conf)

- __Installation & Configuration__
    1. Installer Mosquitto

        ```bash
        sudo apt install mosquitto
        ```

    2. Modifier le port et l’adresse IP associés au `listener` dans le fichier `mosquitto.conf` disponible ci-dessus.

        ```bash
        listener 1883 192.168.1.2
        ```

- Lancer le broker :

    ```bash
    mosquitto -c path/to/mosquitto.conf
    
    # Si ça ne fonctionne pas
    /usr/local/sbin/mosquitto -c path/to/mosquitto.conf
    ```

- Arrêter le broker :

    ```bash
    ctrl+c
      ou
    pkill mosquitto
    ```

---

### Publisher & Subscriber

[publisher.py](/publisher.py)

[subscriber.py](/subscriber.py)

1. Modifier les adresses IP et les ports dans chaque fichier
2. Lancer le publisher

    ```bash
    python3 publisher.py
    ```

3. Lancer le subscriber

    ```bash
    python3 subscriber.py
    ```
