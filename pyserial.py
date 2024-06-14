import serial
import time
import sys

def open_serial_port(port, baudrate):
    try:
        ser = serial.Serial(port, baudrate)
        time.sleep(2)  # Wait for the serial connection to initialize
        return ser
    except serial.SerialException as e:
        print(f"Error opening serial port {port}: {e}")
        sys.exit(1)

# Configure the serial port (update 'COM14' to the correct port for your system)
port = 'COM14'  # Change 'COM14' to your port
baudrate = 115200

ser = open_serial_port(port, baudrate)

# File to append the serial output
file_path = "serial_output.txt"

# Open the file in write mode to clear its contents
with open(file_path, 'w') as file:
    pass  # Just open and close to clear the file

# Reopen the file in append mode for writing the serial output
with open(file_path, 'a') as file:
    while True:
        if ser.in_waiting > 0:
            try:
                line = ser.readline().decode('utf-8').strip()
                print(f"Received: {line}")  # Print to console
                file.write(line + "\n")  # Append to file
            except Exception as e:
                print(f"Error reading from serial port: {e}")
