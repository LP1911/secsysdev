import unittest
import ctypes
import random
from aes import bytes2matrix, matrix2bytes, mix_columns as py_mix_columns, inv_mix_columns as py_inv_mix_columns

class TestMixColumns(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.c_lib = ctypes.CDLL('./rijndael.dll')
        cls.c_lib.mix_columns.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
        cls.c_lib.mix_columns.restype  = None

        cls.c_lib.invert_mix_columns.argtypes = [ctypes.POINTER(ctypes.c_ubyte)]
        cls.c_lib.invert_mix_columns.restype  = None

    def call_c(self, func, block: bytes) -> bytes:
        arr = (ctypes.c_ubyte * 16)(*block)
        func(arr)
        return bytes(arr)

    def test_mix_columns(self):
        for _ in range(3):
            block = bytes(random.getrandbits(8) for _ in range(16))
            cResults = self.call_c(self.c_lib.mix_columns, block)
            # Python reference
            mat = bytes2matrix(block)
            py_mix_columns(mat)
            pyResults = matrix2bytes(mat)
            self.assertEqual(cResults, pyResults, "mix_columns did not match")

    def test_invert_mix_columns(self):
        for _ in range(3):
            block = bytes(random.getrandbits(8) for _ in range(16))
            cResults = self.call_c(self.c_lib.invert_mix_columns, block)
            mat = bytes2matrix(block)
            py_inv_mix_columns(mat)
            pyResults = matrix2bytes(mat)
            self.assertEqual(cResults, pyResults, "invert_mix_columns did not match")

if __name__ == '__main__':
    unittest.main()
