import sqlite3
import serial

ser = serial.Serial('/dev/ttyACM0', 9600)
ser.reset_input_buffer()

while(True):
    if ser.in_waiting > 0:
        result = int(ser.readline().decode('utf-8').rstrip())
        print(f'Wiadomosc: {result}')
        
        con = sqlite3.connect("game.db")
        cur = con.cursor()

        cur.execute('''CREATE TABLE IF NOT EXISTS results (result INTEGER)''')
        cur.execute('''INSERT INTO results VALUES(:result)''', {"result" : result})
        con.commit()

        data = cur.execute('''SELECT result FROM results ORDER BY result DESC LIMIT 3''')
        message = ""

        for row in data:
            message += str(row[0]) + "\n"

        print(f'Odpowiedz: {message}')
        ser.write(message.encode())

        cur.close()
        con.close()
   
