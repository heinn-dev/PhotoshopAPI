#pragma once

#include "LayeredFile/LayerTypes/AdjustmentLayer.h"
#include "LayeredFile/LayerTypes/Layer.h"
#include "Macros.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>

#include <memory>

namespace py = pybind11;
using namespace NAMESPACE_PSAPI;

// Generate an AdjustmentLayer python class from our struct
// This is a minimal binding primarily for type checking (isinstance) and mask access
template <typename T>
void declare_adjustment_layer(py::module& m, const std::string& extension)
{
    using Class = AdjustmentLayer<T>;
    std::string className = "AdjustmentLayer" + extension;
    py::class_<Class, Layer<T>, std::shared_ptr<Class>> adjustmentLayer(m, className.c_str(), py::dynamic_attr());

    adjustmentLayer.doc() = R"pbdoc(

        Represents an Adjustment Layer (Curves, Levels, Brightness/Contrast, etc.)

        This class is read-only and primarily used for type identification and mask access.
        The adjustment layer data is preserved internally when reading/writing PSD files,
        but the adjustment parameters themselves are not exposed to Python.

        Attributes
        ----------

        name : str
            The name of the layer, cannot be longer than 255
        blend_mode : enum.BlendMode
            The blend mode of the layer
        opacity : float
            The layers opacity from 0.0 - 1.0
        width : int
            The width of the layer ranging up to 30,000 for PSD and 300,000 for PSB
        height : int
            The height of the layer ranging up to 30,000 for PSD and 300,000 for PSB
        center_x : float
            The center of the layer in regards to the canvas
        center_y : float
            The center of the layer in regards to the canvas
        is_locked: bool
            The locked state of the layer
        is_visible: bool
            Whether the layer is visible
        clipping_mask: bool
            Whether the layer is clipped to the one below.
        mask: np.ndarray
            The layers' mask channel, may be empty
        mask_disabled: bool
            Whether the mask is disabled. Ignored if no mask is present
        mask_relative_to_layer: bool
            Whether the masks position is relative to the layer. Ignored if no mask is present
        mask_default_color: int
            The masks' default color outside of the masks bounding box from 0-255. Ignored if no mask is present
        mask_density: int
            Optional mask density from 0-255. Ignored if no mask is present
        mask_feather: float
            Optional mask feather. Ignored if no mask is present
        mask_position: psapi.geometry.Point2D
            The masks' canvas coordinates. Ignored if no mask is present
        mask_width: int
            The masks' width
        mask_height: int
            The masks' height

	)pbdoc";

    // No constructor exposed - AdjustmentLayers are read-only from PSD files
    // All properties are inherited from Layer<T> base class which already binds:
    // - name, blend_mode, opacity, width, height, center_x, center_y
    // - is_locked, is_visible, clipping_mask
    // - All mask properties via bind_mask_mixin
}
