# PhotoshopAPI

Currently PhotoshopAPI doesn't preserve data from Adjustment Layers when reading and writing back to the same file, resulting in empty layers. It also doesn't expose Python bindings for Adjustment Layer masks. I only really need access to masks and for the data to be preservedg so this is my attempt to patch these things for a Blender add-on I'm working on.


[PhotoshopAPI](https://github.com/EmilDohne/PhotoshopAPI)