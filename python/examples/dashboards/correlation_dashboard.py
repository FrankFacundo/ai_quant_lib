import numpy as np
import matplotlib.pyplot as plt
import pyquant as pq
from datetime import datetime, timedelta

np.random.seed(1)

assets = ["rates", "fx", "equity", "energy"]
ts_map = {}
start = datetime(2022, 1, 1)
for idx, name in enumerate(assets):
    values = np.cumsum(np.random.normal(scale=0.01 + idx * 0.005, size=300))
    ts = pq.TimeSeriesDouble()
    for i, v in enumerate(values):
        d = start + timedelta(days=i)
        ts.push_back(pq.DateTime(d.year, d.month, d.day), float(v))
    ts_map[name] = ts

dashboard = pq.utils.MacroDashboard()
for k, v in ts_map.items():
    dashboard.add_series(k, v)
mat = np.array(dashboard.correlations())

fig, ax = plt.subplots()
cax = ax.imshow(mat, cmap="coolwarm")
ax.set_xticks(range(len(assets)))
ax.set_xticklabels(assets)
ax.set_yticks(range(len(assets)))
ax.set_yticklabels(assets)
fig.colorbar(cax)
ax.set_title("Cross-asset correlations")
plt.show()
