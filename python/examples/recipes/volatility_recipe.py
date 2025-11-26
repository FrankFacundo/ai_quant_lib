import numpy as np
import matplotlib.pyplot as plt
import pyquant as pq
from datetime import datetime, timedelta

np.random.seed(7)
returns = np.random.normal(scale=0.01, size=400)
ts = pq.TimeSeriesDouble()
start = datetime(2020, 1, 1)
for i, r in enumerate(returns):
    d = start + timedelta(days=i)
    ts.push_back(pq.DateTime(d.year, d.month, d.day), float(r))

rv = pq.realized_vol(ts, window=30)
rv_series = pq.realized_vol_series(ts, window=30)
print("Latest realized vol (annualized):", rv)

plt.plot([v for v in rv_series.values()])
plt.title("Realized vol (30d window)")
plt.show()
