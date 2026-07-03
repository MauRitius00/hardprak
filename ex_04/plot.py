"""
Exercise 04: Real-Time IMU Visualization

Complete all TODO sections.
"""

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# =====================
# TODO (1): Set correct serial port
# =====================
# HINT: Find your board's COM port (Windows) or /dev/ttyUSB* (Linux/Mac)
# Common Windows ports: COM3, COM4, COM5, COM6
PORT = '/dev/cu.usbmodem1401'   # TODO: Change to your board's port
BAUD = 115200

WINDOW_SIZE = 100

# =====================
# TODO (2): Initialize serial connection
# =====================
# HINT: Use serial.Serial(PORT, BAUD) to create connection
ser = serial.Serial(PORT, BAUD)


# =====================
# TODO (3): Create buffers for ax, ay, az, gyrX, gyrY, gyrZ
# =====================
# HINT: Use deque(maxlen=WINDOW_SIZE) for each sensor axis
ax_data = deque(maxlen=WINDOW_SIZE)
ay_data = deque(maxlen=WINDOW_SIZE)
az_data = deque(maxlen=WINDOW_SIZE)
gyrX_data = deque(maxlen=WINDOW_SIZE)
gyrY_data = deque(maxlen=WINDOW_SIZE)
gyrZ_data = deque(maxlen=WINDOW_SIZE)


# =====================
# TODO (4): Initialize text variables
# =====================
# HINT: Store orientation and both gesture types
orientation_text = ""
fsm_gesture_text = ""
gyro_gesture_text = ""


# =====================
# TODO (5): Setup plot with dual subplots
# =====================
# HINT: Create 2 vertical subplots, 3 lines per subplot, text displays, and configure axes
# HINT: Accelerometer subplot: y-range [-15, 15], title "Accelerometer Data (m/s²)"
# HINT: Gyroscope subplot: y-range [-500, 500], title "Gyroscope Data (°/s)"
fig = plt.figure(figsize=(10, 8))
ax1 = fig.add_subplot(2, 1, 1)
ax2 = fig.add_subplot(2, 1, 2)
line_ax, = ax1.plot([], [], label="ax", color="red")
line_ay, = ax1.plot([], [], label="ay", color="green")
line_az, = ax1.plot([], [], label="az", color="blue")

ax1.set_xlim(0, WINDOW_SIZE)
ax1.set_ylim(-15, 15)
ax1.set_title("Accelerometer Data (m/s²)")
ax1.legend(loc="upper right")
ax1.grid(True)

line_gyrX, = ax2.plot([], [], label="gyrX", color="red")
line_gyrY, = ax2.plot([], [], label="gyrY", color="green")
line_gyrZ, = ax2.plot([], [], label="gyrZ", color="blue")

ax2.set_xlim(0, WINDOW_SIZE)
ax2.set_ylim(-500, 500)
ax2.set_title("Gyroscope Data (°/s)")
ax2.legend(loc="upper right")
ax2.grid(True)

text_display1 = ax1.text(0.02, 0.95, "", transform=ax1.transAxes,
                         verticalalignment='top', fontsize=10,
                         bbox=dict(boxstyle="round", facecolor="white", alpha=0.8))

text_display2 = ax2.text(0.02, 0.95, "", transform=ax2.transAxes,
                         verticalalignment='top', fontsize=10,
                         bbox=dict(boxstyle="round", facecolor="white", alpha=0.8))


# =====================
# TODO (6): Parse serial line
# =====================
# HINT: New serial format from Arduino:
# "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
#
def parse_line(line):
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    try:
        # HINT: Serial format: "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
        # TODO: Split by '|' and extract 6 sensor values (ax, ay, az, gyrX, gyrY, gyrZ)
        # TODO: Extract 3 text strings (orientation_text, fsm_gesture_text, gyro_gesture_text)
        # TODO: Return tuple (ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val)

        parts = line.split('|')
        parts = [p.strip() for p in parts]

        values = {}
        for part in parts:
            if ':' not in part:
                continue
            label, value = part.split(':', 1)
            values[label.strip()] = value.strip()

        ax_val = float(values['ax'])
        ay_val = float(values['ay'])
        az_val = float(values['az'])
        gyrX_val = float(values['gyrX'])
        gyrY_val = float(values['gyrY'])
        gyrZ_val = float(values['gyrZ'])

        orientation_text = values['Orientation']
        fsm_gesture_text = values['Accelerometer Gesture']
        gyro_gesture_text = values['Gyro Gesture']

        return (ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val)

    except Exception:
        return None


# =====================
# TODO (7): Update function
# =====================
def update(frame):
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    # TODO: Read all available serial data, parse, and append to buffers
    # TODO: Update all 6 line objects (3 accel + 3 gyro)
    # TODO: Update both text displays
    # TODO: Return all plot objects to redraw

    while ser.in_waiting:
        try:
            raw_line = ser.readline().decode('utf-8', errors='ignore').strip()
        except Exception:
            continue

        if not raw_line:
            continue

        parsed = parse_line(raw_line)
        if parsed is None:
            continue

        ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val = parsed

        ax_data.append(ax_val)
        ay_data.append(ay_val)
        az_data.append(az_val)
        gyrX_data.append(gyrX_val)
        gyrY_data.append(gyrY_val)
        gyrZ_data.append(gyrZ_val)

    x_axis = range(len(ax_data))

    line_ax.set_data(x_axis, ax_data)
    line_ay.set_data(x_axis, ay_data)
    line_az.set_data(x_axis, az_data)

    line_gyrX.set_data(x_axis, gyrX_data)
    line_gyrY.set_data(x_axis, gyrY_data)
    line_gyrZ.set_data(x_axis, gyrZ_data)

    text_display1.set_text(f"Orientation: {orientation_text}\nFSM: {fsm_gesture_text}")
    text_display2.set_text(f"Gyro: {gyro_gesture_text}")

    return [line_ax, line_ay, line_az, line_gyrX, line_gyrY, line_gyrZ,
            text_display1, text_display2]


# =====================
# TODO (8): Create animation
# =====================
# TODO: Create animation with FuncAnimation(fig, update, interval=50)
# TODO: Call plt.tight_layout() for proper spacing
ani = animation.FuncAnimation(fig, update, interval=50, blit=False)
plt.tight_layout()

plt.show()
