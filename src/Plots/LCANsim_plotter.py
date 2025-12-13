import csv
import os
import numpy as np
import matplotlib.pyplot as plt

# Define file names
input_csv = 'LCANsim_v1-0_data.csv'
output_image = 'forming_v1-0.jpeg'

group_pos = {} # set of x, y positions sorted by id
group_trgt = {} # set of target positions sorted by id

# Sim environment dimension (square region)
sim_dim = 20
axis_stepsize = 1

# Define column names to use
Node = 'Node'
x_pos = 'x_pos'
y_pos = 'y_pos'
x_trgt = 'target_x'
y_trgt = 'target_y'

# Read and sort data
with open(input_csv, mode= 'r', newline='') as csvfile:
    reader = csv.reader(csvfile)
    header = next(reader)
    for row in reader:
        node_id = row[0]
        position = [row[1], row[2]]
        target = [row[3], row[4]]

        if node_id not in group_pos:
            group_pos[node_id] = []
        if node_id not in group_trgt:
            group_trgt[node_id] = []

        group_pos[node_id].append(position)
        group_trgt[node_id].append(target)            

# Create the plot
plt.figure(figsize=(sim_dim, sim_dim))

# Generate a color map
colors = plt.cm.get_cmap('tab10', len(group_pos))

# Plot data with different colors, connected points
for i, (node_id, sets_list) in enumerate(group_pos.items()):
    x_vals = []
    y_vals = []

    for s in sets_list:
        elements = list(s)
        if len(elements) > 1:
            x, y = elements[0], elements[1]

        x_vals.append(float(x))
        y_vals.append(float(y))
    
    plt.scatter(x_vals, y_vals, label= node_id, s=60, alpha=0.7, edgecolors='white', color= colors(int(node_id)))
    plt.plot(x_vals, y_vals)

x_vals = []
y_vals = []
for i, (node_id, sets_list) in enumerate(group_trgt.items()):
    for s in sets_list:
        elements = list(s)
        if len(elements) > 1:
            x, y = elements[0], elements[1]
        x_vals.append(float(x))
        y_vals.append(float(y))
plt.scatter(x_vals, y_vals, s=100, c='red', marker='x', linewidth=3)        


plt.xlim(-1*(sim_dim/2.0), (sim_dim/2.0))
plt.ylim(-1*(sim_dim/2.0), (sim_dim/2.0))
ticks = np.arange(-1*(sim_dim/2), (sim_dim/2)+axis_stepsize, axis_stepsize) # Includes correct endpoint
plt.xticks(ticks)
plt.yticks(ticks)

plt.title("LCANsim Formation Change")
plt.xlabel("X-Axis")
plt.ylabel("Y-Axis")
plt.axhline(0, color='gray', linewidth=0.5) # Add x=0 axis line
plt.axvline(0, color='gray', linewidth=0.5) # Add y=0 axis line
plt.grid(True, linestyle='--', alpha=0.6)
plt.legend(title= "Node IDs", loc= "upper right")
plt.gca().set_aspect('equal', adjustable='box')
plt.savefig(output_image)