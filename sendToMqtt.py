#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# PREREQUESTIES:
#  sudo pip install paho-mqtt

import paho.mqtt.client as mqtt
import json

host = '192.168.1.110'
port = 1883

def on_connect(client, userdata, flags, respons_code):
    print('status {0}'.format(respons_code))
    client.subscribe(topic)

#def on_message(client, userdata, msg):
#    print(msg.topic + ' ' + str(msg.payload))

if __name__ == '__main__':
    # Publisherと同様に v3.1.1を利用
    client = mqtt.Client(protocol=mqtt.MQTTv311)
    
    client.on_connect = on_connect
    #client.on_message = on_message

    client.connect(host, port=port, keepalive=60)
    
    # 待ち受け状態にする
    #client.loop_forever()

    import sys
    while True:
        input_line = sys.stdin.readline()
        input_line = input_line.rstrip()
        client.publish("/electricity/all", input_line)
        
