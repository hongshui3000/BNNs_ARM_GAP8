## File Organization

* /XNOR_Eclips : Run BNN and MFCC on stm32f469. Eclipse IDE is needed.

* /forward : Run BNN for host machine. Selecting the correct path or including the needed header for the input data. For more information see the README inthis folder.

* /model_preprocess_for_ARM: the script for precalculating the thresholds.

* /preprocess: MFCC preprocessing based on Aquila DSP library. The configuration is same in LI-YUNG's MFCC preprocessing. Open Source Aquila library is available: https://aquila-dsp.org/

* /stm32f4: the drivers (which is not used. In /XNOR_Eclips, the driver for Eclipse IDE is used instead.)

* /train: network for training using Torch. (This is not used and replaced by PyTorch.)

* /unrolling: generate code for for unrolling.