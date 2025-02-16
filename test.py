import matplotlib.pyplot as plt

# File name
file_name = 'recorded_data.txt'

# Read data from file
data = []
with open(file_name, 'r') as file:
    for line in file:
        parts = line.strip().split(', ')
        x = float(parts[0].split('=')[1])
        y = float(parts[1].split('=')[1])
        t = float(parts[2].split('=')[1])
        data.append((x, y, t))

# Extracting x, y, and t values
x_values = [point[0] for point in data]
y_values = [point[1] for point in data]
t_values = [point[2] for point in data]

# Plotting X vs Y coordinates
plt.figure(figsize=(10, 6))
plt.subplot(3, 1, 1)
plt.plot(x_values, y_values, marker='o', linestyle='-', color='b', label='Position (x, y)')
plt.scatter(x_values, y_values, c=t_values, cmap='viridis')
plt.colorbar(label='Time (t)')
plt.xlabel('X Coordinate')
plt.ylabel('Y Coordinate')
plt.title('X vs Y Coordinates')
plt.legend()
plt.grid(True)

# Plotting X vs Time (t) coordinates
plt.subplot(3, 1, 2)
plt.plot(t_values, x_values, marker='o', linestyle='-', color='r', label='X over Time')
plt.xlabel('Time (t)')
plt.ylabel('X Coordinate')
plt.title('X vs Time (t)')
plt.legend()
plt.grid(True)

# Plotting Y vs Time (t) coordinates
plt.subplot(3, 1, 3)
plt.plot(t_values, y_values, marker='o', linestyle='-', color='g', label='Y over Time')
plt.xlabel('Time (t)')
plt.ylabel('Y Coordinate')
plt.title('Y vs Time (t)')
plt.legend()
plt.grid(True)

# Adjust layout
plt.tight_layout()

# Show plot
plt.show()
