import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
import numpy as np

XY = 15*15

E = np.loadtxt("E.csv", delimiter=";")
E_sq = np.loadtxt("E_sq.csv", delimiter=";")
M = np.loadtxt("M.csv", delimiter=";")
M_sq = np.loadtxt("M_sq.csv", delimiter=";")
M_abs = np.loadtxt("M_abs.csv", delimiter=";")
T = np.loadtxt("T.csv", delimiter=";")

fig = plt.figure()
gs = GridSpec(2, 2, figure=fig)

ax1 = fig.add_subplot(gs[0, 0])
ax2 = fig.add_subplot(gs[1, 0])
ax3 = fig.add_subplot(gs[0, 1])
ax4 = fig.add_subplot(gs[1, 1])

ax1.plot(T, E)
ax2.plot(T, E_sq)
ax3.plot(T, M_abs)
ax4.plot(T, M_sq)

ax2.set_xlabel("Température [K]")
ax2.set_ylabel("$E^2$")
ax1.set_ylabel("$E$")
ax4.set_xlabel("Température [K]")
ax4.set_ylabel("$M^2$")
ax3.set_ylabel("$|M|$")

fig.suptitle("Propriétés de la grille en fonction de la température")

plt.show()