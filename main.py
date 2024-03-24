import machine
import network
import socket
import utime
import urandom as random
# from machine import I2C, Pin
# from ina219 import INA219

# i2c = machine.I2C(scl=machine.Pin(22), sda=machine.Pin(21))
# ina219 = INA219(i2c=i2c)

def update_capacity(current_mA):
    global capacity_mAh, last_reading_time, last_reading_hms
    current_time = utime.time()
    elapsed_time_hours = (current_time - last_reading_time) / 3600
    capacity_mAh += (current_mA * elapsed_time_hours)
    last_reading_time = current_time
    last_reading_time_tuple = utime.localtime(current_time)
    last_reading_hms = "{:02d}:{:02d}:{:02d}".format(last_reading_time_tuple[3], last_reading_time_tuple[4], last_reading_time_tuple[5])
    
def reset_csv(filename):
    with open(filename, "w") as file:
        file.write("Bus Voltage,Current,Power,Capacity mAh\n")
    
def log_data_to_csv(capacity):
    filename = "sensor_data.csv"
    with open(filename, "a") as file:
        file.write(f"{bus_voltage},{current},{power},{capacity_mAh}\n")
        
capacity_mAh = 0.0
last_reading_time = utime.time()

def connect_wifi(ssid, password):
    wlan = network.WLAN(network.STA_IF)  
    wlan.active(True)
    if not wlan.isconnected():  
        print('Connecting to network...')
        wlan.connect(ssid, password)  
        while not wlan.isconnected():
            pass
    print('Network config:', wlan.ifconfig()) 

connect_wifi('your-ssid', 'your-password')

reset_csv("sensor_data.csv")
    
addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
s = socket.socket()
s.bind(addr)
s.listen(1)

while True:
    cl, addr = s.accept()
    request = cl.recv(1024)
    
    # bus_voltage = ina219.bus_voltage()
    # current = ina219.current()
    # power = ina219.power()
    
    bus_voltage = 3.7
    current = 2050 + 100*random.random()
    power = 'test'
    
    update_capacity(current)
    log_data_to_csv(capacity_mAh)
    print(current)
     
    response = f"""HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>
    <html>
        <head>
            <title>INA219 Readings</title>
            <meta http-equiv="refresh" content="1">
            <script>
                setTimeout(function(){{
                   window.location.reload(1);
                }}, 5000);
            </script>
        </head>
        <body>
            <h1>INA219 Sensor Readings</h1>
            <p>Time Elapsed: {last_reading_hms}</p>
            <table>
              <tr>
                <th>Pin Address</th>
                <th>Bus Voltage (V)</th>
                <th>Current (mA)</th>
                <th>Power (mW)</th>
                <th>Capacity Used (mAh)</th>
              </tr>
              <tr>
                <td>1</td>
                <td>{bus_voltage}</td>
                <td>{current}</td>
                <td>{power}</td>
                <td>{capacity_mAh:.2f}</td>
              </tr>
            </table>
        </body>
    </html>
    """
    cl.sendall(response.encode())
    cl.close()
