import matplotlib.pyplot as plt
import pyquant as pq

# Simple curve points
times = [0.5, 1.0, 2.0, 5.0, 10.0]
rates = [0.02, 0.022, 0.025, 0.028, 0.03]
curve = pq.YieldCurve(times, rates)

spots = pq.spot_rates(curve, times)
forwards = pq.forward_rates(curve, times)
print("Spot rates", spots)
print("Forward rates", forwards)

plt.plot(times, spots, label="spot")
plt.plot(times[1:], forwards, label="forward")
plt.legend()
plt.title("Yield curve")
plt.show()
