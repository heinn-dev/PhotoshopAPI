"""
Type stubs for AdjustmentLayer classes.

AdjustmentLayer represents Photoshop adjustment layers (Curves, Levels, etc.).
These are read-only and primarily used for type identification and mask access.
"""

from ._layer import Layer_8bit, Layer_16bit, Layer_32bit


class AdjustmentLayer_8bit(Layer_8bit):
    """
    Represents an 8-bit Adjustment Layer (Curves, Levels, Brightness/Contrast, etc.)

    This class is read-only and primarily used for type identification and mask access.
    All properties are inherited from Layer_8bit.
    """
    pass


class AdjustmentLayer_16bit(Layer_16bit):
    """
    Represents a 16-bit Adjustment Layer (Curves, Levels, Brightness/Contrast, etc.)

    This class is read-only and primarily used for type identification and mask access.
    All properties are inherited from Layer_16bit.
    """
    pass


class AdjustmentLayer_32bit(Layer_32bit):
    """
    Represents a 32-bit Adjustment Layer (Curves, Levels, Brightness/Contrast, etc.)

    This class is read-only and primarily used for type identification and mask access.
    All properties are inherited from Layer_32bit.
    """
    pass
