import ctypes
import random
import unittest

# This program was created to unittest the c code (rijndael) to the python aes code 

# Takes the helper functions from aes.py (Assuming the 16-byte block is set as a 4x4 matrix)
from aes import bytes2matrix, matrix2bytes, sub_bytes as py_sub_bytes, inv_sub_bytes as py_inv_sub_bytes

class TestSubBytes(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Loads the DLL
        cls.c_lib = ctypes.CDLL('./rijndael.dll')
        # Sets the function prototypes for sub_bytes.
        cls.c_lib.sub_bytes.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
        cls.c_lib.sub_bytes.restype = None
        # Same for invert_sub_bytes.
        cls.c_lib.invert_sub_bytes.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
        cls.c_lib.invert_sub_bytes.restype = None

# Takes the 16-byte block from python into a ctype array
# Calls the C function to returns the modified bytes
    def cFunc(self, func, data: bytes) -> bytes:
        array = (ctypes.c_ubyte * 16)(*data)
        func(array)
        return bytes(array)

    def test_sub_bytes(self):

        # Tests sub_bytes on 3 random 16-byte blocks.
        for i in range(3):
            data = bytes(random.getrandbits(8) for _ in range(16))
            # Call the C implementation.
            cResults = self.cFunc(self.c_lib.sub_bytes, data)

            # Converts the data to a 4x4 matrix then applies py_sub_bytes, and then flattens it.
            matrix = bytes2matrix(data)
            py_sub_bytes(matrix)
            pyResults = matrix2bytes(matrix)
            # Checks to see if the python and c results are equal
            self.assertEqual(cResults, pyResults,
                             f"The sub_bytes didn't match {data}\n"
                             f"C output: {cResults}\nPython output: {pyResults}")

    def test_invert_sub_bytes(self):
        # Test invert_sub_bytes on 3 random 16-byte blocks.
        for i in range(3):
            data = bytes(random.getrandbits(8) for _ in range(16))
            cResults = self.cFunc(self.c_lib.invert_sub_bytes, data)

            matrix = bytes2matrix(data)
            py_inv_sub_bytes(matrix)
            pyResults = matrix2bytes(matrix)
            self.assertEqual(cResults, pyResults,
                             f"The inverted_sub_bytes didn't match {data}\n"
                             f"C output: {cResults}\nPython output: {pyResults}")

if __name__ == '__main__':
    unittest.main()
