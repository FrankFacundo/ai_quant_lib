import numpy as np
import matplotlib.pyplot as plt
import pyquant as pq
from datetime import datetime, timedelta

np.random.seed(42)

# Simulate price path
n = 250
mu, sigma = 0.0002, 0.01
prices = [100.0]
for _ in range(1, n):
    prices.append(prices[-1] * np.exp((mu - 0.5 * sigma ** 2) + sigma * np.random.randn()))

bars = pq.backtest.BarSeries()
start = datetime(2021, 1, 1)
for i, p in enumerate(prices):
    current = start + timedelta(days=i)
    dt = pq.DateTime(current.year, current.month, current.day)
    bar = pq.backtest.Bar(dt, p, p, p, p, 0.0)
    bars.push_back(dt, bar)

strategy = pq.backtest.MovingAverageCrossStrategy(short_window=5, long_window=20, qty=1)
portfolio = pq.backtest.Portfolio(0.0)
data = {"EQ": bars}
backtester = pq.backtest.Backtester(data, strategy, portfolio)
result = backtester.run()

plt.plot([v for v in result.equity_curve.values()], label="equity")
plt.title("Moving average backtest")
plt.show()
