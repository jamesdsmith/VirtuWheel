import tkinter
import numpy as np
import serial
import json

window = tkinter.Tk()
canvas = tkinter.Canvas(window, bg='white', highlightthickness=0)
# signal = np.array([30, 0, 0, 4, 5, 5, 4, 6, 0, 1])

yscale = 10
r = 3
thresh = 6

class TouchPoint:
    def __init__(self):
        self.start = None
        self.length = 0
    def __repr__(self):
        return "[start: {}, length: {}]".format(self.start, self.length)

def process(a):
    length = 0
    start = None
    end = None
    touch_pts = []
    new_touch = TouchPoint()
    for i, v in enumerate(a):
        if v >= thresh:
            if new_touch.start is None:
                new_touch.start = i
        else:
            if new_touch.start is not None:
                new_touch.length = i - new_touch.start
                touch_pts.append(new_touch)
            new_touch = TouchPoint()
    if new_touch.start is not None:
        new_touch.length = len(a) - new_touch.start
        touch_pts.append(new_touch)
    if len(touch_pts) > 0 and touch_pts[0].start == 0 and touch_pts[-1].start + touch_pts[-1].length == len(a):
        touch_pts[-1].length += touch_pts[0].length
        del touch_pts[0]
    return [(2 * t.start + t.length - 1) / 2  % len(a) for t in touch_pts]

def loop():
    signal = np.array([30, 0, 0, 4, 5, 5, 4, 6, 0, 1, 0])
    try:
        serial_data = ser.readline().decode('utf-8')
        print(serial_data)
        data = json.loads(serial_data)
        print(data)
        if 'touch_points' in data:
            signal = data['touch_points']
    except ValueError as e:
        print(e)
    touch_pts = process(signal)
    window.update()
    h = canvas.winfo_height()
    w = canvas.winfo_width()
    xscale = (w - 10) / len(signal)
    canvas.delete('all')
    ypad = 10
    canvas.create_line(0, h - ypad, w, h - ypad, width=2, fill='black', dash=(4,4))
    x = w - xscale / 2
    for v in signal:
        y = h - v * yscale
        canvas.create_line(x, h - ypad, x, y - ypad, width=2, fill='black')
        canvas.create_oval(x - r, y - r - ypad, x + r, y + r - ypad, outline='green')
        x -= xscale
    canvas.create_line(0, h - yscale*thresh - ypad, canvas.winfo_width(), h - yscale*thresh - ypad, fill='red', dash=(4,4))
    for t in touch_pts:
        canvas.create_line(w - (t * xscale + xscale / 2), 0, w - (t * xscale + xscale / 2), h, fill="blue", dash=(8,8))
    window.after(33, loop)
canvas.pack(fill='both', expand=True)

ser = serial.Serial('COM4', 115200, timeout=1)
loop()
window.mainloop()

