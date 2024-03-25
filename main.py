import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
import numpy as np

plt.style.use("ggplot")

# temp_data = np.loadtxt("res/temp_data_2D_T10.csv", delimiter=";")
temp_data = np.loadtxt("res/temp_data.csv", delimiter=";")
magn_data = np.loadtxt("res/magn_data.csv", delimiter=";")

fig1 = plt.figure(1, figsize=(12, 8))
fig2 = plt.figure(2, figsize=(12, 8))

fig1.suptitle("Modèle d'Ising avec variation sur $T$")
fig2.suptitle("Modèle d'Ising avec variation sur $h$")

gs1 = GridSpec(3, 2, figure=fig1)
gs2 = GridSpec(2, 2, figure=fig2)

# Affichage des données selon champ magnétique
am1 = fig2.add_subplot(gs2[0, 0])
am2 = fig2.add_subplot(gs2[0, 1])
am3 = fig2.add_subplot(gs2[1, 0])
am4 = fig2.add_subplot(gs2[1, 1])

am1.plot(magn_data[:, 0], magn_data[:, 1])
am1.set_ylabel("$<E>$ [UA]")

am2.plot(magn_data[:, 0], magn_data[:, 3])
am2.set_ylabel("$<M>$ [%]")

am3.plot(magn_data[:, 0], magn_data[:, 2])
am3.set_ylabel("$<E^2>$")
am3.set_xlabel("Champ magnétique $h$")

am4.plot(magn_data[:, 0], magn_data[:, 4])
am4.set_ylabel("$<M^2>$")
am4.set_xlabel("Champ magnétique $h$")

# Affichage des données selon température
at1 = fig1.add_subplot(gs1[0, 0])
at2 = fig1.add_subplot(gs1[0, 1])
at3 = fig1.add_subplot(gs1[1, 0])
at4 = fig1.add_subplot(gs1[1, 1])
at5 = fig1.add_subplot(gs1[2, 0])
at6 = fig1.add_subplot(gs1[2, 1])

at1.plot(temp_data[:, 0], temp_data[:, 1])
at1.set_ylabel("$<E>$ [UA]")

at2.plot(temp_data[:, 0], temp_data[:, 3])
at2.set_ylabel("$<M>$ [%]")

at3.plot(temp_data[:, 0], temp_data[:, 2])
at3.set_ylabel("$<E^2>$")

at4.plot(temp_data[:, 0], temp_data[:, 4])
at4.set_ylabel("$<M^2>$")

at6.plot(temp_data[:, 0], temp_data[:, 7])
at6.set_ylabel("$\\chi$")
at6.set_xlabel("Température $T$")

at5.plot(temp_data[:, 0], temp_data[:, 6])
at5.set_ylabel("$C_V$")
at5.set_xlabel("Température $T$")


plt.show()