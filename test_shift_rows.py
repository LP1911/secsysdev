import ctypes
import random
import unittest

# Import Python reference
from aes import bytes2matrix, matrix2bytes, shift_rows as py_shift_rows, inv_shift_rows as py_inv_shift_rows

class TestShiftRows(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Loads the DLL 
        cls.c_lib = ctypes.CDLL('./rijndael.dll')
        
        # shift_rows function prototypes
        cls.c_lib.shift_rows.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
        cls.c_lib.shift_rows.restype  = None
        
        # invert_shift_rows function prototypes
        cls.c_lib.invert_shift_rows.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
        cls.c_lib.invert_shift_rows.restype  = None

    def c_call(self, func, data: bytes) -> bytes:
        arr = (ctypes.c_ubyte * 16)(*data)
        func(arr)
        return bytes(arr)

    def test_shift_rows(self):
        # Generate3 3 random blocks and tests each one
        for _ in range(3):
            block = bytes(random.getrandbits(8) for _ in range(16))
            
            # C result
            cResults = self.c_call(self.c_lib.shift_rows, block)
            
            # Python reference
            mat = bytes2matrix(block)
            py_shift_rows(mat)
            pyResults = matrix2bytes(mat)
            
            self.assertEqual(cResults, pyResults, 
                             f"shift_rows mismatch.\n"
                             f"Input: {block}\nC out: {cResults}\nPy out: {pyResults}")

    def test_invert_shift_rows(self):
        for _ in range(3):
            block = bytes(random.getrandbits(8) for _ in range(16))
            
            # C result
            cResults = self.c_call(self.c_lib.invert_shift_rows, block)
            
            # Python reference
            mat = bytes2matrix(block)
            py_inv_shift_rows(mat)
            pyResults = matrix2bytes(mat)
            
            self.assertEqual(cResults, pyResults,
                             f"invert_shift_rows mismatch.\n"
                             f"Input: {block}\nC out: {cResults}\nPy out: {pyResults}")

if __name__ == '__main__':
    unittest.main()
