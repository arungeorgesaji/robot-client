import serial
import time

ser = None

def init_serial(port='/dev/ttyS0', baudrate=115200, timeout=1):
    global ser
    try:
        ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(2)  
        print(f"Connected to ESP32 on {port}")
        return True
    except Exception as e:
        print(f"Failed to connect to ESP32: {e}")
        return False

def send_command(command):
    global ser
    if ser is None:
        print("Serial connection not initialized")
        return None
    
    try:
        clean_command = command.strip().upper()
        ser.write(f"{clean_command}\n".encode())
        print(f"Sent command: {clean_command}")
        
        time.sleep(0.1)
        
        response_lines = []
        while ser.in_waiting > 0:
            response = ser.readline().decode().strip()
            if response:
                response_lines.append(response)
                print(f"ESP32 response: {response}")
        
        return response_lines if response_lines else None
        
    except Exception as e:
        print(f"Error sending command: {e}")
        return None

def flush_serial():
    global ser
    if ser:
        ser.flushInput()
        ser.flushOutput()
        print("Serial buffer flushed")

def get_serial_status():
    global ser
    if ser and ser.is_open:
        return f"Connected to {ser.port} at {ser.baudrate} baud"
    return "Not connected"

def close_serial():
    global ser
    if ser:
        ser.close()
        print("Serial connection closed")
