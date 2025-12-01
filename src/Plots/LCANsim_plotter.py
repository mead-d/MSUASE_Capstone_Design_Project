import csv
import os
import matplotlib.pyplot as plt


# Define file names
input_csv = 'LCANsim_data.csv'
output_image = 'plot1.jpeg'

group_pos = {} # set of x, y positions sorted by id
group_trgt = {} # set of target positions sorted by id

# Sim environment dimension (square region)
sim_dim = 20

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

# print(f"target set:\n {group_trgt}")
# print(f"position set:\n {group_pos}")
            
# Plot data with different colors, connected points, and legend for legibility
plt.figure(figsize=(sim_dim, sim_dim))
plt.title("LCANsim Formation Change")
plt.xlabel("X-Axis")
plt.ylabel("Y-Axis")

plt.xlim(-1*(sim_dim/2.0), (sim_dim/2.0))
plt.ylim(-1*(sim_dim/2.0), (sim_dim/2.0))
plt.grid(True, linestyle='--', alpha=0.5)

# Generate a color map
colors = plt.cm.get_cmap('tab10', len(group_pos))

for i, (node_id, sets_list) in enumerate(group_pos.items()):
    x_vals = []
    y_vals = []

    for s in sets_list:
        elements = list(s)
        if len(elements) > 1:
            x, y = elements[0], elements[1]

        x_vals.append(x)
        y_vals.append(y)
    
    plt.scatter(x_vals, y_vals, label= node_id, color= colors(i), s=60, alpha=0.7, edgecolors='white')
    plt.plot(x_vals, y_vals)

plt.legend(title= "Node IDs", loc= "upper right")
plt.savefig(output_image)