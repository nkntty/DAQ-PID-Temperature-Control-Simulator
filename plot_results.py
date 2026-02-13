import pandas as pd
import matplotlib.pyplot as plt


df = pd.read_csv("data.csv")

plt.figure(figsize=(8,5))

plt.plot(df["time"], df["temp"], label="Temperature")
plt.plot(df["time"], df["setpoint"], linestyle="--", label="Setpoint")

plt.title("PID Temperature Control Simulation")
plt.xlabel("Time")
plt.ylabel("Temperature")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()

# Optional quick stats
print(df.describe())
