import numpy as np
import pandas
import matplotlib.pyplot as plt


if __name__ == '__main__':
    delta_t = 0.01
    lambda_max = 2.0
    times = np.zeros(101, dtype=np.float64)
    Fs = np.zeros((101, 3, 3), dtype=np.float64)
    Ps = np.zeros((101, 3, 3), dtype=np.float64)

    for n in range(times.shape[0]):
        times[n] = n * delta_t

    for n in range(1, Fs.shape[0]):
        Fs[n, 1, 1] = 1.0 + n * delta_t

    for n in range(1, Fs.shape[0]):
        Fs[n, 0, 0] = 1.0 / np.sqrt(Fs[n, 1, 1])
        Fs[n, 2, 2] = 1.0 / np.sqrt(Fs[n, 1, 1])
        Ps[n, 1, 1] = 1.0e6 * (Fs[n, 1, 1] - 1.0 / (Fs[n, 1, 1]**2))

    dict = {'time': times,
            'F_xx': Fs[:, 0, 0],
            'F_yy': Fs[:, 1, 1],
            'F_zz': Fs[:, 2, 2],
            'P_xx': Ps[:, 0, 0],
            'P_yy': Ps[:, 1, 1],
            'P_zz': Ps[:, 2, 2]}

    dataframe = pandas.DataFrame(dict)
    print(dataframe)
    dataframe.to_csv('./gold/neohookean_uniaxial_stress_out.csv', index=False)
    # plt.figure(1)
    # plt.plot(Fs[:, 0, 0], Ps[:, 0, 0])
    # plt.show()
