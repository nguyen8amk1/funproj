import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d

def generate_balanced_wave(sample_rate=1000, duration=1, randomness=0.5):
    """Generate a randomized wave that always has both positive and negative parts."""
    t = np.linspace(0, duration, sample_rate * duration, endpoint=False)
    wave = np.sin(2 * np.pi * 3 * t)  
    
    for _ in range(5):
        freq = np.random.uniform(1, 10)  # Random frequency
        phase = np.random.uniform(0, 2 * np.pi)
        amp = np.random.uniform(0.3, 1.0) * randomness
        wave += amp * np.sin(2 * np.pi * freq * t + phase)

    wave -= np.mean(wave)  # Center the wave around zero
    return t, wave

def find_zero_crossings(t, wave):
    """Find indices where the wave crosses y=0."""
    zero_crossings = np.where(np.diff(np.sign(wave)))[0]  
    return zero_crossings

def split_into_full_cycles(t, wave, zero_crossings):
    """Split wave into full cycles (two zero crossings)."""
    cycles = []
    for i in range(len(zero_crossings) - 2):
        start, end = zero_crossings[i], zero_crossings[i + 2]
        cycles.append((t[start:end] - t[start], wave[start:end]))  # Normalize time to start at 0
    return cycles

def interpolate_cycles(cycles, num_points=100):
    """Resample all cycles to a fixed number of points."""
    resampled_cycles = []
    for t_cycle, wave_cycle in cycles:
        interp_func = interp1d(t_cycle, wave_cycle, kind='linear', fill_value="extrapolate")
        t_uniform = np.linspace(0, t_cycle[-1], num_points)
        wave_uniform = interp_func(t_uniform)
        resampled_cycles.append(wave_uniform)
    return np.array(resampled_cycles), np.linspace(0, np.mean([c[0][-1] for c in cycles]), num_points)

def visualize_stacked_cycles(cycles, avg_cycle, t_avg):
    """Plot stacked cycles and their average."""
    plt.figure(figsize=(10, 5))
    
    # Plot all individual cycles
    for cycle in cycles:
        plt.plot(np.linspace(0, 1, len(cycle)), cycle, color='gray', alpha=0.3)
    
    # Plot the average cycle
    plt.plot(np.linspace(0, 1, len(avg_cycle)), avg_cycle, color='red', linewidth=2, label="Averaged Cycle")
    
    plt.title("Stacked Cycles and Averaged Cycle")
    plt.xlabel("Normalized Time")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.show()

# Run the steps
t, wave = generate_balanced_wave()
zero_crossings = find_zero_crossings(t, wave)
cycles = split_into_full_cycles(t, wave, zero_crossings)
resampled_cycles, t_avg = interpolate_cycles(cycles)

# Compute the average cycle
avg_cycle = np.mean(resampled_cycles, axis=0)

# Visualize
visualize_stacked_cycles(resampled_cycles, avg_cycle, t_avg)
