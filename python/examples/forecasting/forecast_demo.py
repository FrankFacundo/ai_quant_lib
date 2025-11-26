import numpy as np
import matplotlib.pyplot as plt
import pyquant as pq
from datetime import datetime, timedelta

np.random.seed(0)

# Simulate AR(1) process
n = 200
phi = 0.8
noise = np.random.normal(scale=0.5, size=n)
series = [0.0]
for i in range(1, n):
    series.append(phi * series[-1] + noise[i])

# Build TimeSeries
from datetime import datetime, timedelta
ts = pq.TimeSeriesDouble()
start = datetime(2020, 1, 1)
for i, v in enumerate(series):
    current = start + timedelta(days=i)
    dt = pq.DateTime(current.year, current.month, current.day)
    ts.push_back(dt, float(v))

model = pq.ARIMAModel(1, 0, 0)
model.fit(ts)
forecast = model.forecast(20)

plt.plot(series, label="history")
plt.plot(range(len(series), len(series)+len(forecast)), forecast, label="forecast")
plt.legend()
plt.title("ARIMA(1,0,0) forecast")
plt.show()
