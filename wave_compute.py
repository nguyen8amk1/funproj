import numpy as np
import matplotlib.pyplot as plt

# Read wave data from file
def read_wave_from_file(filename):
    times, values = [], []
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            if len(parts) == 2:
                time, value = map(float, parts)
                times.append(time)
                values.append(value)
    return np.array(times), np.array(values)

# Find zero crossings
def find_zero_crossings(wave):
    crossings = []
    for i in range(1, len(wave)):
        if wave[i - 1] <= 0 < wave[i] or wave[i - 1] >= 0 > wave[i]:
            crossings.append(i)
    return crossings

# Extract cycles using zero crossings
def extract_cycles(times, wave, crossings):
    cycles = []
    for i in range(len(crossings) - 2):  # Ensure full cycles
        start, end = crossings[i], crossings[i + 2]
        cycle_time = times[start:end]
        cycle_values = wave[start:end]

        # Normalize length to avoid distortion
        resampled_time = np.linspace(0, 1, 100)
        resampled_values = np.interp(resampled_time, np.linspace(0, 1, len(cycle_values)), cycle_values)

        # Normalize to prevent flattening
        resampled_values -= np.mean(resampled_values)  # Center around 0
        resampled_values /= np.max(np.abs(resampled_values))  # Normalize amplitude

        cycles.append(resampled_values)
    
    return np.array(cycles)

# Compute the average cycle
def compute_average_cycle(cycles):
    return np.mean(cycles, axis=0) * np.max(np.abs(cycles))  # Scale back amplitude

# Plot results
def plot_cycles(cycles, avg_cycle):
    plt.figure(figsize=(10, 5))
    for cycle in cycles:
        plt.plot(np.linspace(0, 1, len(cycle)), cycle, color='gray', alpha=0.3)
    plt.plot(np.linspace(0, 1, len(avg_cycle)), avg_cycle, color='red', linewidth=2, label="Averaged Cycle")
    plt.xlabel("Normalized Time")
    plt.ylabel("Wave Amplitude")
    plt.legend()
    plt.show()

# Main execution
filename = "wave.txt"
times, wave = read_wave_from_file(filename)
wave -= np.mean(wave)  # Center the wave
crossings = find_zero_crossings(wave)
cycles = extract_cycles(times, wave, crossings)
avg_cycle = compute_average_cycle(cycles)
plot_cycles(cycles, avg_cycle)
