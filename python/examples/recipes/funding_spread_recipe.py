import pyquant as pq

times = [1, 2, 3, 5, 10]
ois = pq.YieldCurve(times, [0.015, 0.017, 0.018, 0.019, 0.02])
libor = pq.YieldCurve(times, [0.02, 0.022, 0.023, 0.024, 0.025])

for t in times:
    spread = pq.funding_spread(ois, libor, t)
    print(f"Funding spread at {t}y: {spread*100:.2f} bps")
