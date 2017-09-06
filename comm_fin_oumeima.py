#!/usr/bin/env python

import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import spidev
import struct
import os
import datetime
import glob
import MySQLdb
from time import strftime

#Variables for MySQL
db = MySQLdb.connect(host="localhost",user="root",passwd="bresil", db="temp_database")
cur = db.cursor()

GPIO.setmode(GPIO.BCM)
pipes = [[ 0xE8, 0xE8, 0xF0, 0xF0, 0xE1 ], [0xF0, 0xF0, 0xF0, 0xF0, 0xE1]]

radio = NRF24(GPIO, spidev.SpiDev())
radio.begin(0,17)
radio.setPayloadSize(32)
radio.setChannel(0x50)
radio.setDataRate(NRF24.BR_1MBPS)
radio.setPALevel(NRF24.PA_MIN)
radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()
radio.openReadingPipe(1, pipes[1])
radio.openWritingPipe(pipes[0])

radio.printDetails()
radio.startListening()

message = list("GETDATA")
while len(message) < 32:
	message.append(0)

while True:
	datetimeWrite = (time.strftime("%Y-%m-%d ") + time.strftime("%H:%M:%S"))
	print datetimeWrite

	start = time.time()
	radio.write(message)
	print("Sent the message: {}".format(message))
	radio.startListening()

	while not radio.available(0):
		time.sleep(1/100)
		if time.time() - start > 2:
			print("Timed out.")
			break

	receivedMessage = []
	radio.read(receivedMessage, radio.getDynamicPayloadSize())
	print("Received : {}".format(receivedMessage))
	print("Translating the receivedMessage into unicode characters..")
	
	string = ""
	for n in receivedMessage:
		if (n >= 32   and n<= 126 ):
			string += chr(n)
	print("Our received message decodes to: {}".format(string))
	
	radio.stopListening()
	
	if string:
		temperature = string.split(";")[0]
		humidity = string.split(";")[1]
		brightness = string.split(";")[2]
		print(temperature)
		print(humidity)
		print(brightness)
		sql = ("""INSERT INTO tempLog(datetime,temperature,humidity,brightness) VALUES (%s,%s,%s,%s)""", (datetimeWrite,temperature,humidity,brightness))
		try:
			print "Writing to database..."
			#Execute the SQL command
			cur.execute(*sql)
			#commit your changes in the database
			db.commit()
			print "Write Complete"
		except:
			#rollback in case there is any error
			db.rollback()
			print "Failed writing to database"
		cur.close()
		db.close()


		break
