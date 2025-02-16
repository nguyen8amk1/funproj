import numpy as np
import matplotlib.pyplot as plt

def generate_balanced_wave(sample_rate=1000, duration=1, randomness=0.5):
    """Generate a randomized wave that always has both positive and negative parts."""
    t = np.linspace(0, duration, sample_rate * duration, endpoint=False)
    
    # Start with a base sine wave
    wave = np.sin(2 * np.pi * 3 * t)  
    
    # Add multiple sine waves with random frequencies & amplitudes
    for _ in range(5):
        freq = np.random.uniform(1, 10)  # Random frequency
        phase = np.random.uniform(0, 2 * np.pi)  # Random phase
        amp = np.random.uniform(0.3, 1.0) * randomness  # Random amplitude
        wave += amp * np.sin(2 * np.pi * freq * t + phase)
    
    # Subtract the mean to keep wave oscillating around zero
    wave -= np.mean(wave)
    
    return t, wave

def find_zero_crossings(t, wave):
    """Find indices where the wave crosses y=0 (both directions)."""
    zero_crossings = np.where(np.diff(np.sign(wave)))[0]  # Indices just before crossing
    return zero_crossings

def split_into_full_cycles(t, wave, zero_crossings):
    """Group zero crossings into pairs to create full wave cycles."""
    cycles = []
    for i in range(len(zero_crossings) - 2):  # Step by 2 to get full cycles
        start, end = zero_crossings[i], zero_crossings[i + 2]
        cycles.append((t[start:end], wave[start:end]))
    return cycles

def visualize_cycles(t, wave, cycles):
    """Visualize the wave and overlay its full cycles with different colors."""
    plt.figure(figsize=(10, 5))
    plt.plot(t, wave, 'k', alpha=0.3, label="Original Wave")

    colors = plt.cm.viridis(np.linspace(0, 1, len(cycles)))
    for i, (t_cycle, wave_cycle) in enumerate(cycles):
        plt.plot(t_cycle, wave_cycle, color=colors[i], label=f"Cycle {i+1}")

    plt.axhline(0, color='gray', linestyle='--', linewidth=0.5)
    plt.legend()
    plt.title("Randomized Wave Split into Full Cycles (Two Zero-Crossings per Cycle)")
    plt.xlabel("Time")
    plt.ylabel("Amplitude")
    plt.show()

# Run the steps
t, wave = generate_balanced_wave()
zero_crossings = find_zero_crossings(t, wave)
cycles = split_into_full_cycles(t, wave, zero_crossings)
visualize_cycles(t, wave, cycles)
