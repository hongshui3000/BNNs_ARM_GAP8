Li-Yung, Chen. 2018 Spring Semester. Masther thesis at ETH Zurich

Topic: Implementation and Optimization of BNNs in Embedded Platform and IoT

Precalculate the threshold 1, threshold 2, sign(BetaStar) as thresholds.
This script precalculate the parameters like alpha, beta, gamma, mu, sigma from scaling and batch norm to the thresholds.
The binary weights are already pack into 32-bits integer.
The order of the data is match to GAP8 code, not for ARM_XNOR verison.