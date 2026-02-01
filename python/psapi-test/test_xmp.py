import unittest
import os
import numpy as np
import photoshopapi as psapi

class TestXMPMetadata(unittest.TestCase):

    def _get_output_path(self, file_name: str) -> str:
        base_path = os.path.dirname(os.path.abspath(__file__))
        output_dir = os.path.join(base_path, "image_data")
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        return os.path.join(output_dir, file_name)

    def test_roundtrip_xmp_metadata(self):
        # Create a simple 8-bit layered file
        layered_file = psapi.LayeredFile_8bit(psapi.enum.ColorMode.rgb, 100, 100)
        
        # Define some XMP metadata (dummy XML)
        xmp_data = b'<x:xmpmeta xmlns:x="adobe:ns:meta/"><rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns/"></rdf:RDF></x:xmpmeta>'
        xmp_array = np.frombuffer(xmp_data, dtype=np.uint8)
        
        # Set the XMP metadata
        layered_file.xmp_metadata = xmp_array

        # Add a dummy layer because Photoshop files require at least one layer
        img_data = np.zeros((3, 100, 100), dtype=np.uint8)
        layer = psapi.ImageLayer_8bit(img_data, "Layer Red", width=100, height=100)
        layered_file.add_layer(layer)
        
        # Write the file
        output_path = self._get_output_path("test_xmp_roundtrip.psd")
        layered_file.write(output_path)
        
        # Read the file back
        read_file = psapi.LayeredFile_8bit.read(output_path)
        
        # Verify XMP metadata
        read_xmp = read_file.xmp_metadata
        self.assertIsNotNone(read_xmp)
        self.assertTrue(np.array_equal(read_xmp, xmp_array))
        
        # Verify text content
        self.assertEqual(read_xmp.tobytes(), xmp_data)

    def test_empty_xmp(self):
        layered_file = psapi.LayeredFile_8bit(psapi.enum.ColorMode.rgb, 100, 100)
        # Should be empty initially
        xmp = layered_file.xmp_metadata
        self.assertEqual(xmp.size, 0)

    def test_set_xmp_from_bytes(self):
         # Create a simple 8-bit layered file
        layered_file = psapi.LayeredFile_8bit(psapi.enum.ColorMode.rgb, 100, 100)
        
        # Define some XMP metadata (dummy XML)
        xmp_data = b'<x:xmpmeta>Test</x:xmpmeta>'
        xmp_array = np.frombuffer(xmp_data, dtype=np.uint8)
        
        # Set the XMP metadata using numpy array (as that's what we exposed)
        layered_file.xmp_metadata = xmp_array
        
        self.assertTrue(np.array_equal(layered_file.xmp_metadata, xmp_array))

if __name__ == '__main__':
    unittest.main()
