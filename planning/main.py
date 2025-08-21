from flask import Flask, request, jsonify
from flask_cors import CORS
import threading
import time
from serial_com import init_serial, send_command, close_serial, flush_serial, get_serial_status
from ai import init_ai, scan_mode, cleanup_ai
import cv2

app = Flask(__name__)
CORS(app)  

serial_initialized = False
ai_initialized = False
current_mode = "auto"  
current_speed = 0
current_direction = "STOP"
plastic_detection_active = False

def initialize_system():
    global serial_initialized, ai_initialized
    print("Initializing system components...")
    
    if not init_serial():
        print("Failed to initialize serial communication")
        return False
    
    flush_serial()
    
    if not init_ai():
        close_serial()
        return False
    
    serial_initialized = True
    ai_initialized = True
    send_command("STOP")
    print("System initialization complete")
    return True

def plastic_detection_loop():
    global plastic_detection_active, current_mode
    search_direction = "left"
    search_count = 0
    
    while plastic_detection_active and current_mode == "auto":
        if search_count % 10 == 0:
            print(get_serial_status())
        
        detected_objects = scan_mode()
        
        plastic_detected = False
        
        if detected_objects:
            for obj in detected_objects:
                plastic_keywords = ['pbottle', 'plastic', 'pwaste', 'plastic waste', 'bottle']
                if any(keyword in obj['name'].lower() for keyword in plastic_keywords):
                    plastic_detected = True
                    message = f"PLASTIC DETECTED: {obj['name']}, {obj['depth']:.2f}cm away, position: {obj['position']}"
                    print(message)
                    
                    if obj['position'] == 'front':
                        send_command("FORWARD")
                        current_direction = "FORWARD"
                        print("Moving towards plastic waste")
                    elif obj['position'] == 'left':
                        send_command("TURN_LEFT")
                        current_direction = "TURN_LEFT"
                        print("Turning left towards plastic")
                    elif obj['position'] == 'right':
                        send_command("TURN_RIGHT")
                        current_direction = "TURN_RIGHT"
                        print("Turning right towards plastic")
                    
                    if obj['depth'] < 50:  
                        send_command("SLOW")
                        current_speed = 3
                    elif obj['depth'] < 100:  
                        send_command("MEDIUM")
                        current_speed = 6
                    else:  
                        send_command("FAST")
                        current_speed = 9
                    
                    time.sleep(1)
                    break  
        
        if not plastic_detected:
            print("Searching for plastic waste...")
            
            if search_direction == "left":
                send_command("SLOW")
                send_command("TURN_LEFT")
                current_direction = "TURN_LEFT"
                current_speed = 3
                time.sleep(2)
                send_command("STOP")
                current_direction = "STOP"
                search_direction = "right"
            else:
                send_command("SLOW")
                send_command("TURN_RIGHT")
                current_direction = "TURN_RIGHT"
                current_speed = 3
                time.sleep(2)
                send_command("STOP")
                current_direction = "STOP"
                search_direction = "left"
            
            search_count += 1
            
            if search_count % 5 == 0:
                flush_serial()
        
        time.sleep(0.5)

if not initialize_system():
    print("Warning: System initialization failed. API will run in limited mode.")

@app.route('/api/status', methods=['GET'])
def get_status():
    return jsonify({
        'serial_connected': serial_initialized,
        'ai_initialized': ai_initialized,
        'mode': current_mode,
        'speed': current_speed,
        'direction': current_direction,
        'plastic_detection_active': plastic_detection_active
    })

@app.route('/api/control', methods=['POST'])
def control_motors():
    global current_mode, current_speed, current_direction, plastic_detection_active
    
    if not serial_initialized:
        return jsonify({'error': 'Serial connection not available'}), 500
    
    data = request.json
    command = data.get('command')
    speed = data.get('speed', current_speed)
    
    if command:
        if command.upper() == "AUTO":
            current_mode = "auto"
            plastic_detection_active = True
            detection_thread = threading.Thread(target=plastic_detection_loop)
            detection_thread.daemon = True
            detection_thread.start()
            return jsonify({'status': 'switched to auto mode'})
        
        elif command.upper() == "MANUAL":
            current_mode = "manual"
            plastic_detection_active = False
            send_command("STOP")
            current_direction = "STOP"
            return jsonify({'status': 'switched to manual mode'})
        
        if current_mode == "manual":
            valid_commands = ["FORWARD", "BACKWARD", "LEFT", "RIGHT", "STOP", "SLOW", "MEDIUM", "FAST"]
            if command.upper() in valid_commands:
                send_command(command.upper())
                current_direction = command.upper() if command.upper() in ["FORWARD", "BACKWARD", "LEFT", "RIGHT", "STOP"] else current_direction
                
                if command.upper() == "SLOW":
                    current_speed = 3
                elif command.upper() == "MEDIUM":
                    current_speed = 6
                elif command.upper() == "FAST":
                    current_speed = 9
                
                return jsonify({'status': 'command executed', 'command': command.upper()})
            else:
                return jsonify({'error': 'invalid command'}), 400
    
    if speed is not None and 0 <= speed <= 9:
        current_speed = speed
        if speed == 0:
            send_command("STOP")
            current_direction = "STOP"
        elif speed <= 3:
            send_command("SLOW")
        elif speed <= 6:
            send_command("MEDIUM")
        else:
            send_command("FAST")
        
        return jsonify({'status': 'speed changed', 'speed': speed})
    
    return jsonify({'error': 'no valid command or speed provided'}), 400

@app.route('/api/objects', methods=['GET'])
def get_detected_objects():
    if not ai_initialized:
        return jsonify({'error': 'AI not initialized'}), 500
    
    detected_objects = scan_mode()
    plastic_objects = []
    
    for obj in detected_objects:
        plastic_keywords = ['pbottle', 'plastic', 'pwaste', 'plastic waste', 'bottle']
        if any(keyword in obj['name'].lower() for keyword in plastic_keywords):
            plastic_objects.append({
                'name': obj['name'],
                'depth': obj['depth'],
                'position': obj['position']
            })
    
    return jsonify({'objects': plastic_objects})

@app.route('/api/shutdown', methods=['POST'])
def shutdown():
    global plastic_detection_active
    plastic_detection_active = False
    send_command("STOP")
    time.sleep(1)
    cleanup_ai()
    close_serial()
    return jsonify({'status': 'system shutdown complete'})

if __name__ == '__main__':
    try:
        app.run(host='0.0.0.0', port=5000, debug=True)
    except KeyboardInterrupt:
        print("Shutting down...")
        plastic_detection_active = False
        send_command("STOP")
        time.sleep(1)
        cleanup_ai()
        close_serial()
