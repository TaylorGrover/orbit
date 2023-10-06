import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
import sys

plt.ion()

fig = plt.figure(figsize=(16, 9))
ax = fig.add_subplot(projection="3d")
fig.subplots_adjust(top=1.1, bottom=-.01)

# The x-axis is fine, but y should actually be the z-axis, and z should be -y
x_axis = [[0, 1], [0, 0], [0, 0]]
y_axis = [[0, 0], [0, 0], [0, 1]]
z_axis = [[0, 0], [0,-1], [0, 0]]

x_line, = ax.plot(*x_axis, color="blue")
y_line, = ax.plot(*y_axis, color="red")
z_line, = ax.plot(*z_axis, color="green")

def read_data():
    with open("camera_out.txt", "r") as camera_file:
        data = camera_file.readlines()
        if len(data) > 0:
            vals = data[-1].split()
            u = np.array(list(map(float, vals[1:4])))
            v = np.array(list(map(float, vals[5:8])))
            return u, v

