import plotly.graph_objects as go

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

# Create 3D plot
fig = go.Figure()

# Add trace
fig.add_trace(go.Scatter3d(
    x=x_values, y=y_values, z=t_values,
    mode='lines+markers',
    line=dict(color='blue', width=2),
    marker=dict(size=5, color=t_values, colorscale='Viridis', opacity=0.8)
))

# Update layout
fig.update_layout(
    scene=dict(
        xaxis_title='X Coordinate',
        yaxis_title='Y Coordinate',
        zaxis_title='Time (t)'
    ),
    title='3D Visualization of Data Points Over Time'
)

# Show plot
fig.show()
